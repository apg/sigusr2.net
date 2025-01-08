# "Meta-circular" Static Types

I've only ever written little languages without static typing. There's
no particularly good reason here. I'm not against static types; it's
just not the thing that I'm typically exploring.

A while back I started thinking about exploring a basic language that
was heavily inspired by <a href="https://en.wikipedia.org/wiki/Scheme_48#Implementation">PreScheme</a>. I call it "Underploy," and like
PreScheme it aims to compile a restricted subset of Scheme to as
idiomatic as possible other languages, starting with C. C, despite its
lack of type soundness, is still typically considered a "statically
typed language." From the perspective of a compiler writer targeting
C, that means that I need to have some way in which to select the
correct types. In a language as dynamic as Scheme, this is typically
hard.

Typical Scheme to C compilers don't target idiomatic C. Instead, the
output is often indistinguishable from Perl (Oh please, I kid. I cut
my teeth on Perl back in the 90s and it still warms my heart -- Ed).
Instead of machine `int`s, boxed value structs that can be *any* type
are prevalent, proving yet again that Bob Harper's observations are
sound. (Insert link to Dynamic Languages are singly typed languages).
Control flow is a mix of function calls and `goto`s depending on the
algorithm utilized to support tail calls, and closures are hoisted
into static functions with names such as `clos_HX1138`, and replaced
by one of those boxed value structs at the call site.

To say this is idiomatic C would be a stretch by most people's
standards.

PreScheme takes a different tact. You get the entirety of the Scheme
system, but PreScheme itself can only *compile* the restricted set.
That means that you can build a tower of macros that expand into the
restricted subset, but that subset doesn't include things like first
class closures, continuations, or even automatic memory management. 

The more primitive language can then be type-inferred using classic
Hindley-Milner, and everything can be compiled into machine `int`s,
`char *`, etc, and standard C functions.

So Underploy, if it's going to live in the shadows of PreScheme, needs
a similar story. I intend for Underploy to adopt a Hindley-Milner 
type inference algorithm as well, but you've gotta learn to `$X`, before
you can `$Y`, as they say.

What follows is a tiny, statically typed language written for Chicken.
There's no reason it couldn't be modified to work in other Scheme systems,
trivially, but I wanted slightly better exception handling, and used
what Chicken had. 

Now we `$X`.

Chicken has modules that can be `import`ed. I should probably have omitted
these for brevity. The `condition` module provides us with 
`with-exception-handler` which we use in the REPL. And `port` gives us the
`with-output-to-string` function that removes our need for proper string
formatting.

```
(import (chicken condition)
        (chicken port))
```

We next define the classic meta-circular Scheme interpreter's environments.
Environments hold "frames" of "bindings" that map a variable name to a value.
In our case, we build them out of nested lists of improper pairs, where the
`car` of the binding is the name, and the `cdr` is the value. The `env-extend`
function is used during function application, to extend the closure's captured
environment with the application's parameters.

```
(define (empty-env) (list '()))

(define (env-extend env bindings)
  (cons bindings env))

(define (env-set! env x v)
  (let ((binding (env-find env x)))
    (if (null? binding)
        (set-car! env
                  (cons (cons x v) (car env)))
        (set-cdr! binding v))))
```

Two functions are used to lookup a binding, such that `env-set!` can
modify the binding in place. One might otherwise implement
`env-lookup` as "just" returning the value, and interleaving the
parent environment exploration when no binding is found in the current
frame.

```
(define (env-find env x)
  (cond
   ((null? env) '())
   ((pair? env)
    (let ((v (assq x (car env))))
      (or v
          (env-find (cdr env) x))))
   (else (error "bad env: not a pair"))))

(define (env-lookup env x)
  (let ((binding (env-find env x)))
    (if (pair? binding)
        (cdr binding)
        (error (string-append (symbol->string x) ": not found")))))
```

There are only 3 real forms in this little language, and that's `if`,
`lambda`, and `set!`. These work very similarly to the Scheme relatives
except that `lambda`'s are typed. 

```
(define (if? form)
  (and (pair? form)
       (eq? (car form) 'if)
       (= (length form) 4)))

(define (set!? form)
  (and (pair? form)
       (eq? (car form) 'set!)
       (= (length form) 3)))

;; (lambda ((x type) ...) return-type body ...)
(define (lambda? form)
  (and (pair? form)
       (eq? (car form) 'lambda)
       (> (length form) 3)))
```

`lambda` differs for Scheme's as can be seen by the comment. In practice
it looks like this:

```
;;; define a function `double` in our mini language
(set! double (lambda ((x int)) int (+ x x)))
```

The evaluator is typical of a metacircular Scheme interpreter. I
cut some corners here in that `eval-lambda` returns a list with a
tag, `closure`, and a vector to access the captured environment, 
arguments, and the body itself.

Still nothing interesting.

