% Softserv Serves Requests
% servers, clojure
% 2011-01-07



Anyone that knows me knows that I have a lot of projects, most of which are in
some sort of unfinished but partially functional state. Then there are others
that have a solid 0.1 release, maybe lacking documentation, but usable. There
are yet others that never make it to 0.1, and despite my happy thoughts about
how this piece of code will change the world, or be the future, it's as good
as deadpooled.

One project that will certainly not be deadpooled is being developed in my 10%
time at work, though on hiatus whilst I resolve some other issues that I
inadvertently committed and finish up some other tasks that have higher
priority.

But, I don't wanna discuss that project right now, it is in a sort of
discovery phase where I'm attempting to iron out the network protocol, and
what functionality it actually provides for its initial run[[1]][1].

It does, however, do what many servers do--listen to and accept requests via a
socket.

Clojure comes with many ways, of course, to do just that. It is certainly
possible to use one of the many frameworks for Java for writing servers, but
clojure.contrib/server-socket, looked to be exactly what I needed, and I
decided to have a look.

When evaluating any sort of library, the first thing I always do is create
something simple that makes use of it. This is not uncommon of course, and the
"Hello World" of network programming is the trusted echo server, in which,
everything you say will be parroted back to you. That's pretty simple using
server-socket:


    (ns echo

      (:require [clojure.contrib.server-socket :as ss]

                [clojure.contrib.duck-streams :as ds]))


    (defn echo-server [in out]

      (binding [*in* (ds/reader in)] [[2]][2]

         (ds/with-out-writer out

            (println (read-line)))))


    (ss/create-server 1025 echo-server)


    (comment

    $ echo "HELLO WORLD" | nc 127.0.0.1 1025

    HELLO WORLD

    $

    )

Simple enough, but it is unfortunately _too_ simple. For one, it creates a
thread per connection, which is just asking for trouble with a public server.
Secondly, since it only gives you an `InputStream` (`in`) and an
`OutputStream` (`out`), there isn't a way to log who connected, or anything
about the user who connected other than time. Hell, there's nothing
guaranteeing that you're even connecting via a network for this!

Now, I understand that `server-socket` was not really meant for production use
--it is meant for quick toying around, and testing out ideas as quickly as
possible, something it does extremely well, but with a little bit of
modification, I think it can be almost just as simple and still be suitable
for production use.

Enter [softserv][3], a still simple, but slightly more complicated replacement
for server-socket.

So what is more complicated about it? Let's look at "Hello World" and find
out:


    (ns echo

      (:use [softserv.core :only (create-server

                                  defservice

                                  defhandler

                                  with-shutdown)])

      (:require [clojure.contrib.duck-streams :as ds]))


    (defn echo-parser [s]

      (binding [*in* (ds/reader s)]

        {:data (read-line) :type :echo}))


    (defservice echo-server :type echo-parser)


    (defhandler echo-server :echo

      [s req]

      (with-shutdown s

        (ds/with-out-writer s

          (println (:data req)))))


    (create-server 1025 echo-server 10)


    (comment

    $ echo "HELLO WORLD" | nc 127.0.0.1 1025

    HELLO WORLD

    )

Phew! That was a mess of code to write to get a simple echo server, surely
softserv isn't so, well, soft? Just look at what we've gained, though!

First and foremost, we've gained the ability to specify a maximum number of
threads to create. That's what the `10` is for in `create-server`. Softserv
executes request handlers in a thread pool. This of course has some
limitations over the thread-per-connection model (like making it harder to
allow for long running session based servers), but for resource constrained
single request services, it's probably a plus. Maybe relaxing this requirement
and figuring out an appropriate way to handle long running connections is a
good next step.

Secondly, and without any hit in the number of symbols or parentheses, we're
passing an actual socket object to the function that does the handling. You'd
think that we'd have to call `(.getInputStream s)`, but `duck-streams` knows
how to get an `InputStream` for a `Socket`. This is almost identical to the
original example.

Third, and most importantly, we've made it possible to dispatch based on the
type of request. Softserv enforces what you'd have done anyway. The handler
function that `server-socket/create-server` takes in almost all cases will end
up being a `cond` statement dispatching to other functions. Softserv just
makes that explicit and up front.

Enough chatter, how can I make use of it? Well, our echo server would be much
more interesting if in fact we echoed back more interesting things. For
instance, we could translate the input, or rot-13 the input, or for ease of
illustrative purposes echo back the actual date, a more useful echo, when
someone sends the string "DATE".


    (ns echo

       (:use [softserv.core :only (create-server

                                   defservice

                                   defhandler

                                   with-shutdown)])

    -  (:require [clojure.contrib.duck-streams :as ds]))

    +  (:require [clojure.contrib.duck-streams :as ds])

    +  (:import [java.util Date]))


    (defn echo-parser [s]

      (binding [*in* (ds/reader s)]

    -    {:data (read-line) :type :echo}))

    +    (let [l (read-line)]

    +      (assoc {:data l} :type (if (= l "DATE") :date :echo)))))


    (defservice echo-server :type echo-parser)


    +(defhandler echo-server :date

    +  [s req]

    +  (with-shutdown s

    +    (ds/with-out-writer s

    +      (println (str (Date.))))))


    (create-server 1025 echo-server 10)


    (comment

    $ echo "HELLO WORLD" | nc 127.0.0.1 1025

    HELLO WORLD

    $ echo "DATE" | nc 127.0.0.1 1025

    Wed Jan 05 08:29:38 EST 2011

    )


Using a single request parsing function of course has its limitations, but in
general, network services are not the type of thing that really have diverse
request types. The protocol normally allows you to tell within a few bytes
what kind of request it is. In the case of softserv servers the handlers can
then finish reading the request and taking the appropriate actions.

I'd really appreciate any feedback on this.

  1. There's also the question of non-blocking vs. blocking, but the first
pass will be blocking until there are real numbers to look at.

  2. I'm not sure of the reasoning, but `(with-in-reader s ...)` throws an
exception saying java.io.PushbackReader cannot be cast to
java.io.BufferedReader. This doesn't seem right to me, since it'd work if it
were a LineNumberingPushbackReader, but PushbackReader's don't have a
`.readLine` method, and don't derive from BufferedReader. Nevertheless, I feel
as though read-line should know how to handle it, or wrap the PushbackReader
in a LineNumberingPushbackReader temporarily, if that's possible.

   [1]: #note-nonblocking

   [2]: #note-with-in-reader

   [3]: http://github.com/apgwoz/softserv

