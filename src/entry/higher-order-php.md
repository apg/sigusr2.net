% Higher Order PHP
% functional, php, lambda-the-ultimate
% 2009-02-06

There's no doubt in my mind that [Higher order programming][1] is value added
to a programmers toolbox, and with the pending release of PHP 5.3, it's about
to become much more mainstream.

[PHP][2], as it stands currently (pre 5.3), already has some support for
higher-order programming by passing around the names of functions as strings.
It also supports so-called [anonymous functions][3], via the
[`create_function`][4] function, though PHP does give those functions a name
(something like "`lambda_N`").

Functions created in this way, or created in the "normal" PHP way, can then be
used in library functions such as [`array_map`][5], [`array_reduce`][6] and
[`array_filter`][7]. These are extremely useful for performing some action on
each element of the array passed as an argument, but it seems rare that PHP
programmers actually use them in practice.

It may be the case that most PHP programmers don't know about these functions,
or it could be the case that these functions are extremely awkward to use
currently due to the requirement of having already created the function
elsewhere.

See, in nearly every major language that supports higher-order programming, a
concept of [closure][8] comes into play. This is extremely useful when
creating functions one off anonymous functions to pass around. Basically, when
a function is defined within some lexical scope, variables that are
"[free][9]" in that function must be "[bound][10]" in an enclosing
[environment][11], or when one uses a variable it will be undefined, causing
an error. The solution is simply to keep a reference to the enclosing
environment when the function is created.

` (define (make-counter starting) (lambda (increment) ;;; the variable
`starting` occurs "free" in this function (set! starting (+ starting
increment)) starting)) (define count (make-counter 0)) (define count2 (make-
counter 3)) (count 1) ;;; sets starting in the closure 'count' to 1 and
returns it (count2 5) ;;; sets starting in the closure 'count2' to 8 and
returns it (count 3) ;;; sets starting in the closure 'count' to 4 and returns
it `

In the example above, `count` and `count2` both refer to different starting
values. `make-counter` "closed" over the environment each time it was called
and produced closures. `make-counter` has the effect of being a function
factory that stamps a starting value on the function returned.

So, until the alpha release of PHP 5.3, it was impossible to create a function
that referred to it's enclosing environment without lots of hackery. PHP 5.3,
makes it possible, but in a slightly awkward way.

` function make_counter($starting) { return function ($increment) use
(&$starting) { $starting += $increment; return $starting; }; } `

PHP doesn't allow you to keep a reference to the entire enclosing environment;
instead you must explicitly state which variables you want to be able to refer
to. Taking that one step further, you must decide whether or not you want that
variable to be re-assignable and "pass by reference," if you do. I think it's
a little clumsy, but the introduced `use` keyword at least makes these things
a little bit self-documenting.

` $cnt = make_counter(5); $cnt2 = make_counter(15); echo $cnt(5) . " ==
10?\n"; // outputs: 10 == 10? echo $cnt2(-15) . " == 0?\n"; // 0 == 0? echo
$cnt2(5) . " == 5?\n"; // 5 == 5? `

As you can see, it does pretty much the same thing as was done in the Scheme
example above.

So now it's time to exploit it.

### Introducing Fn.php

Throughout the history of [functional programming][12], programmers (and non
programmers alike), have identified many useful functions for performing
operations. These include the functions above such as `array_map`,
`array_reduce` and `array_filter`, but also things like [`curry`][13], which
given a function `g` and an argument `a`, returns a function that promises to
call the function `g` with the argument `a` plus whatever arguments are passed
to it. In effect, `curry`, delays the function call until more knowledge is
known. (Incidently, the `make-counter` function uses currying.)

[Fn.php][14] is an attempt to define lots of useful higher-order functions to
PHP, and fix some of the things that are inconsistent with the others. Fn.php
already supports the things in PHP that already exist, but adds `foldr`,
`compose`, `zip`, `andf`, `orf`, `not`, `any`, `every`, `curry`, `I`, `K`,
`S`, `flip` and a new short hand way to define functions with strings.

There's virtually no documentation, and very little in the way of examples or
tests. It was started on a whim yesterday when I woke up, so we'll see where
it goes.

   [1]: http://en.wikipedia.org/wiki/Higher_order_programming

   [2]: http://www.php.net

   [3]: http://en.wikipedia.org/wiki/Anonymous_functions

   [4]: http://www.php.net/create_function

   [5]: http://www.php.net/array_map

   [6]: http://www.php.net/array_reduce

   [7]: http://www.php.net/array_filter

   [8]: http://en.wikipedia.org/wiki/Lexical_closure

   [9]: http://en.wikipedia.org/wiki/Free_variable

   [10]: http://en.wikipedia.org/wiki/Bound_variable

   [11]: http://en.wikipedia.org/wiki/Scope_(programming)

   [12]: http://en.wikipedia.org/wiki/Functional_programming

   [13]: http://en.wikipedia.org/wiki/Currying

   [14]: http://hg.apgwoz.com/fn-php

