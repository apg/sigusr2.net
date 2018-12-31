% LinkedList NYC Solution
% silly, scheme, hack, brainfuck, #pinned
% 2012-11-05


Though I hate competition[^1], I love ice cream, and so the [LinkedList's
programming contest][2] was a worth while endeavor.

I didn't spend much time on it (I planned about 20 minutes since I had a
already planned out what I was going to)--it ended up taking about 45 minutes.
Phooey.

My play was simple. Create a program which writes a simple [brainfuck][3]
program which is executed on a 6 instruction subset of brainfuck. If you
ignore the looping construct, which is the only complicated part of a simple
brainfuck system, then an interpreter is about 6 lines of code.

To add a few lines of code for my final trick, I enlisted the help of [Run
Length Encoding][4]. The proverbial cherry on top, was formatting the code
like a bowl of ice cream (I didn't think to actually put a cherry on top--I
probably would have won if I had).


                          (define (ntbf-eval

                      x h d) (unless (null? x) (case

                  (car x) ((#\>) (ntbf-eval (cdr x) h

                 (+ d 1))) ((#\<) (ntbf-eval (cdr x) h

                (- d 1))) ((#\+) (begin (vector-set! h d

               (+ 1 (vector-ref h d))) (ntbf-eval (cdr x)

                h d))) ((#\-) (begin (vector-set! h d (-

                1 (vector-ref h d))) (ntbf-eval (cdr x)

               h d))) ((#\.) (begin (format #t "~a" (

              integer->char (vector-ref h d))) (ntbf-eval

             (cdr x) h d))) (else (error "invalid non turing

            brainfuck statement"))))) (define (prog-table) (let

           loop ((i 0) (accum '())) (if (< i 128) (loop (+ 1 i)

           (cons `(,(integer->char i) (,i #\+) (1 #\.)) accum))

            accum))) (define (rld ps) (let ((char (cadr ps)))

              (let loop ((i (car ps)) (accum '())) (if (> i

                0) (loop (- i 1) (cons char accum)) accum)

                ))) (define fragments (prog-table))(define

                (make-prog s) (let ((ls (string->list s)))

                  (let loop ((ls ls) (accum  '()))  (if

                    (not (null? ls))  (let ((p (assoc

                     (car ls) fragments))) (if (not

                      (null? p)) (loop (cdr ls) (

                        append accum (append (cdr

                         p) '((1 #\>))))) (error

                        "I only do ASCII"))) accum

                      )))) (define (decode ls) (apply

                   append (map rld ls))) (begin(ntbf-eval

                  (decode    (make-prog "LinkedList NYC"))

                  (make-vector 100         0) 0) (newline))


                        ;; LinkedList Ice Cream Bowl

                        ;; By Andrew Gwozdziweycz


The code runs in Guile 2.0+, or a sane scheme implementation that supports
`format`.

BTW, some of the [other entries][5] were pretty cool. If only they'd use their
time working on free software...

  [^1]: In fact, I've rated it [1 star][6] on [Combosaurus][7]. Why? Well, that's a long story (I suppose), but it doesn't bring out the best (in some ways, it does of course, but not the side I'd like to see) in people.

   [1]: #hate-competition

   [2]: http://www.linkedlistnyc.org/archive/issue_083.html

   [3]: https://en.wikipedia.org/wiki/Brainfuck

   [4]: https://en.wikipedia.org/wiki/Run-length_encoding

   [5]: http://www.linkedlistnyc.org/contest.html

   [6]: http://alpha.combosaurus.com/interest/competition

   [7]: http://combosaurus.com

