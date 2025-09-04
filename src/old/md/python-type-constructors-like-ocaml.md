% Python Type Constructors a la OCaml
% python, types, ocaml
% 2008-09-30

Earlier this summer, I started taking a look at the [OCaml][1] programming
language in anticipation of the compilers class I'm taking this fall.

OCaml is a strongly, [statically typed][2], functional language with [type
inference][3]. It's pretty neat, and supports imperative programming as well
as object-oriented constructs for when you need them. As a language, I've
found few things wrong with it, except for the edit-compile-run cycle (I
prefer the edit-run cycle you get in a language like Scheme, or even Python),
but OCaml does actually have a toplevel (the toplevel isn't crippled either,
but it doesn't seem realistic to not compile for testing, at least I haven't
yet found it to be).

Anyway, among all the amazing features of OCaml, the two that stand out as
being the most useful are pattern matching (on types) and the simplicity of
defining new types and creating constructors:


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


Of course, the type annotation, `(n: astnode)`, is optional due to the type
inference engine, but the above example shows a simple example type that might
be used when defining a language to do boolean arithmetic. It's quick to see
that the combination of type constructors (like the `IdNode` above) and
pattern matching simplify this incredibly.

But, we don't get this sort of coolness in Python. No, in Python, we can make
classes, but we do not get type checking of any kind, unless of course we do
it at runtime manually. That's because Python is [dynamically typed][4].

On the other hand, if we _really_ want to, we can use the built in functions
`isinstance` and `type` to raise errors when incompatible types are passed
along to functions. This is slow, but it works. And, in some cases, like the
one I'm about to show, maybe it doesn't matter, if you get certain benefits.

Consider a language, like before, that does boolean arithmetic. It'll support
boolean AND (&), OR (|), XOR (^) and NOT (!). A valid expression in this
language is of the form `t & !(f | t)` which evaluates to `false`.

I've created two different parsers for this language. In [bptuples][5], I
create an [AST][6] using Python tuples of the form `('AndNode', t, f)`. In
[bpnodes][7], however, I take a more elegant approach when building the AST--I
create "OCaml like" type constructors.



    ASTNode = deftype('ASTNode', ())

    AndNode = deftype('AndNode', (ASTNode, ASTNode,), ASTNode)

    OrNode = deftype('OrNode', (ASTNode, ASTNode,), ASTNode)

    XorNode = deftype('XorNode', (ASTNode, ASTNode,), ASTNode)

    NotNode = deftype('NotNode', (ASTNode,), ASTNode)

    IdNode = deftype('IdNode', (bool,), ASTNode)


This defines all the Python classes needed at runtime. `deftype` takes up to 3
arguments. The first argument is the name of the class, the second is a tuple
of classes which it can accept as arguments, and the optional third argument
is a parent class to inherit from. The optional third argument is only
necessary if you need multiple constructors for the same type, as in the case
of our OCaml example above, and this example here.

The code for `deftype` is actually pretty simple:



    def deftype(name, types, extends=None):

        class _dtype(object):

            def __init__(self, *args):

                for i, a in enumerate(zip(args, self.types)):

                    if isinstance(a[0], a[1]):

                        self.__setattr__('op%d' % i, a[0])

                    else:

                        raise TypeError("%s expected argument of type " % \

                                            (self.__class__.__name__,

                                             str(a[1])))

        if extends:

            parents = (_dtype, extends,)

        else:

            parents = (_dtype,)

        return type(name, parents, {'types': types})


Basically, what it does is use the Python built-in `type` to construct a class
at runtime with an attribute called `types` which holds the class names of the
expected arguments to the constructor of the type. On construction, the
arguments are enumerated and attributes are created of the form `op0`...`opN`.

It's a mouthful, for sure, but all that means is that when we construct a new
`AndNode`, we are sure that the arguments the instance was created with are
indeed `ASTNode`s, and those nodes are accessible at `op0` and `op1`.

At a cursory glance, you might be thinking, "but what does this actually do
for us?" Well, honestly, not much, but you do get to abstract out some more
runtime type checking and do dynamic dispatch with the help of some Python
decorators. Observe:



    @multimethod(AndNode)

    def interpret(ast):

        return interpret(ast.op0) and interpret(ast.op1)


    @multimethod(XorNode)

    def interpret(ast):

        left = interpret(ast.op0)

        right = interpret(ast.op1)

        return (left or right) and not (left and right)


    @multimethod(OrNode)

    def interpret(ast):

        return interpret(ast.op0) or interpret(ast.op1)


    @multimethod(NotNode)

    def interpret(ast):

        return not interpret(ast.op0)


    @multimethod(IdNode)

    def interpret(ast):

        return ast.op0


That's the entire interpreter to walk the AST and evaluate boolean
expressions, and is called via `interpret(parseBool(tokenizer('t & ! (t |
f)')))`.

In the tuple version, it's actually shorter when you use `if` statements, but
this approach is much more readable in my opinion. The [multimethod
decorator][8] runs the correct `interpret` based on the types of arguments.

This is as close to OCaml like pattern matching that we're going to get, at
least as far as I know how to get, but it's sort of cool, and definitely a
hack.

   [1]: http://caml.inria.fr/ (Objective-Caml)

   [2]: http://en.wikipedia.org/wiki/Type_system#Static_typing

   [3]: http://en.wikipedia.org/wiki/Type_inference

   [4]: http://en.wikipedia.org/wiki/Type_system#Dynamic_typing

   [5]: http://hg.apgwoz.com/boolinterp/file/0c592ee06c7f/bptuples.py

   [6]: http://en.wikipedia.org/wiki/Abstract_syntax_tree (Abstract Syntax
Tree)

   [7]: http://hg.apgwoz.com/boolinterp/file/0c592ee06c7f/bpnodes.py

   [8]: http://www.artima.com/weblogs/viewpost.jsp?thread=101605

  *[XOR]: One or the other true, but not both

  *[OR]: One or the other true

  *[AST]: Abstract Syntax Tree

