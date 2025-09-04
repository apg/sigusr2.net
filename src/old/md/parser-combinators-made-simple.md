% Parser Combinators Made Simple
% python, parsing, combinators, functional, #pinned
% 2011-04-18


Parsing theory has been around for quite a long time, but it is often thought
of as magic by the swarms of people who haven't bothered to read about it, and
see how plain and dry it actually is. Algorithms for parsing [LR(k)][1]
grammars (meaning Left-to-right, Right-most derivation, k tokens lookahead)
for instance, normally just traverse a state machine that was computed before
hand (either by hand, or by using a parser generator such as [bison][2] or
[yacc][3]). Sure, there are many things to trip on, tedious to track down
ambiguities, and other issues, but the general theory of parsing has remained
unchanged for years--one might say, it is a solved problem.[^1]

When learning about parsing for the first time though, the idea of a
[recursive descent parser][5] is often taught first. Recursive descent
parsers, are relatively simple to reason about, to write and to shoot yourself
in the foot with. A simple [LL(1)][6] parser (meaning Left-to-right, Left-most
derivation, 1 token lookahead), for instance, can't parse [left-recursive
grammars][7], which is the most natural way to write certain types of
grammars[^2]. Typically, when writing a recursive descent parser, the
author takes the grammar and produces a function for each production (non-
terminal). Each function then reads a token and recurses to the other non-
terminals in the grammar reachable from the current production. And,
eventually, at the end of the function the sub parts will be combined in such
a way that a [parse tree][9] will be created.

This sounds boring and tedious, and in fact is. However, there is a useful
technique for creating these types of parsers that was developed some time
ago[^3], which involves composing a small set of functions into more
meaningful, more advanced parsers. They still suffer from the same problems as
your typical recursive descent parser (as presented), but with some other
trickery can be made to overcome those deficiencies (we won't discuss that in
this article).

In order to build a parser from the ground up, we need to think about what a
parser actually is. In some sense, it is really just a function that takes an
input string and produces some result. That result, in order to make any
progress should contain the leftover string after consuming some part of it,
or in the case of error (i.e. incorrect input), return some value indicating
failure. In Python, a natural way to encode both results would be to use
`(_"matched string"_, _"leftover string"_)` or `None`. For sanity's sake, let
us refer to functions which match this criteria as _parser functions_.

To start off, we'll write a useful parser function, which at first glance
seems pointless, `anychar`. `anychar` matches _any_ (no trickery here!)
character so long as there is at least one character left in the input string.
(**Note:** we'll use the variable `strn` to always refer to the input string,
which represents the _string left to parse_.)


    def anychar(strn):
        if strn == "":
            return None
        return (strn[0], strn[1:])


It is easy to see that the result of this parser function matches our
encoding. If there are no characters left in `strn`, then we return the
failure condition, `None`, otherwise we return a tuple of what we parsed, and
the rest of the string which we didn't parse.

It becomes more useful when we pair `anychar` with a test against the
character it consumes. Enter `chartest`, which is a function that creates
another parser function, given a predicate (i.e. a function which returns
`True` or `False`).


    def chartest(pred):
        def _(strn):
            c = anychar(strn)
            if c and pred(c[0]):
                return (c[0], c[1])
            return None
        return _


In order to use `chartest`, we pass it a predicate, like so:


    >>> chartest(lambda x: x == 'a')('abc')
    ('a', 'bc')


To see what happened, remember that `chartest` _creates_ a new parser
function. With that, we just call the new parser function with the rest of the
input string `'abc'`. The result indicates success, because an `'a'` was
discovered as the first character. If we were unsuccessful, just like in
`anychar`, instead of `('a', 'bc')`, we'd have seen `None` returned.

It is a bit verbose to always create a `lambda` to match a single character,
so `matchchr` gets a target character and calls `chartest` for us. (Remember,
calling `chartest` creates a _new_ parser function, this is an important thing
to note.)


    def matchchr(targetchr):
        return chartest(lambda x: x == targetchr)


Now we can match single characters against our input stream, which is a great
starting point, but hardly makes for an easy to use library. One limitation is
that there is no way to specify more than one character as a possible match,
such as "all alpha numeric"--for that, we use `oneof`.


    def oneof(target):
        chars = set([x for x in target])
        return chartest(lambda x: x in chars)


`oneof` creates a new test function to pass to chartest, which instead of
testing if a character is equal to a single target character, checks to see if
the character is in the set of characters we're looking for. Some useful
definitions follow, which make parser functions using `oneof`, and a set of
characters.


    alpha = oneof('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ')
    loweralpha = oneof('abcdefghijklmnopqrstuvwxyz')
    upperalpha = oneof('ABCDEFGHIJKLMNOPQRSTUVWXYZ')
    digit = oneof('0123456789')
    hexdigit = oneof('0123456789abcdefABCDEF')
    whitespace = oneof(' \t\n\r')


