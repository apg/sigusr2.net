% Regex Fractals
% code, fractals
% 2014-10-09

I recently discovered the [Daily Programmer][dpr] sub-reddit, which
posts coding challenges 3 times a week (one small, one medium, one
hard). It's been going on for quite a while now, and looking back
through the archives I bit, I stumbled upon something that really
caught my eye--[generating fractals with regex][genfrac].

Now, I used to write Perl, so I know my way around regular
expressions, but I hadn't ever considered them as a way to generate
art. Well, OK. Writing regular expressions is an artform in and of
itself, I suppose, but it's all text based, not graphical. The
description of the challenge pointed me to this [imgur][imgur]
gallery, which explained what was happening.

There are 4 quadrants in the [Cartesian][cartesian] coordinate system. The top
right is quadrant 1, top left is quadrant 2, bottom left--3, bottom
right--4. It makes a "C" for Cartesian[^0]. Pixels can be described by
subdivisions of those quadrants, by a string containing the characters
`{'1', '2', '3', '4'}`. So, the string "11" describes the very top
right pixel of a 4 by 4 pixel image (the [imgur][imgur] makes this
pretty clear I think).

Now, if you loop through all of the strings that describe those pixels
and color only those that match a given regular expression such as
`.*1.*` a fractal will be the result. My mind was pretty much blown at
this point, and I just had to implement it. There is a somewhat
obvious recursive solution to it, and indeed many people choose to
take that route, but I wanted to do something different. I wanted to
do it *without* recursion , and in as few lines as possible.

For some reason I chose Python[^1]. Python and me go way back, but I tend
to not utilize it as much as I once did. That said, I knew I could do
the bulk of the work in 2 steps, and complete it in 4:

1. Generate all pixel "coordinates", *e.g.* the strings representing each pixel
2. Filter the stream of "coordinates" via the regex
3. Translate the "coordinates" into actual Cartesian coordinates
4. Put the pixel on a canvas.

But, how do you turn the string coordinates into a Cartesian, x, y
pair? That stumped me for a bit, but after sleeping on it, I came up
with:

    def str2pt(s):
        L = len(s) - 1
        x, y = 1, 1
        for n in s:
            y += 2**L if n in '34' else 0
            x += 2**L if n in '14' else 0
            L -= 1
        return x-1, y-1
    
Start at 1, and add `2^L` (where `L` is a recursion level[^2] to the
`Y` coordinate if in the negative `Y` coordinates, and add `2^L` to
`X` if the current coordinate is in the positive `X` range.

Tie in the following:

    def generate(n):
        return map(lambda x: ''.join(x), itertools.product('1234', repeat=n))
    
    def fractal(r, n):
        return map(str2pt, (c for c in generate(n) if re.match(r, c)))
        
and we have a solution. Of course, drawing it is just a matter of
looping over the coordinates given to us by `fractal` and putting them
on a canvas.

On a 1024 by 1024 canvas with the regex `.*1.*` a beautiful, though
admittedly skewed, [Serpeinski's Triangle][triangle] is seen.

The code is [here][code], along with the output from the [regex above][output]
 

[^0]: Some math teacher probably taught me that. But it's not nearly as memorable as "demise ate the cheese," which teaches you nothing, but will forever remind me of a chuckling high school English teacher.
[^1]: Likely because it was just there and I know my way around the Python Imaging Library
[^2]: By level I mean nesting level, where level N is the biggest container, and level 0 represents a single pixel

[dpr]: https://www.reddit.com/r/dailyprogrammer/
[genfrac]: https://www.reddit.com/r/dailyprogrammer/comments/2fkh8u/9052014_challenge_178_hard_regular_expression/
[imgur]: https://imgur.com/a/QWMGi
[cartesian]: https://en.wikipedia.org/wiki/Cartesian_coordinate_system
[triangle]: https://en.wikipedia.org/wiki/Sierpinski_triangle
[code]: https://gist.github.com/apg/0ac682c18d3a852fefcd
[output]: https://camo.githubusercontent.com/24a96cfc015891944c6ee01dc8bd314da2f06a47/68747470733a2f2f692e696d6775722e636f6d2f315850636d4a642e706e67
