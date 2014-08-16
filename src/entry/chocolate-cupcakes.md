% Chocolate Cupcakes
% recipe, scheme
% 2008-12-15


Every now and then, I dust off my copy of _[The Little Schemer][1] ![][2]_ and
read a few sections. It's a great book, in a unique and friendly format.

Part of the reason I pick up the book time and time again is because re-
reading sections of it re-enforces the basic concepts that a Scheme programmer
needs to know. Now I'm not saying I don't know these basic concepts, because I
do, but my daily routine unfortunately does not involve Scheme. Instead, I
spend my time not recursing on `cdr`s but using [imperative][3] style looping
constructs.

And, since I like to make my own junk food, it's the perfect book since it
leaves pages for jelly stains and provides a chocolate chip cookie recipe. I
thought it'd be nice to share my new favorite cupcake recipe, with peppermint
icing--just in time for the holidays. I've stolen [Friedman][4] and
[Felleisen][5]'s recipe format:



    (define (chocolate-cupcakes)

      (bake '(375 degree) '(20 minute)

            (mix (mix '(sugar 1 cup)

                      '(flour 3/2 cup)

                      '(baking-soda 1 teaspoon)

                      '(cocoa 1/3 cup)

                      '(apple-sauce 1/2 cup)

                      '(cold-water 1 cup)

                      '(vanilla 2 teaspoon)

                      '(salt 1/2 teaspoon))

                 '(vinegar 2 tablespoon))))


    (define (peppermint-icing)

      (mix '(butter 1/2 cup)

           '(powdered-sugar 2 cup)

           '(vanilla 1 teaspoon)

           '(peppermint-extract 2 teaspoon)

           '(milk 3 tablespoon)))


Enjoy!

   [1]: http://www.amazon.com/gp/product/0262560992?ie=UTF8&tag=siusdesi2-20&l
inkCode=as2&camp=1789&creative=9325&creativeASIN=0262560992

   [2]: http://www.assoc-amazon.com/e/ir?t=siusdesi2-20&l=as2&o=1&a=0262560992

   [3]: http://en.wikipedia.org/wiki/Imperative_programming (Imperative
Programming)

   [4]: http://www.cs.indiana.edu/~dfried/

   [5]: http://www.ccs.neu.edu/home/matthias/

