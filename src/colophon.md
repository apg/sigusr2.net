# Colophon

This is [SIGUSR2][home]. SIGUSR2 is the blog of [Andrew
Gwozdziewycz][andrew], and the name of a [Signal][signal] in the
[POSIX][posix] specification. Not surprisingly, there is a SIGUSR1 as
well.

Content is written in markdown and published using a
[ssg4][ssg4], which invokes 
[lowdown][lowdown], a markdown implementation written in C. The [RSS feed](/rss.xml) is
generated with [rssg][rssg]. All
content on the blog, including this page, and any others, unless
specifically stated, is released under the
[Creative Commons Attribute-ShareAlike license][cc], which means you
are free to repurpose the site for your own use provided you share
your changes, and provide attribution back to the author, me, Andrew
Gwozdziewycz.

SIGUSR2 has undergone many transformations in the past, and has served
as a test bed for various static blog engines. In [2008][modest], the
blog received a major refresh, and its current name. It had previously
been 23excuses. Select posts from 23excuses were kept and
republished--the rest were burned.

The refresh lasted until 2014, when I decided to center it, change its
color scheme, and rewrite all but the content. While the original
[modest][modest], the name of the static blog tool I had created, used
HTML, I was determined to use something more friendly, such as
Markdown. I used [html2text.py][html2text] to get things mostly back
in shape, but laziness means that there are still some entries that
are broken, which I fix when I encounter them and have time to do so.

This iteration of SIGUSR2 is as bare bones as I can get design wise. I
used [make8bitart][make8bitart] for the "logo," pngcrushed it, then 
base64 encoded it into the stylesheet as data.

I have unlinked many posts from the index this time around. The content
is still somewhat available, [here][here].

Under the hood, pages are served from [httpd(8)][httpd], on top of
[OpenBSD's][openbsd].



[andrew]: http://apgwoz.com
[home]: http://sigusr2.net
[signal]: https://en.wikipedia.org/wiki/Unix_signal#SIGUSR2
[posix]: http://pubs.opengroup.org/onlinepubs/9699919799/
[opensans]: https://en.wikipedia.org/wiki/Open_Sans
[more-modest]: https://github.com/apg/more-modest
[cc]: https://creativecommons.org/licenses/by-sa/3.0/deed.en_US
[discount]: https://github.com/Orc/discount
[modest]: http://sigusr2.net/announcing-modest.html
[html2text]: https://github.com/Alir3z4/html2text/
[openbsd]: http://openbsd.org
[httpd]: http://man.openbsd.org/OpenBSD-current/man8/httpd.8
[make8bitart]: https://make8bitart.com/
[here]: /old
[ssg4]: https://www.romanzolotarev.com/ssg.html
[rssg]: https://www.romanzolotarev.com/rssg.html
[lowdown]: https://github.com/kristapsdz/lowdown
