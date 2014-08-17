% Realm of Racket
% book, thoughts, racket
% 2013-12-05

_This has been in draft state since September 2013, and I've only just
rediscovered it. Disclaimer: No Starch sent me a review copy of % Realm of
Racket._

I tend to read a lot of books of a technical nature, and a couple of weeks ago
I finished up _[Realm of Racket][1]_ by No Starch Press. No Starch has put
out lots of great books recently, including (but not limited to) _[Learn You a
Haskell for Great Good][2]_, _[Learn You Some Erlang For Great Good!][3]_, and
Conrad Barski's _[Land of Lisp][4]_ (which I haven't read, but have flipped
through and would love to do so).

And, while I think that the recent batch of No Starch books targetting
functional programming is great, I thought _Realm_, overall, was just "good."

Despite the "just good" rating, I loved the presentation and delivery of the
material. As a reader, you assume the role of Chad, a sad soul, searching for
meaning in his college career. His friends suggest he becomes a programmer,
and he ends up in a dungeon maze controlled by Dr. Racket. In order to leave,
Chad must be victorious in a series of games and challenges. This, of course,
is enhanced by [Dr. Conrad Barski's][5] signature comic book style.

The book starts out with a great introduction to [Racket][6], and programming
in general. It then dives right in to building games with simple user
interfaces, focusing mostly on game state management. Managing the state of a
program is, in principle, the most important part of a program. I would have
liked to see a more functional approach taught here, rather than a purely
imperative one. Big bang, the rendering and event handling system that the
book uses, of course treats state in a functional manner, by insisting that a
user returns the state as the result of callbacks, etc. But, within the pages
of Realm, this isn't emphasized, and we're told to _modify_ the state.

In later chapters, networking is introduced using Racket's [universe][7]
library and packages. While, it's great to leave out some of the underlying,
low-level details, the packages abstractions are, perhaps, just a little bit
too hand-wavy. That being said, it'd be very hard to teach robust network
programming for low latency games in a _whole book_, without packages, let
alone a few pages.

Finally, creating an intelligent agent is presented using [minimax][8]. While
certainly not a trivial concept in itself, I thought that the explanation was
a little weak and deserved a bit more detail, especially when compared to the
treatment of game tree evaluation and laziness in the previous chapters--all
this build up for 2 or 3 rather simple functions to make use of it.[^1]

Overall, I think _Realm of Racket_ is a good book best suited for early
career programmers and students[^2]. It provides valuable lessons on
deconstructing real problems into smaller, more manageable abstractions, does
a good job at demystifying game programming in general, and does so with a fun
and entertaining presentation. Those unfamiliar with Lisp shouldn't worry, as
the first few chapters do a great job of introducing what you're getting into.
And, while I would have liked to see a deeper dive into networking and AI,
both concepts are rather advanced, and I understand the authors' intent--just
enough to demystify.

  [^1]: Irony is not lost here. The chapter on AI is short, in part, because of the chosen abstractions and work done previously.

  [^2]: But that doesn't mean advanced programmers won't find value in it, too.


   [2]: http://www.amazon.com/gp/product/1593272839/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=1593272839&linkCode=as2&tag=siusdesi2-20

   [3]: http://www.amazon.com/gp/product/1593274351/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=1593274351&linkCode=as2&tag=siusdesi2-20

   [4]: http://www.amazon.com/gp/product/1593272812/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=1593272812&linkCode=as2&tag=siusdesi2-20

   [5]: http://lisperati.com

   [6]: http://racket-lang.org

   [7]: http://docs.racket-lang.org/teachpack/2htdpuniverse.html

   [8]: https://en.wikipedia.org/wiki/Minimax

   [9]: #note-irony

   [10]: #note-audience

