# Fabric not Uniforms

Too many decisions in the software industry are made, not by rigorous
evaluation or personal experience, but instead by fads that a small, vocal
minority tout.

These things aren't typically as simple as the best sorting algorithm, or a
recipe for hashing passwords<sup><a id="one-return" href="#one">1</a></sup> (maybe a bad example<sup><a id="two-return" href="#two">2</a></sup>), but are
deeper than that--I'm talking about which programming language one should use,
or which "framework," or even which message broker or database.

Many times the question about whether the problem's solution even requires a
framework, or a message broker isn't discussed. "Of course we need X, GitHub
uses it!" Or "Node.js achieves 3,000 requests/s in _NodeLover_'s (pointless)
benchmarks! Surely it's the right tool for our factorial server!"

Please, make the herding stop!

[Ian MacKaye][wiki-ian] once wrote two songs, "Straight Edge," and
"Out of Step," and accidentally started the "straight edge" movement
(or life-style, or a music scene--depending on who you talk to <sup><a
id="three-return" href="#three">3</a></sup>). Both songs are
biographical, but they were re-purposed to power other people's
agendas. Something in which he never intended. There's a great
metaphor that goes with it:

> ...if words or songs or lyrics are clothes, then really direct ideas become
uniforms that anybody can put on. So some people can use the uniform to
further their agenda and they don't engage with it, they just put it on. And
in my mind _I thought being really direct would make it much more difficult
for people to abuse the idea, but actually it's easier to abuse because the
ideas were finished and they didn't have to contribute any of their own selves
to the situation_. -[Ian MacKaye][gothamist]

In the software world there are "uniforms" too. Many of us choose to put on
the Rails uniform, the NoSQL uniform, the Node.js uniform, or even the Agile
uniforms. These tools and methodologies have movements, of course, with people
actively recruiting for them at conferences, via blog posts, books and more.
We, like fish, are attracted to "shiny buttons."

Fabric, on the other hand is a foundational idea. It's non-biased research,
from industry, from academia, from individuals on blogs. It's the type of talk
at a conference which discusses concurrency primitives in a multitude of
languages. Their pros. Their cons. It's a paper about a new data structure
which outlines its performance properties under different circumstances.

Uniforms hinder progress in our industry. The legions that uniforms create
can't see past their solutions--sort of analogous to ["the blub
paradox."][blub].

Uniforms don't always fit<sup><a id="four-return"
href="#four">4</a></sup>, and more often than not cause us to lose
site of the original problem. Instead we focus on how we can use a
belt and some thread to make the uniform fit our new problem, for no
better reason than allegiance. This leads to security problems, bugs
and bloated software, among other things. People too often tread water
dressed as a Policeman.

We need to all become better tailors. And we certainly need to learn
the art of fabric construction and selection. Only then will we be
able to create our own properly fitting clothes.

_- 2012/10/11 (Revised 2018/12/30)_

### Footnotes

1. <a id="one"/>"Just use bcrypt" -- <a href="http://antirez.com/post/crypto-dogmas.html">On Cryptography and Dogmas</a>, <a href="https://github.com/antirez/lamernews/pull/8">Old, but epic fad herd</a>.<a href="#one-return">#</a>
2. <a id="two"/>A potential bad example because even if you don't understand it, using bcrypt is fundamentally more secure than salted, or even non-salted SHA1 / MD5 for storing passwords. But the whole "don't think or evaluate for yourself, 'cause we're waving what we use at you and we're awesome" attitude is shit.Other obvious choices are scrypt, PBKDF1 or PBKDF2.<a href="#two-return">#</a>
3. <a id="three"/><a href="http://www.theedgeprojectmovie.com/">EDGE Perspectives on Drug Free Culture</a> has great explorations on the matter<a href="#three-return">#</a>
4. <a id="four"/> But, there's no denying that a uniform sometimes perfectly fits. The reason for this stems from the fact that uniforms are normally patterned after a successful project. Rails was patterned after <a href="http://basecamp.com">Basecamp</a> was created, for instance. Django was patterned after a CMS for a newspaper. There are many cases when Rails and Django (and other frameworks and languages) are absolutely the best choice for a project.<a href="#four-return">#</a>


[gothamist]: http://gothamist.com/2006/11/06/interview_with.php
[wiki-ian]: https://en.wikipedia.org/wiki/Ian_MacKaye
[blub]: https://en.wikipedia.org/wiki/Blub#Blub
