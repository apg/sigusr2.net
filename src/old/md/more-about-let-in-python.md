% More About "let" in Python
% python, hacks, whyohwhy
% 2014-11-13

Yesterday, in [A "One" Line Echo Server Using "let" in Python][echo],
I showed how one can utilize Python `lambda`s to simulate `let` style
bindings in Python. There is obviously some ugliness when trying to
actually do this though--not to mention a bunch of limitations with
it:

    (lambda x=1, y=1:
       (lambda z=x + y:
           z)())()

Works, and returns `2`, but we need to immediately call each new
function that we had created. This adds to the syntactic noise, and
also makes for some difficult to track down bugs. If you don't *call*
the function, the created function will be returned. Not the end of
the world in a statically typed language, but a possible risk in
Python if not properly tested.

One mechanism to deal with this is [trampolining][trampoline]. Instead
of calling the functions immediately, we call each function as it's
returned to us in a loop. Observe:

    x = (lambda x=1, y=1:
          (lambda z=x + y:
             z)
    x = x()
    
    while callable(x):
       x = x()
    
    print(x) # => 2

There are some other pretty big limitations. Whereby in Scheme, `if`
is an expression, in Python it is a syntactic statement, and therefore
cannot be used in the body of a `lambda`, sort of. Python also has an
expression based conditional `<consequent> if <condition> else
<alternate>`. This can be utilized, but nesting these expressions is
pretty sore on the eyes.

An alternative approach embeds a `case` style statement within a
dictionary:

    {'foo': lambda: <body to execute for case 'foo',
     'bar': lambda: <body to execute for case 'bar', 
     ... }.get(casetest, lambda: None)()
     
Loops, however, require some creativity. Generator expressions are
extremely helpful here, but must be paired with a forcing function to
actually generate the values. The [echo server][echo] from yesterday
utilized `map` and the `count` function from itertools to create an
infinite socket accept loop. Unfortunately, `map` is lazy, which
required forcing values from the generator by using the eager `list`,
which also would, in this case, result in unbounded memory
consumption.

One thing to note, as I said in a [footnote][def] yesterday, is that
nested `def`s can do the same exact thing, but without the expression
limitation that `lambda` brings with it. First, though, it'd be
convenient to solve the problem of immediately calling the newly
created functions:

    let = lambda func: func() and func
    
That allows the following to "just work":

    def print3():
        @let 
        def _(x=1, y=2):
            @let 
            def _(z=x + y):
                print(z)
    
I'll be the first to admit that this *too* isn't pretty, but being as
people argue against [parentheses][parens] all the time, maybe someone
would prefer it.


[echo]: /one-line-echo-server-using-let-python.html
[trampoline]: https://en.wikipedia.org/wiki/Trampoline_%28computing%29
[def]: /one-line-echo-server-using-let-python.html#fn:3
[parens]: https://fare.livejournal.com/77842.html
