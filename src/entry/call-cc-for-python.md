% Code Blogging: Call With Current Continuation for Python
% python, continuations, greenlet, callcc
% 2011-08-09


[Continuations][1] are a snapshot of the programs control state at a given
time. The concept exists in every language, but only some allow a programmer
to bundle it up into an actual object to manipulate in some way.

That bundled, first-class, version of a continuation is applyable, like a
function, and depending on the language is either invokable once (one-shot) or
multiple times (multi-shot). In the case of multi-shot continuations, one can
imagine a great number of use cases such as building coroutines, efficient
backtracking and stateful web-servers.

For one-shot continuations, the number of things that can be done with them is
more limited, but they are still extremely useful. These are often described
as "escape-continuations" and are often compared to something like `setjmp` /
`longjmp`--they can "escape" back to a save point, but can never jump forward.
As such, often the example given for uses of them is exception
handling.**[[1]][2]** For instance, one couldn't implement nondeterminism as
in Prolog because the continuation needs to be invoked multiple times to
collect multiple values[[2]][3]

Anyway, most languages do not have either type of first class continuations,
and more often than not, already have the common use cases of them as part of
the language.

For example, Python has exception handling built in, as well as the `yield`
statement which supports a limited coroutine mechanism. For actual coroutines,
an extension called [greenlet][4] can be used, which implements symmetric
coroutines[[3]][5].

What do coroutines have to do with continuations, aside from the fact that
multi-shot continuations can be used to implement coroutines?

Well, it turns out[[4]][6] that symmetric coroutines are all that is needed to
support one-shot continuations, and with greenlet, this can be done in Python:


    import greenlet


    class ContinuationError(Exception): pass


    def callcc(f):

        saved = [greenlet.getcurrent()]


        def cont(val):

            if saved[0] == None:

                raise ContinuationError("one shot continuation called twice")

            else:

                return saved[0].switch(val)


        def new_cr():

            v = f(cont)

            return cont(v)


        value_cr = greenlet.greenlet(new_cr)

        value = value_cr.switch()

        saved[0] = None

        return value


Its use is illustrated in the following simple example which adds 3 numbers,
but only when the first one is not 5. In that case, 0 is returned from the
computation:


    def add3(x, y, z):

        def add_x_but_not_when_5(cont):

            if x == 5:

                cont(0)

            else:

                cont(x + y + z)

        return callcc(add_x_but_not_when_5)


Using it:


    >>> add3(5, 6, 7)

    0

    >>> add3(6, 7, 8)

    21

    >>>


  1. My haste in writing caused me to write something that wasn't correct.
One-shot continuations and escape continuations are not the same thing. Thanks
to the commenter below who pointed out my error.

  2. Carl Bruggeman, Oscar Waddell, and R. Kent Dybvig; "Representing control
in the presence of one-shot continuations"; In Proceedings of the SIGPLAN '96
Conference on Programming Language Design and Implementation, 99-107, May
1996.

  3. Symmetric coroutines have a single control transfer which allows them
individually to switch amongst themselves. In the Greenlet library, this is
the `switch` method. Asymmetric coroutines, which is the other type, have two
control operations, `yield` and `resume`, which makes them subordinates to
their caller.

  4. Ana Lucia de Moura and Roberto Ierusalimschy; "Revisiting Coroutines";
2004

   [1]: http://en.wikipedia.org/wiki/Continuation

   [2]: #note-haste-in-writing

   [3]: #note-one-shot

   [4]: http://pypi.python.org/pypi/greenlet

   [5]: #note-symmetric-coroutines

   [6]: #note-revisiting-coroutines

