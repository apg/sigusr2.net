% Python Worlds
% worlds, python
% 2009-10-01


Last year, I was introduced to a paper via [Lambda the Ultimate][1] about
worlds, a language construct which allows one to control the scope of side
effects while programming.

Worlds allow you to capture the current scope of a program in a first-class
way. All updates to the current state (i.e. local variables, global variables)
happen in a non-commiting way. In other words, you can back out of any changes
at any time.

Consider the following example (taken from the [Warth paper][2]):


    A = thisWorld; // thisWorld is always the current world

    r = new Rectangle(4, 6);


    B = A.sprout(); // sprout creates a new world with it's parent set to A

    in B { r.h = 3; } // side effects within this `in' occur only in the world
B.


    C = A.sprout();

    in C { r.h = 7 }; // in A's world r.h = 6 still.


    C.commit(); // only now does r.h = 7 in world A.


If you follow along in the comments I've appended to the example, you'll start
to see why this idea is interesting, even from this little example.

The astute Scheme programmer, however, will notice almost certainly that this
construct could be created with `call/cc`, which is certainly true. The
problem with this fact is that not all programming languages are Scheme
(unfortunately), and of course not all languages support first-class
[continuations][3].

The question I asked myself, however, is this: Can I hack worlds into Python?
To which I came up with the short answer after some thinking, sort of.

I guess I should explain what's going on more clearly in the example above.
The first thing to note is that `A` represents the current scope; the current
state of all variables in the program. Sprouting a new world from an existing
world means that any changes that occur when using the sprouted world, do not
affect the world who sprouted the current world, unless the new world
_commits_ the changes made to the original world.

Which is to say, changes that occur in an `in` block acting on world `X` do
not propagate to the parent (the world `X` was sprouted from) of `X` unless
`X.commit()` is called.

### Enter Context Managers

A few months ago, I wrote a blog post about _[Dispatching With "with"][4]_, in
which I explained [context managers][5] in Python, and how they can be
exploited to create a less separated mapping from URLs to request handlers
(something that definitely has its place in the small web-app world).

The basic idea of this was that in the `__exit__` method of the web-
application object, the current frame was inspected and references to
functions that represent HTTP methods would be collected, stored and tied to
the last regular expression passed to the `expose` method in the application
object. This simple solution allowed us to express a web application
succinctly like so:


    app = web.application()

    with app.expose('/'):

       def get():

           return "Hello World"

    app.run()


For worlds, I also exploit context managers, though mostly for the `in`-like
syntax, and for managing the current `thisWorld` variable.

The quick[[1]][6] solution that I came up with for [implementing worlds][7]
can be used like so:


    with Universe(): # establishes new world, assigns to local variable
`thisWorld'

       thisWorld.r = True # must assign _directly_ in the world. LIMITATION

       new = thisWorld.sprout()


       with new:

           new.r = False


       with new.sprout():

           thisWorld.r = 15

           thisWorld.commit() # now new.r is 15, but the original r is still
True


       print thisWorld.r # => True

       new.commit()

       print thisWorld.r # => 15

       thisWorld.commit() # have to commit to the actual scope LIMITATION

       # r is now part of the local variables where this universe exists

       print r # => 15


Looking at this example, it's already apparent that the Warth implementation
of worlds is superior, just in the amount of code needed to take advantage of
it. You might also see that I didn't even attempt to port the rectangle
example from above. That is because there isn't anything smart going on under
the hood when it comes to container objects (such as lists, tuples, objects,
dicts), and I'm not yet sure how to get there.

With simple immutable objects such as booleans, integers and strings, using
copy-on-write semantics works wonderfully. Then, on `commit` of the world, the
code just copies all of the changes into its parent. I haven't tackled the
case of mutable container objects just yet, as there are complications in the
API[[2]][8], as well as the implementation.

The interaction with this is sort of annoying though. In order to take
advantage of worlds in Python, you have to touch virtually every line of code
in the function you are trying to worldize, because you must assign explicitly
to a world. The world's context manager sets up `thisWorld` for you, but you
still have to do `thisWorld._variable_` to get any sort of benefit.

My inclination is to get into some bytecode hacking to modify all assignments
within the `with` block to be assignments to `thisWorld` automatically, but
bytecode hacks are neither pleasant to maintain, nor are they portable across
implementations.

It's also possible in the Warth version to worldize functions and any other
first class object. Maybe the solution is simple and I just haven't seen it
yet. Whatever hacks, that I come up with though, will be just that, hacks, as
there is no _easy_ way to add worlds to Python in the same way that Warth
added them to JavaScript[[3]][9].

We are in an age of programming where mainstream programming languages are
unable to adapt to our needs as programmers. We are unable to bend them at our
will like we can with [Scheme][10], [Lisp][11] and even [Clojure][12].
Attempts to bring about change on this front have not been met with enthusiasm
from most groups. Whether it's a lack of marketing, evangelism or just that
the general population doesn't view unbendability as a problem, I'm not sure.
But, I for one like the idea of being able to _easily_ add worlds, and other
ideas, as _true_ language features to languages that by practicality, I'm
forced into using. That would make me a much happier, and effective
programmer.

  1. By quick, I do mean quick. This was 2 hours of work and sketching. Surely
there is lots of work to be done to make it a true solution.

  2. The same strategy could be used as for simple values like booleans, if
the API used a method, say `assign` instead of the more natural assignment
operator. Consider, `thisWorld.assign('obj.height.inches', 30)` vs.
`thisWorld.obj.height.inches = 30`.

  3. The Worlds prototype was written in [OMeta][13], which is a solution to
the "unbendable" languages problem. Note: I didn't attempt to write worlds in
PyMeta, but, it may be possible to do.

   [1]: http://lambda-the-ultimate.org/node/3040

   [2]: http://www.vpri.org/pdf/rn2008001_worlds.pdf

   [3]: http://en.wikipedia.org/wiki/Continuation

   [4]: http://sigusr2.net/2009/Mar/04/dispatching-with-with.html

   [5]: http://www.python.org/dev/peps/pep-0343/

   [6]: #f1

   [7]: http://github.com/apgwoz/python-worlds

   [8]: #f2

   [9]: #f3

   [10]: http://en.wikipedia.org/wiki/Scheme_%28programming_language%29

   [11]: http://common-lisp.net/

   [12]: http://clojure.org/

   [13]: http://tinlizzie.org/ometa/