```
(define (eval-if x env)
  (let ((cnd (cadr x))
        (cns (caddr x))
        (alt (cadddr x)))
    (if (my-eval cnd env)
        (my-eval cns env)
        (my-eval alt env))))

(define (eval-lambda x env)
  (let ((args (map car (cadr x)))
        (body (cdddr x)))
    (list &#39;closure
          (vector env args body))))

(define (eval-set! x env)
  (env-set! env (cadr x) (my-eval (caddr x) env)))
```

`eval-app` teeters on being interesting since I was
a bit too lazy to do the traditional `evlist` helper. That
means computing the `frame` to extend the environment with
is a little more chatty. In my defense, applying a Scheme
procedure is the other branch of this `cond`, and that needs
the list without names to bind to. `evlist` would, therefore,
be used once.

```
(define (eval-app x env)
  (let ((fun (my-eval (car x) env)))
    (cond
     ((procedure? fun)
      (apply fun
             (map (lambda (ex)
                    (my-eval ex env))
                  (cdr x))))
     ((eq? &#39;closure (car fun))
      (let* ((vfun (cadr fun))
             (cenv (vector-ref vfun 0))
             (bindings (vector-ref vfun 1))
             (body (vector-ref vfun 2)))
        (let* ((frame (map (lambda (b ex)
                            (cons b (my-eval ex env)))
                          bindings
                          (cdr x)))
               (menv (env-extend env frame))
               (result &#39;()))
        (for-each (lambda (form)
                    (set! result (my-eval form menv)))
                  body)
        result)))
     (else (error "don&#39;t know how to apply that")))))
```

<em>Yawn</em>.

```
(define (my-eval x env)
  (cond
   ((boolean? x) x)
   ((integer? x) x)
   ((procedure? x) x)
   ((symbol? x) (env-lookup env x))
   ((if? x) (eval-if x env))
   ((set!? x) (eval-set! x env))
   ((lambda? x) (eval-lambda x env))
   ((pair? x) (eval-app x env))
   (else (error "fail"))))
```

We finally get to the type checker. It's basic. For an `if`
expression, we require that the conditional be `bool`, and then
require that the consequent and alternate expressions, when
recursively type checked, are the same type. It doesn't matter <em>which</em>
type, they just have to be the same. The type of the `if`, then, is
the type of one of those expressions (doesn't matter which one).

```
(define (type-of-if x env)
  (let ((cnd (type-of (cadr x) env))
        (cns (type-of (caddr x) env))
        (alt (type-of (cadddr x) env)))
    (if (eq? cnd &#39;bool)
        (if (eq? cns alt)
            cns
            (error (with-ouput-to-string
                    (lambda ()
                      (display "type check failed in if: consequent is: ")
                      (display cns)
                      (display " alternate is: ")
                      (display alt)
                      (display ". must be equal.")))))
        (error (with-output-to-string
                 (lambda ()
                   (display "type check failed in if: condition needs bool, got: ")
                   (display cnd)))))))
```

I think I assumed the type of a `lambda` would be trickier, but it's not.
It's really the only place in the code where the types need to be declared,
and hopefully that will change with type inference, and polymorphic types.

Given we require that the function parameters are typed, and the return type
is given, we just assume the programmer knows what they're doing, and ship
it. I don't see why walking the tree wouldn't work to ensure consistency, but
I believe in trust. I want to believe in trust.

```
(define (type-of-lambda x env)
  (let ((targs (map cadr (cadr x)))
        (return (caddr x)))
    (list '-> targs return)))
```

OK, but seriously. The assuming the `lambda`'s parameters are given correctly,
we can at least check that they are consistent and that the declared return
type is consistent as well:

```
(define (type-of-lambda x env)
  (let* ((frame (map (lambda (p)
                      (cons (car p)
                            (cadr p)))
                    (cadr x)))
         (return (caddr x))
         (nenv (env-extend env frame)))
    (let loop ((exps (cdddr x))
               (result &#39;void))
      (if (null? exp)
          (if (equal? result return)
              (list '-> (map cadr (cadr x)) result)
              ;; in reality, we'll probably never see this error
              ;; message since an application will fail, or otherwise
              ;; before we do.
              (error (with-output-to-string
                       (lambda ()
                         (display "type check failed in lambda: ")
                         (display "mismatched returns. declared: ")
                         (display return)
                         (display " actual: ")
                         (display result)))))
          (loop (cdr exps)
                (type-of (car exps) nenv))))))
```

When we build the returned type `(list '-> (map cadr (cadr x)) result)`,
notice that we're taking the declared types of the functions parameters,
as we assumed in the previous definition of `type-of-lambda`. That's fine,
now. We've checked to ensure they're consistent with the body.

The type of `set!` is actually quite interesting. If, given our current frame,
we can see this variable, we're forcing it to be the same type. If the variable
is new, `set!` will modify the environment updating the "type binding" with the
type of the expression we're using for the value. 

```
(define (type-of-set! x env)
  (let ((exists (env-find env (cadr x)))
        (te (type-of (caddr x) env)))
    (if (null? exists)
        (begin
          (env-set! env (cadr x) te)
          te)
        (if (equal? te (cdr exists))
            te
            (error (with-output-to-string
                     (lambda ()
                       (display "type check failed in set!. type is: ")
                       (display (cdr exists))
                       (display " got: ")
                       (display te))))))))
```

Given we've trusted the programmer to ensure that `lambda` functions
are typed correctly, and then verified their work, we're just going to
double check that the call's arguments are consistent with the
function being called. We call `type-of` on each expression, and then
validate that the `lambda`'s type matches the application's type. We
can do this because we've checked the `lambda`'s return type is OK.

```
(define (type-of-app x env)
  (let ((fun (type-of (car x) env))
        (args (map (lambda (x)
                     (type-of x env))
                   (cdr x))))
    (if (and (pair? fun)
             (eq? (car fun) '->))
        (if (equal? (cadr fun) args)
            (caddr fun)
            (error (with-output-to-string
                     (lambda ()
                       (display "type check failed at application: applying: ")
                       (display fun)
                       (display " to: ")
                       (display args)))))
        (error "can&#39;t apply non-function: " fun))))
```

<em>Yawn</em>. This is what we've been waiting for and it turns out that the
important bits are up above. No wait. The point was to show that this
is pretty simple. If you understand the classic metacircular
evaluator, then you understand "my first static type checker." 
Congratulations.

```
(define (type-of x env)
  (cond
   ((boolean? x) &#39;bool)
   ((integer? x) &#39;int)
   ((procedure? x) &#39;procedure)
   ((symbol? x) (env-lookup env x))
   ((if? x) (type-of-if x env))
   ((set!? x) (type-of-set! x env))
   ((lambda? x) (type-of-lambda x env))
   ((pair? x) (type-of-app x env))
   (else (error "unknown form:" x))))
```

Our little language doesn't have a ton of builtins, but does have
enough to illustrate everything.

```
(define initial-type-env
  (env-extend
   (empty-env)
   `((+ . (-> (int int) int))
     (- . (-> (int int) int))
     (* . (-> (int int) int))
     (/ . (-> (int int) int))
     (< . (-> (int int) bool))
     (> . (-> (int int) bool))
     (<= . (-> (int int) bool))
     (>= . (-> (int int) bool))
     (= . (-> (int int) bool))
     (not . (-> (bool) bool)))))
