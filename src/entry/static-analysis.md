% Static Analysis
% pl, static analysis, dynamic languages
% 2014-01-15


Recently, I took part in a discussion about whether or not it was possible to
implement something like [Dialyzer][1] in Python. The answer is of course
[yes][2], and there are many examples of such tools[^1]. The discussion
soon turned into "why?"

Here's the thing. Using a static analyzer in a dynamic language isn't supposed
to be 100% accurate--that'd be pretty tough to guarantee, and if you _could_
guarantee it, it'd probably be pretty mediocre. These tools should only be
suggestive of potential problems, in much the same way that Java programmers
are sometimes forced to use `SuppressWarnings` to tell the type system,
"Bugger off, man! I know what I'm doing!"

The point being, static analysis in most languages is just a suggestion that
things are right. In C++, or in Java there are lots of ways to introduce
something that type checks but doesn't really give you any benefit of
"rightness." In Haskell and OCaml, the type systems are sound, so the
suggestions of "rightness" are much more trustworthy--in fact, they're no
longer suggestions, they're guarantees. If a Haskell or OCaml program type
checks, the program is guaranteed to never get into a state where data cannot
be combined because of mismatched types. That doesn't mean that you can't
still have errors, but some class of errors are completely erased as
possibilities, and they never have to be thought about.

Static analysis for a dynamic language is more similar to the C++ and Java
world. There's no "proof," but if you have a good analyzer, and the program
checks out, there's a bit less work that needs to be done to prove to yourself
that the program is "correct." The analysis tool might tell you about a bunch
of places where you're doing something funky--better verify those by hand, and
maybe write some test cases around that code.

Ultimately, the value of any static analysis tool comes down to getting as
many potential failures and error conditions out of your head and into
something recorded, so that you can address them one by one, decide for
yourself if it's a problem, and move on to reasoning about whether or not your
program addresses the reasons you're writing it in the first place.

_Thanks to [snowsuit][4] for leading me to this thought_

  
[^1]: [PySonar](https://yinwang0.wordpress.com/2010/09/12/pysonar/) is especially cool. See also [ternjs][6] for javascript.


   [1]: http://www.erlang.org/doc/man/dialyzer.html

   [2]: http://stackoverflow.com/questions/35470/are-there-any-static-analysis-tools-for-python

   [3]: note-pysonar

   [4]: http://snowsuit.github.io

   [6]: http://ternjs.net/

