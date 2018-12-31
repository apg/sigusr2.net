% Serializing Work to be Done
% archive, meetup, java, #pinned
% 2011-04-15


_This post originally appeared on [Making Meetup][1]. It is being republished
here for archive purposes._

Activity feeds were a logical addition to [Meetup][2] when they were added,
and are still a valuable feature today. They provide a great summary for
members that are actively engaged with many Meetups. They also aide in a
member's discovery of new groups for themselves. In our usability tests of the
site, we constantly see people new to Meetup click on a group from the search
results page, and eye the group's activity feed to get a little bit more
insight into who is actually a member of the group.

But, I don't want to dwell on how great our activity feeds are. Instead, I'd
like to take a little look into how they work.

For starters, our feeds are built on top of [HBase][3]. Why HBase and not
[CouchDB][4], [Riak][5], [MongoDB][6], [Cassandra][7], _<some other shiny
thing that's "all the rage">_? Aside from being a scalable, fault tolerant
system with lots of users (and growing) and hordes of contributors (also
growing), in the Spring of 2009, when the NoSQL rebellion was first recruiting
new members, the choices for stable, viable options were slim.[[1]][8]

Interaction with feeds is done with a slightly modified version of [beeno][9],
which provides an interface to HBase that looks somewhat close to our custom
ORM for MySQL.

Layered above beeno is a set of classes that provide common abstractions--
_pull out all items for member A_, _associate this item to all members in
group B_ basic object oriented design, as well as a set of classes for
creating asynchronous tasks, which we use mostly for large write operations.

Up until a few months ago, all of these asynchronous write tasks happened on
the application server that the task was initiated from. That meant that if a
member posted a comment to a group such as the [NY Tech Meetup][10], the
application server would temporarily take a hit in performance while that
operation was carried out. Now maybe I'm playing up the performance "hit" a
bit, but for a group like the, NY Tech Meetup with close to 20,000 members,
that results in an explosion of writes that need to be carried out in HBase,
as well as a few potentially large queries in order to figure out exactly what
to write to HBase to begin with.

There was another issue though. If HBase was unable to perform an operation
due to a region server failure or some other issue, our only course of action
was to discard the operation instead of trying it again, or let the
application server's job queue[[2]][11] fill to the point of memory
exhaustion. Neither of these options is better than the other--they're both
pretty horrible, so we decided to bound the queue size and retry--the "best"
of both worlds. This isn't without it's issues too of course. Extended
downtime results in [RejectedExecutionException][12]'s and lost writes.
_*sigh*_

Enter enterprise messaging.

One solution to the issues outlined above which has the potential to minimize
data loss is to queue all write operations to a shared location and have a set
of worker tasks perform the operations.

Last October, we enlisted [Rabbit MQ][13] for this task.

If you make all the entities durable (queues, exchanges, bindings), have
enough memory and disk space, implement proper failover in the broker and
networking infrastructure, and ban wire cutters from your data center, it's a
pretty good bet that actual fires, and earthquakes are the only things that
will stop the write operations from getting off the application server and
into the bus.[[3]][14]

But, just to be sure, and in case failover takes longer than "instantly," we
queue up writes from the application servers to the Rabbit MQ broker.[[4]][15]

The new issue then becomes what do we send on the bus? The implication of
using an [Executor][16] is that we have [Runnable][17]'s already, so obviously
that'd make the most sense; which, unfortunately, is not possible, because
Runnable instances are not serializable.

Enter the serializable closure problem.

As one might suspect, the reason that a Runnable isn't serializable is because
it is a hard problem. The same problem exists in the functional programming
world where closures are a prevalent abstraction. Why both of these problems
are hard is directly related. The core issue is that both abstractions are
meant to represent _computation_, not data. Computation has a representation
in a machine (virtual or otherwise), but we aren't normally privy to it.

One solution, however, comes directly from researchers figuring out how to
compile functional languages.

Enter [closure conversion][18].

What is a [closure][19]? Well, a closure is really a 2-tuple, (_code_,
_data_). We know what the _code_ is, but what about the _data_? The _data_ is
a representation of bindings to all the free variables in the _code_.

For example,  creates a new closure:


    function make_counter(x) {

      return function(incby) {

        x += incby;

        return x;

      }

    }


(Here, x is a _[free variable][20]_ with respect to the inner function because
the inner function did not introduce it. However, incby is _bound_ with
respect to the inner function, since it was a parameter of the function)

Closure conversion might, for instance, translate the above function to be
this instead:


    function make_counter_closure_1(closure, incby) {

      closure.bindings[0] += incby;

      return closure.bindings[0];

    }

    function make_counter(x) {

      return new Closure("make_counter_closure_1", [x]);

    }


