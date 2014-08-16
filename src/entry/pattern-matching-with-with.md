% Pattern Matching with "With"
% python, with, hack, ocaml
% 2010-04-28


When I originally thought about adding pattern matching to Python, in the
[OCaml sense][1], I ended up using a decorator that more or less registed a
bunch of callbacks with a dispatch table based on the types of it's arguments.

That worked out fine, but it didn't really have the feel of [pattern
matching][2] like you get with real [algebraic data types][3]. If you recall,
I was playing with the following example with the decorator approach:


    type astnode =

    | AndNode of astnode * astnode

    | OrNode of astnode * astnode

    | NotNode of astnode

    | IdNode of bool


    let rec eval_node (n: astnode) =

      match n with

      | AndNode (l, r) -> (eval_node l) && (eval_node r)

      | OrNode (l, r) -> (eval_node l) || (eval_node r)

      | NotNode l -> not (eval_node l)

      | IdNode v -> v


    eval_node (AndNode (IdNode true, IdNode false)) (* returns false *)


The idea of that program was to create a small language to evaluate boolean
expressions. In OCaml, it's quite succinct--too succinct, in all honesty.
That's it. Of course it doesn't include a parser, or a lexer, but that's the
crux of it.

Since that original post, I've posted about two other language hacks that I've
attempted to create--both of which use [context managers][4] and the
`with`-statement, [worlds][5] and [dispatching urls (a la routes)][6].

Basically, it occurred to me yesterday, that `with`'s `as` clause did
destructuring of tuples, in the same way that the assignment statement does.
That is to say:


    a, b, c = 1, 2, 3

Will correctly assign `a = 1`, `b = 2`, `c = 3`, in the same exact way that:


    from contextlib import contextmanager

    @contextmanager

    def assign(*args):

        yield args


    with assign(1, 2, 3) as (a, b, c,):

        pass


will assign `a = 1`, `b = 2`, `c = 3`.

I'll admit, that doesn't look very powerful by itself, but when you consider
the possibilities, you might come up with something like I did:


    with structural_matching((1, 2, 3)) as match:

        with match('list() x y z') as (x, y, z):

            print x, y, z

        with match('tuple() x _ z') as (x, z):

            print "tuple case"

            print x, z


which looks incredibly close to pattern matching in OCaml. I was super excited
--but it won't work.

See, `match` is a context manager that gets returned with the intention that
if the `__enter__()` method raises a `NoMatch` exception, it skips the "body"
and goes to the next match. The problem with that thinking however is simple--
there's no way for `__enter__` to force skipping the body due to rejected
[PEP-377][7]!

In the example above (full source [here][8]), raising `NoMatch` in the first
`match` block, results in control being passed back to the `__exit__()` of the
outer context manager--`structural_matching`. And to think, I got my hopes up!

But nevertheless, I pressed on, and hacked [together][9], a `match`, that can
destructure the following examples correctly:


    with match('[1:3]', [1, 2, 3, 4]) as (a,):

        print a

    # [2, 3]


    with match('[1:]', "hello world") as (a,):

        print a

    # ('e', 'ello world')


    with match('str() x y', 'hello world') as (h, e):

        print 'h = ', h, ',',

        print 'e = ', e

    # h = h, e = e


    with match('x y z', [1, 2, 3]) as (x, y, z,):

        print z, y, x

    # 3 2 1


    class obj(object):

        def __init__(self, x, y):

            self.x = x

            self.y = y


    with match('obj() .x .y', obj('x-ity', 'y-ity')) as (x, y):

        print 'x = ', x, ',',

        print 'y = ', y

    # x = x-ity, y = y-ity


    with match('x y _', [1, 2, 3]) as (x, y):

        print x, y

    # 1, 2


It's much less useful considering you can't put it in the `structural_match`
block ,like you would in a _real_ `match` statement, but it's all we've got.

Back then, I concluded with This is as close to OCaml like pattern matching
that we're going to get, at least as far as I know how to get, but it's sort
of cool, and definitely a hack. Today, I'll conclude the same way.

   [1]: http://sigusr2.net/2008/Sep/30/python-type-constructors-like-
ocaml.html

   [2]: http://en.wikipedia.org/wiki/Pattern_matching

   [3]: http://en.wikipedia.org/wiki/Algebraic_data_type

   [4]: http://www.python.org/dev/peps/pep-0343/

   [5]: http://sigusr2.net/2009/Oct/01/python-worlds.html

   [6]: http://sigusr2.net/2009/Mar/04/dispatching-with-with.html

   [7]: http://www.python.org/dev/peps/pep-0377/

   [8]: http://files.sigusr2.net/match1.py

   [9]: http://files.sigusr2.net/match2.py