While matching a single character is useful, it would be much more useful if
we could match a _token_, like "`while`," or "Content-Type." Not to worry,
`matchstr` produces a parser function that will combine the previously created
`matchchr` for each character in the target string. It looks a bit
complicated, so we'll go through it step by step.


    def matchstr(target):
        if not target:
            return lambda strn: ("", strn)

        def _(strn):
            c = matchchr(target[0])(strn)
            if c:
                cs = matchstr(target[1:])(c[1])
                if cs:
                    return (c[0] + cs[0], cs[1])
            return None
        return _


`target`, just like `targetchr` in `matchchr` is the string we're eventually
trying to match in full. If `target` is empty, then our parser function is
simple--it doesn't advance the input string, and doesn't consume anything.

Why don't we return `None` here? Well, if our target is empty, we're not
asking `matchstr` to do any work at all, so there isn't a failure (indicated
by `None`). It, however, also makes for a great base case to the recursion
that follows.

If there _is_ a target string to match against, we attempt to match the first
character within it. If that succeeds, we shorten the target string and
recurse. We eventually return a combination of the result of `matchchr` and
the result of the recursive call to `matchstr`. Take a minute to look over
this and ensure you understand it--it's actually pretty straightforward
assuming understanding of the previous functions.

Let's take a look at how we use it:


    >>> matchwhile = matchstr('while')
    >>> matchwhile('while True:')
    ('while', ' True:')


As you can see, we used `matchstr` to _build_ a parser function which matches
the string "`while`"--simple enough.

Ok, so what if we want to parse more complicated things, like say, the rest of
the input string from the "while True:" example? We need some ways to combine
these parser functions to make them more useful, otherwise, all we did was
create the equivalent of:


    if strn.startswith("while"):
        return (strn[0:5], strn[5:])


Which, in Python, would be much more efficient![^4]

Another parser function that we need to make this whole thing useful is
`optional`. `optional` takes as an argument a parser function, and returns a
new parser function that succeeds even if the original parser function does
not. Essentially, if there is a failure, it returns the original input string.


    def optional(parserfn):
        def _(strn):
            c = parserfn(strn)
            if c:
                return c
            return ('', strn)
        return _


If we make `matchwhile`, from above, optional we get this:


    >>> optional_matchwhile = optional(matchwhile)
    >>> optional_matchwhile('foo')
    ('', 'foo')


Without `optional`, attempting to call `matchwhile` on the input string
`'foo'` would have resulted in `None`, the failure condition.

The presence of `optional` also leads us to `repeat` and `repeat0` which are
mutually exclusive. `repeat` will attempt to match the parser function at
least once, with no boundary. `repeat0` will match the parser function zero or
more times:


    def repeat(parser):
        def _(strn):
            c = parser(strn)
            if c:
                cs = repeat0(parser)(c[1])
                return (c[0] + cs[0], cs[1])
            return None
        return _


    def repeat0(parser):
        return optional(repeat(parser))




Again, like `optional`, `repeat` and `repeat0` build parser functions from
existing ones. This is very much a common pattern when building parsers of
this type.

The implementation of `repeat0` and `repeat` is quite clever. Note that zero
or more is the same as _optionally_ one or more. The implementation of both
follows from that realization. `repeat` first attempts to call the passed in
parser function. If it succeeds it calls `repeat0` on the rest of the input
string after calling `parser` the first time. If `repeat0` succeeds, which it
always will given `optional`, we combine the results and return.


    >>> optrepeat_while = repeat0(matchwhile)
    >>> optrepeat_while('whilewhilewhile')
    ('whilewhilewhile', '')
    >>> optrepeat_while('foo')
    ('', 'foo')
    >>> repeat_while = repeat(matchwhile)
    >>> repeat_while('foo')
    None
    >>> repeat_while('while foo')
    ('while', ' foo')


We still need the ability to do alternation, like "while" _or_ "if." For that
we introduce `alt`.


    def alt(*parsers):
       def _(strn):
           for p in parsers:
               result = p(strn)
               if result:
                   return result
       return _


This is really simple. We take a list of parser functions and try them one by
one, from left to right, until we find one that passes.


    >>> iforwhileorfor = alt(matchstr('if'), matchstr('while'), matchstr('for'))
    >>> iforwhileorfor('if')
    ('if', '')
    >>> iforwhileorfor('while')
    ('while', '')
    >>> iforwhileorfor('for')
    ('for', '')
    >>> iforwhileorfor('foof')
    None


