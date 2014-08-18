% Stupid Programming Language Arguments
% scheme, ocaml, ridiculous
% 2014-08-15

I have to say, there are few things finer than idiotic metrics for why
one programming language is better than another, but by far, my
favorite is "how many characters does it take to produce a local
binding?"

Some languages do quite good here--Go uses just 2 characters `:=`, for
instance. But, I'd like to compare two languages that are quite
similar--they encode different variants of the Lambda Calculus. OCaml,
which implements the typed Lambda Calculus, and Scheme, which
implements the untyped Lambda Calculus (more or less).

Both languages introduce local bindings with the symbol `let`, and
`let` in both constructs is equivalent sugar to immediately applying
an abstraction of a single parameter with the bindings value. In
Scheme, this desugaring looks something like[^1]:

    (let ((x 1)) (+ 1 x))       
           
    ;; is functionally equivalent to:
    
    ((lambda (x) (+ x 1)) 1)
    
OCaml looks like so:


    let x = 1 in
       1 + x
    
    (* which is functionally equivalent to: *)
    
    (fun x -> x + 1) 1

Just by looking at the number of characters used for the desugared
version, Scheme looses by quite a bit. And, even the sugared version
is close, with OCaml *inching* ahead. The story quickly changes,
however, when a programmer must introduce more than one binding. In
OCaml, this looks like so:

    let x = 1 in
    let y = 2 in
    let z = 3 in
       x + y + z

For each binding, we need "`let `", 4 characters, an "`=`", 1 character,
and "` in`", 3 characters--8 characters for each new binding, minimum. A
simple formula then, is `8n` where `n` is the number of bindings.

*Update:* A friend suggested that we could use "`and`" and remove the 
need for the "`in`" here, but that implements recursive bindings which
are semantically similar to Scheme's "`letrec`" which makes it an apples
to orange's comparison.

*Update 2:* I misunderstood. "and" works here in an apples to apples way,
but the whole point of this post is that this type of comparison is stupid
anyway, so whatever.


Let's now consider Scheme:

    (let ((x 1)
          (y 2)
          (z 3))
       (+ x y z))

We get "`(let`", for 4 characters, an opening "`(`", for 1, a closing "`)`"
for 1, and a "`)`", for 1 to close out the whole form. That's 7 total
just for introducing a `let`. But, for each binding, it's only 3
characters extra: "`(`", some whitespace (which is only necessary for
atoms, e.g. symbols, characters, numbers) and a closing "`)`". The basic
formula here? `7 + 3n`

So, when introducing more than 1 binding, Scheme *clearly* is better,
and it's pretty close in the single binding case.

What does this mean? Absolutely nothing! It's a frivolous comparison
for the sake of experiment, but is there a point here?

Sort of. I choose to compare Scheme and OCaml simply because they are
sort of duals of each other. On the one hand, you have Scheme, vastly
flexible and superior because of it's dynamic nature. On the other
hand, you have OCaml, vastly flexible and superior because of it's
static nature. 

Now you can argue all you want about why dynamic languages are a
[special case](harper) of staticly typed languages[^2], but that
argument doesn't help anyone get any (non PL-research) work done, just
as character/typing overhead for local bindings, proves nothing about
the merit of using one language over another.

Move along, and let people use whatever tools they need to use to
get shit done, and spend time with their families.

[^1]: This isn't really the whole story, but it's close enough for illustrative purposes.
[^2]: The punchline of the argument is that a dynamic language has but one single static type.

[harper]: http://existentialtype.wordpress.com/2011/03/19/dynamic-languages-are-static-languages/
