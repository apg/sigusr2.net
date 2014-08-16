% Applying the Web of Trust Model to Blog Reading
% web, rss, blogs
% 2013-07-25


_This was originally written in November of 2012, but I for some reason never
published it._

There seems to be a [growing][1] [trend][2] in that reading Hacker News or
Reddit is a waste of time. But, even those who agree with that statement, seem
to also agree that reading blogs is quite valuable. Some will utilize their
social media connections to filter, and others still will just read posts from
people who they know, or otherwise respect.

How will one discover new blogs and new people to respect? I see two ways but
only one is interesting to think about[1][3].

Option 0: Code is written by people. If we discover the code (in some _other_
way) and respect it, then we might respect the author enough to read his/her
thoughts on technical things.

Option 1: Much more interesting is the Phil Zimmerman conceived ["Web of
Trust"][4], which exists primarily to distribute and verify public keys in
lieu of a certificate authority.

I regularly exchange emails with 3 other people regarding tech news, new
algorithms, interesting CS papers, things about startups, etc. They're already
in my web of trust, since I a) know them personally, b) respect their
thoughts, ideas and opinions on things. I also trust that they won't waste my
time with some frivolous article. So, I automatically read everything they
write.[2][5]

By transitivity, I'm also likely to trust some portion of articles their web
of trust writes and as a result, reading articles recommended by them, or
engaging in discussion with their web of trust, is not unthinkable. I
certainly don't have to trust them completely, that's the beauty of the web of
trust model, but I can, if I wish, choose to trust a friend's friend because I
trust the friend to not have untrustworthy friends.

Said another way, and in a more palatable way... If many people within 2
degrees of me trust a given blog, or a given author, it's very likely that I
should give them a shot by reading something of theirs.

That just leaves one thing. What does an RSS/Atom aggregator look like in this
model?[3][6]

  1. This only considers blog posts targetting to proggit, or the technical
Hacker News, which discuss technology that programmers utilize and
criticize.[↵][7]

  2. The same can be said for links they just happen to share related to the
news. I don't necessarily have to know the author if my web of trust
recommends it.[↵][8]

  3. Since first writing this, and *not* publishing it in a timely manner
(first written in November 2012), there have been a few attempts
[potluck.it][9], and maybe less so [monocle.io][10] that make the web of trust
model not so explicit, but rather subtle. [↵][11]

   [1]: https://twitter.com/fogus/status/265117597043924994

   [2]: http://prog21.dadgum.com/155.html

   [3]: #note-interesting

   [4]: http://home.clara.net/heureka/sunrise/pgpweb.htm

   [5]: #note-share

   [6]: #note-since

   [7]: #return-important

   [8]: #return-share

   [9]: http://potluck.it

   [10]: http://monocle.io

   [11]: #return-since

