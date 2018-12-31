# Scanabanana

My kids have a few [Scanimation](http://scanimationbooks.com/)
books. These books add animation to the pages while flipping pages
using an animation technique called
[Kinegrams](http://thinkzone.wlonk.com/Kinegram/Kinegram.htm). I had
no idea how they worked, or even what a Kinegram was until one of the
books fell apart (after much abuse!) and I did some investigation.

The animated image is composed of an image which is built in the
following way (pseudocode):

    For each X in range 0 to WIDTH:
        frame = FRAMES[X % count(FRAMES)]
        temp = copy a 1xHEIGHT pixel rectangle from frame at (X, 0)
        copy temp to (X, 0) in the OUTPUT

This results in an image that looks like this:

![background](./i/scana-bg.png)

They then overlay a clear, acetate, screen printed with rectangles
`count(FRAMES) - 1` pixels wide, 1 pixel apart. The screen looks
pretty much like this:

![background](./i/scana-mask.png)

From there, all you have to do is align the screen on top of the
image, and it's Peanut Butter Jelly Time:

![Peanut Butter Jelly Time](./i/scana-banana.gif)

I wrote some [Racket](http://racket-lang.org) code to produce this
one. And, even wrote a little
[GUI app](https://github.com/apg/scanabanana) to explore it all with.

_- 2015/09/01_