And any time a value returned by make_counter is called, the appropriate
transformation occurs to make that happen.

Could we serialize the returned value of make_counter? Absolutely! We just
need a way to refer to the function "make_counter_closure_1" by name (or
address)--easy in a dynamic language, solvable in a compiled language
too![[5]][21]

This translates quite nicely to Java (and our situation), with a few tweaks as
well:


    public class DelayedJob implements Serializable {

      // i.e. "com.meetup.feeds.commands.PostItemCommand"

      private String _command;

      // the serialized data the PostItemCommand required to run

      private Context _context;

      public DelayedJob(String className, Context context) {

        _command = className;

        _context = context;

      }


      /* here's the "magic" method, _with error handling removed for

         brevity's sake_ */

      public Command bind() throws Exception {

        Class c = Class.forName(_command);

        Command instance = (Command)

        c.newInstance();

        instance.setContext(_context);

        return instance;

      }

    }



And an example command:


    public class PrintCommand extends Command {

      public PrintCommand() { super(); }

      public void run() {

        Context ctx = getContext();

        String message = ctx.getString("message");

        System.out.println("The Message Is: '" + message + "'");

      }

    }


Where Command is:


    public abstract class Command extends Runnable {

      private Context _context = null;

      public Command() {}

      public void setContext(Context c) {

        _context = c;

      }

      public Context getContext() {

        return _context;

      }

      abstract void run();

    }


It might be obvious from the code above, but this won't work with anonymous
Command instances, because we're serializing a fully qualified class name as
part of the DelayedJob.

And how we use it:


    public class FeedClient {

      public static void print(String message) {

        Context c = new Context();

        c.set("message", message);

        AsyncFeedService().get.asyncQueueJob(

          new DelayedJob(PrintCommand.class.getName(), c);

        }

    }


As can be imagined, the process that executes these jobs deserializes the
DelayedJob that was sent, calls bind(), and submits it to an Executor to be
run.

The one pseudo issue with this solution is that code changes can require a
redeploy and restart of the job executing machines. This seems like a
reasonable tradeoff.

How does this pan out in practice? It works rather well. We've kept the
ability to execute jobs the "old way" as well, and switch back to that for
short periods of time in some maintenance situations, but this runs and stays
up without issues. It also has worked well enough that we're going to try
using it for replicating writes across data centers, until HBase's [multi data
center replication][22] aware capabilities mature a bit.

  1. We love shiny stuff as much as the next guy, but we hate downtime, and
hate putting out fires. [↵][23]

  2. We're using Java's [Executor][16] framework. [↵][24]

  3. … and since our data center is on the east coast and the data center is
made out of asbestos (this paragraph and footnote are me _trying_ to be
funny)… [↵][25]

  4. Crap! Distributed systems are hard! [↵][26]

  5. In fact, the reason I'm writing this post is that I just read [this
paper][27] yesterday, in which almost the exact same thing is done in Haskell!
[↵][28]

   [1]: http://making.meetup.com/post/4639337121/serializing-work-to-be-done

   [2]: http://www.meetup.com

   [3]: http://hbase.apache.org

   [4]: http://couchdb.apache.org/

   [5]: http://www.basho.com/products_riak_overview.php

   [6]: http://www.mongodb.org

   [7]: http://cassandra.apache.org/

   [8]: #note-slimoptions

   [9]: https://github.com/ghelmling/meetup.beeno

   [10]: http://www.meetup.com/ny-tech/

   [11]: #note-executors

   [12]: http://download.oracle.com/javase/6/docs/api/java/util/concurrent/Rej
ectedExecutionException.html

   [13]: http://www.rabbitmq.com

   [14]: #note-asbestos

   [15]: #note-disthard

   [16]: http://download.oracle.com/javase/6/docs/api/java/util/concurrent/Exe
cutor.html

   [17]: http://download.oracle.com/javase/6/docs/api/java/lang/Runnable.html

   [18]: http://en.wikipedia.org/wiki/Lambda_lifting

   [19]: http://en.wikipedia.org/wiki/Closure_(computer_science)

   [20]: http://en.wikipedia.org/wiki/Free_variable

   [21]: #note-cloudskell

   [22]: https://issues.apache.org/jira/browse/HBASE-1295

   [23]: #return-slimoptions

   [24]: #return-executors

   [25]: #return-asbestos

   [26]: #return-disthard

   [27]: http://research.microsoft.com/en-
us/um/people/simonpj/papers/parallel/remote.pdf

   [28]: #return-cloudskell