Alternation is important, but it is maybe even _more_ important to ensure that
many parser functions pass in order, a sequence of parser functions if you
will. It is this operator that allows us to do something like `whilestmt =
sequence(whileToken, conditional, colonToken, codeBlock)`.


    def sequence(*parsers):
        def _(strn):
            parsed = ''
            rest = strn
            for p in parsers:
                result = p(rest)
                if result:
                    rest = result[1]
                    parsed += result[0]
                else:
                    return None
            return (parsed, rest)
        return _


Assuming simplified definitions of the supporting rules, our `whileStmt`
example looks something like this:


    >>> whileToken = matchstr("while")
    >>> conditional = oneof("><=")
    >>> colonToken = matchchr(":")
    >>> codeBlock = alt(matchstr("if"), matchstr("for"))
    >>> whileStmt = sequence(whileToken, conditional, colonToken, codeBlock)
    >>> whileStmt('while<:if')
    ('while<:if', '')
    >>> whileStmt('while>:if')
    ('while>:if', '')
    >>> whileStmt('while>:for')
    ('while>:for', '')
    >>> whileStmt('while:for')
    None


`sequence` looks complicated, but is rather simple. It is basically `reduce`,
combining the results of each parser into the results of all of the parser
function outputs together.

That's all we really need to construct more interesting parsers, so we'll now
construct a simplified parser for JSON.[^5]

We'll start with some utility functions:


    def betweenchrs(parser, left="(", right=")"):
        def _(strn):
            lres = matchchr(left)(strn)
            if lres:
                pres = parser(lres[1])
                if pres:
                    rres = matchchr(right)(pres[1])
                    if rres:
                        return (left + pres[0] + right, rres[1])
            return None
        return _

    betweenparens = lambda p: betweenchrs(p, left="(", right=")")
    betweenbrackets = lambda p: betweenchrs(p, left="[", right="]")
    betweencurlies = lambda p: betweenchrs(p, left="{", right="}")


`betweenchrs` lets us easily create a parser function which attempts to parse,
using `parser`, only if it is between `left` and `right`. This is useful in
JSON, because of its list and dictionary data types, which are delimited by
`[]` and `{}` respectively.

