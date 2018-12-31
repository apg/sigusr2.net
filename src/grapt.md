# Grapt

In [Plotting Points][points], I ranted about how it's difficult to
take a column of data and turn it into something visual without
resorting to non-[CLI][cli] tools. Well, with [grapt][], I may have
done something about it, or at least started on something that could
be a reasonable solution.

If you recall, the problem statement was something along the lines of
"Why can't I tack on `| linegraph` to get a nice DWIM graph?"

`Grapt` allows me to easily do this:

    $ iostat 1 30 | grep 'sda' | awk '{print $2}' | grapt -o iops.png

which gives me:

<figure>
<img src="./i/grapt-iops.png" alt="Output of iostat iops as a graph made from grapt." />
<figcaption>Output of iostat iops as a graph made from grapt.</figcaption>
</figure>

There's no padding. No scale. No features. It just reads in a column
(or 2) of data and draws a graph. I *do* however plan on adding a few
features when I get around to it (or need them):

1. Padding around the viewing window. Even a few pixels would help here.
2. Multiple series on the same chart, ideally with independent scales,
   if I can DWIM it well enough. I don't really want to build out labels,
   and tick marks and that type of stuff, but we'll see.
3. Multiple series will likely imply a need for specifying colors in
   an appropriate way. My guess is that this will be doable by reading an
   environment variable `GRAPT_COLORS` which would then just be a comma
   separated list of RGB triplets in hexidecimal.
4. Smoothing. Likely just a moving average smoother for simplicity's sake.
5. Support for distributions of some kind. Bonus points if I can
   support something truly neat like [frequency trails][freqtrails]
   easily.

These things probably won't be added until I need them, as the basic
usage fits most of my graphing needs when using the CLI, but only time
will tell.


[points]: /plotting-points.html
[cli]: https://en.wikipedia.org/wiki/Command-line_interface
[grapt]: https://github.com/apg/grapt
[freqtrails]: http://www.brendangregg.com/frequencytrails.html