```

The environment for the actual values just embeds the Scheme procedure,
which the interpreter understands how to use.

```
(define initial-env
  (env-extend
   (empty-env)
   `((+ . ,+)
     (- . ,-)
     (* . ,*)
     (/ . ,quotient)
     (> . ,>)
     (< . ,<)
     (>= . ,>=)
     (<= . ,<=)
     (= . ,=)
     (not . ,not))))
```

Because our represtation of closures is primitive, and environments
are built out of standard Scheme lists, printing a closure will result in
recursively printing environments until our machine gives up. This is a 
great way to force restart your computer, but not great otherwise. In our
REPL, then, we've gotta be careful to call `my-display` which handles
printing our closure type without printing the environment.

```
(define (my-display value)
  (cond
   ((and (pair? value)
         (eq? (car value) 'closure)
         (vector? (cadr value)))
    (display "#"closure: ")
    (display (vector-ref (cadr value) 1))
    (display ">"))
   (else (display value))))
```

The last thing we'll show is our REPL. It utilizes Scheme's
`read` to read an expression, calls `type-of`, which will
raise an exception on error, and then, if successfully type
checked, calls our `my-eval`. There's of course some error
handling to ensure we get some information on errors, but the
REPL keeps going.

```
(define (my-repl)
  (let loop ((tenv initial-type-env)
             (env initial-Nev))
    (display "#> ")
    (let ((x (read)))
      (if (or (eof-object? x)
              (equal? x '(unquote quit)))
          "bye!"
          (begin
            (call/cc
             (lambda (k)
               (with-exception-handler
                   (lambda (exn)
                     (display "WHOOPS!")
                     (print-error-message exn)
                     (newline)
                     (k '()))
                 (lambda ()
                   (type-of x tenv) ;; will raise an exeption
                   (let ((res (my-eval x env)))
                     (newline)
                     (my-display res)
                     (newline)
                     )))))
            (loop tenv env))))))
```

And that's it. A simple, statically typed language that's kind of
like Scheme, built in Scheme.

The next step is to make this work without specifying types
everywhere, by implementing Hindley-Milner type inference. We'll 
follow that up with adding Polymorphic types, which will allow us 
to have functions that can take lists of `bools`, or lists of `ints`,
*or* lists of *lists*.


_- 2022/06/08_