Strings in JSON are composed of a series of characters between `"`'s. But, if
you want to actually use a `"` within the string, you can do that by preceding
it with a `\`. We can make a parser function that satisfies these rules rather
easily, making use of `anychar`.


    def charorquoted(strn):
        c = anychar(strn)
        if c[0] == '"':
            return None
        elif c[0] == '\\':
            c2 = chartest(lambda x: x in ('\\', '"'))(c[1])
            if c2:
                return (c[0] + c2[0], c2[1])
        else:
            return c


In the case that we find a '"' character without a '\' character preceding it,
it is a failure.

Whitespace doesn't much matter between tokens in JSON, so let us define
something that ultimately ignores it. `ignorews` uses `repeat0` to strip the
preceding whitespace, calls the parser function given using the left over
input string. If the parser function passes, it calls `repeat0` again against
whitespace and ultimately returns the passed in parser function's parsed
result and the ignored whitespace's left over input string. That's a mouthful,
but it's fairly easy to understand:


    def ignorews(p):
        def _(strn):
            w = repeat0(whitespace)(strn)
            if w:
                pres = p(w[1])
                if pres:
                    w2 = repeat0(whitespace)(pres[1])
                    if w2:
                        return (pres[0], w2[1])
            return None
        return _


`anint`, `astring`, `acolon` and `acomma` are just helper functions which do
exactly what they describe. We're simplifying this implementation, as it is
for demonstration purposes, so we're not taking into consideration floating
point numbers, or integers specified using hexidecimal and other formulations
of numbers.


    anint = sequence(optional(matchchr("-")), repeat(digit))
    astring = betweenchrs(repeat0(charorquoted), left='"', right='"')
    acolon = matchchr(':')
    acomma = matchchr(',')


When we define dictionaries and lists, we run into a problem. Both lists and
dictionaries can contain lists and dictionaries (as well as numbers and
strings of course), which represents a problem for when we define these
functions (we can't recursively define something that doesn't already exist!).
One solution to this problem is to use a mutable object which acts as a
"forward reference." When we finish defining the pieces we need, we update the
forward reference, and then all is well.

For both aesthetic, and practical reasons, we'll use a class instance which
overrides `__call__`, and `__ilshift__`, which will allow us to use an
instance of `Forward` as a parser function, and `<<=` (from __ilshift__) as a
way to update the parser function that's contained within the reference.


    class Forward(object):
        def __init__(self):
            self.p = None

        def __call__(self, *args, **kwargs):
            return self.p(*args, **kwargs)

        def __ilshift__(self, p):
            self.p = p
            return self


To use a `Forward`, we simply create an instance of `Forward` and assign it to
a variable, just as if we were creating a parser function. `Forward` is like a
promise. "If you act like a parser function for me for a little bit, I promise
I'll actually turn you into one later." If the promise is kept, and the
`Forward` is updated, parsing will proceed as if nothing was ever not
specified to begin with.


    avalue = Forward()
    akey = ignorews(alt(anint, astring))
    akeyvaluepair = sequence(akey, acolon, avalue)


Both lists and dictionaries have items that are separated by comma.
`commaseparated` is essentially `repeat0` except that it ensures a comma
appears after each item, except in the last item.


    def commaseparated(parser):
        def _(strn):
            r = repeat0(sequence(parser, acomma))(strn)
            if r:
                r2 = parser(r[1])
                if r2:
                    return (r[0] + r2[0], r2[1])
            elif r:
                return r
            return None
        return _


Now that we have all the pieces specified, we put a `commaseparated` key value
pair between curly braces to parse a dictionary, and a `commaseparated` value
parser function between square brackets to parse a list.


    adict = betweencurlies(commaseparated(akeyvaluepair))
    alist = betweenbrackets(commaseparated(avalue))


We still have our promise to keep for `avalue`, and with the definitions of
`alist` and `adict`, we now can. `avalue`, as in JSON, should either be a
number, a string, a list or a dictionary, and whitespace is ignored.


    avalue <<= alt(*map(ignorews, [adict, alist, anint, astring]))


To achieve alternation, we make use of `alt`, but before we do that, we wrap
each parser function contained in `avalue` in an `ignorews` parser function
builder to satisify that requirement. Finally, we shift the newly created
parser function into the forward reference for `avalue`.

To parse a top level JSON document, we look for either a list, or a
dictionary. The parser function to do that is quite easy to specify.


    json = alt(adict, alist)


Last, but certainly not least, let's actually use what we've constructed:


    >>> json('''{"hello": {1: "how are you?"}, "i is": "fine", "how": "are you?", 1: ["these", "values", "work", 2]}''')
    ('{"hello":{1:"how are you?"},"i is":"fine","how":"are you?",1:["these","values","work",2]}', '')


Success!

While we haven't shown how to formulate an LL(k) grammar, or even talked about
what that actually is formally, we have shown that with a few simple functions
that build parser functions, we can build, quite easily, parsing functions
which parse complicated things. However, we've only shown that the input is
valid, actually constructing a parse tree, or acting upon it, is left as an
exercise to the reader.

Source code for all this is [here][13]

[^1]: I'm not sure if parsing is really "solved," but the algorithms we have work well enough in practice that there isn't a ton of interesting new research going on around it. [Yacc is Dead](http://arxiv.org/abs/1010.5023), for instance used the results of [a paper](http://portal.acm.org/citation.cfm?id=321249) from 1964, but came [under fire](http://research.swtch.com/2010/12/yacc-is-not-dead.html). Can parsing be made trivially easy? Maybe, but it's likely that ambiguity will be somewhere--which would be the unsolved in parsing.

[^2]: See [left](#fnref:2) [recursion](http://en.wikipedia.org/wiki/Left_recursion). All joking aside, left recursion occurs in a grammar when a non-terminal rule is used recursively and appears on the left. For example: `expr = expr + tail`. 

[^3]: Graham Hutton. Proceedings of the 1989 Glasgow Workshop on Functional Programming (Fraserburgh, Scotland), Springer-Verlag Series of Workshops in Computing, Springer-Verlag, Berlin, 1990.

[^4]: The purpose of this article isn't to describe an efficient parsing technique for Python, but to rather demonstrate a useful technique that could be adapted and built upon, to build an efficient parsing framework for _any_ language that supports first class functions. There's a follow up article to this which shows just how much of this can actually be abstracted out in order to make it even more simple.

[^5]: We limit our parser to strings, integers, dictionary and lists. The complete specification appears at [http://json.org](http://json.org)

   [1]: http://en.wikipedia.org/wiki/LR_parser

   [2]: http://www.gnu.org/software/bison/

   [3]: http://en.wikipedia.org/wiki/Yacc

   [5]: http://en.wikipedia.org/wiki/Recursive_descent_parser

   [6]: http://en.wikipedia.org/wiki/LL_parser

   [7]: http://en.wikipedia.org/wiki/Left_recursion

   [8]: #note-leftrecursion

   [9]: http://en.wikipedia.org/wiki/Parse_tree

   [13]: http://files.sigusr2.net/parser_functions.py

   [14]: http://arxiv.org/abs/1010.5023

   [15]: http://portal.acm.org/citation.cfm?id=321249

   [16]: http://research.swtch.com/2010/12/yacc-is-not-dead.html

   [21]: http://json.org


