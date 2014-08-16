% unipoint-mode: A minor-mode for Mathematical Unicode
% emacs, inspiration, unicode, programming
% 2010-11-04



Last week I [reblogged][1] Poul-Henning Kamp's article ["Sir, Please Step away
from the ASR-33"][2].

If you haven't read it, do so. It has an interesting outlook on what it means
to write code, and asks the question, "why the hell are we stuck in the 60s?"
His basic premise is that we're married to ASCII, despite having all sorts of
other characters at our disposal via unicode, which provides much clearer and
concise possibilities for syntax. He also goes on about how none of us have
monochromatic screens, so color could play some role in what our programs
mean, as well as arguing that the vertical nature of code is unjustified,
since we have column editing.

If you ask me, his other arguments are crap, but the unicode argument is
completely valid. There's just one problem, have you ever tried to type
unicode? It's a mess. In Emacs, to type ->, it's `C-x 8 RET RIGHTWARDS ARROW
RET`, or if you know the hex value, `C-x 8 RET 2192 RET`. Sure, there are
[other options][3], but most editors aren't nearly as flexible to customize as
Emacs is, so who knows what it's like outside of it--I can't imagine it's any
better.

But, when I commented on the fact that "typing unicode sucks," I was greeted
by an email from a friend of mine:

> [DrRacket][4] lets you enter common Unicode characters by typing their LaTeX
name followed by control-\. For example, you type ∈ by typing "\", "i", "n",
"control-\". It's easy enough that I use Unicode in my code all the time. I
bet it wouldn't be too hard to hack something like this feature into Emacs.
--[Casey Klein][5]

This got me thinking about the problem, and in 20 minutes I had `C-\` bound to
a function that would read a TeX symbol name from the minibuffer and looked up
the unicode character in a table then spit it out. I then began to go on a
typing spree, seeing how easy it actually was to use. Of course, this wasn't
exactly what Casey had described, but it was close enough, and made it way
better than before.

It actually had some great features of it's own, too. For one, it had TAB
completion, to cut down on typing. DrRacket didn't have that.

I told Casey about my efforts and mentioned TAB completion and how "this isn't
all that bad." Casey was intrigued by the TAB completion, and I wanted to see
if `C-\` after typing \in was better--it is, but there's still value some
value in the prefixed entry.

So now I had a function bound that would do the right thing. If point was at
the end of a word boundary with a \ to the left of it (the word), it'd attempt
to convert the sequence to a symbol, otherwise it'd be left alone. If you were
in empty space or the completion failed, you'd be asked for the symbol as if
you were going the prefix route.

This all worked wonderfully well, and our conversation about why I don't like
entering unicode went on; he was still convinced it was woefully easy. I, too,
was beginning to see that it's not as painful with the proper tools as I
originally thought.

The 43 message thread of back and forth sparked inspiration in him around the
idea of completion for DrRacket. His idea was simple, hit `C-\` and it'd
attempt to complete whatever was before it, or output the longest subsequence
of the symbols, prefixed with whatever you typed, in the lookup table. In
other words, if you had "\sub" it'd complete to "\subset" because both
"\subset" and "\subseteq" are in the lookup table. Hit `C-\` again and the
substitution to ∈ takes place.

I couldn't help but be inspired to add that behavior to what I had now dubbed
[unipoint][6]. His [changes][7] went into DrRacket. We both found solace in
the fact that typing symbols was a bit easier than it was before.

I'm a lot less scared now to see symbols in code, and certainly advocating for
them, so long as they stay mathematical in nature. I can't help but feel,
though, that most people see this still as a problem. It's because of this
that I recorded a quick screencast tutorial (no audio) of how unipoint is
used. Hopefully it does its job in showing that it doesn't have to be so
painful, and Kamp's dream will eventually become a reality.

   [1]: http://fold.sigusr2.net/2010/10/sir-please-step-away-from-the-asr-33
-acm-queue.html

   [2]: http://queue.acm.org/detail.cfm?id=1871406

   [3]: http://xahlee.org/emacs/emacs_n_unicode.html

   [4]: http://racket-lang.org/

   [5]: http://users.eecs.northwestern.edu/~clk800/

   [6]: http://github.com/apgwoz/unipoint

   [7]:
https://github.com/plt/racket/commit/bd0ebc7511c7b66dfdd0b24d68dbe27077a9a7dd

