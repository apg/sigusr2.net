% Smart Moves in URL Shorteners
% urls, encoding
% 2012-03-13

It's no secret that virtually every popular (and non popular) site on the web
has a [URL shortener][1]. What if that URL shortener ceases to exist? This has
been discussed more times than I can enumerate I'm sure, but I'd really just
like to pose an observation that seems like a good idea.

We've been playing around a bit with [FreeBase][2] at [work][3]. It's going to
"solve" all of the data problems we have for a new product. In reality, it
brings up way more, but bare with me.

[Revolution OS][4], which should be in every hacker's Netflix queue if not
consumed already, on FreeBase, has many links attached to it. Now, regardless
of what you do with these links, it might make sense in the case of a film to
extract the Netflix link to pull structured information from that service as
well. The astute reader has already seen where I'm going with this, and will
point out that the link is already expanded, but bare with me--I wanted to
plug a favorite documentary (even if it is aging).

Notice the "movi.es" link. "movi.es", as it turns out, is the domain that
Netflix uses for shortened URLs. The short link for Revolution OS is
[http://movi.es/ApRqW][5]. Following that link, of course, will redirect you
to [http://movies.netflix.com/Movie/Revolution%20OS/60025132][6], which
contains a bit of interesting information in it--the unique id of the movie on
Netflix (in this case 60025132).

What's smart is that Netflix uses a predictable scheme for these mappings.
Many other services do this. And, if you search [DuckDuckGo][7] (or your
favorite search engine) for how to build a URL shortener, you're likely to
find the similar methods illustrated. In fact, Netflix just uses [Base 62][8],
though with an unnecessary twist.

The twist is that they take the id (e.g. 60025132), prepend 1 to it (e.g.
160025132) and encode the result. It's simple and effective, but unfortunately
not immediately obvious (it took about 20 minutes of trial, error and some
Python to nail this down with a coworker).

It's obvious, and simple that will save immense effort in preserving the world
wide web. We'll still need projects like [301works][9] in the fight against
[Link rot][10]--mostly for general purpose shorteners like [bitly][11].

But, for specialized shorteners like "movi.es"--well, don't over engineer it.
Do something simple, something predictable. Or, better yet, don't make me
guess! Just tell me how to reverse them. That way I don't have to crawl your
site unnecessarily.

   [1]: http://en.wikipedia.org/wiki/URL_shortener

   [2]: http://www.freebase.com

   [3]: http://nyc.okcupidlabs.com

   [4]: http://www.freebase.com/view/en/revolution_os

   [5]: http://movi.es/ApRqW

   [6]: http://movies.netflix.com/Movie/Revolution%20OS/60025132

   [7]: http://duckduckgo.com

   [8]: https://en.wikipedia.org/wiki/Radix

   [9]: http://www.archive.org/details/301works

   [10]: http://en.wikipedia.org/wiki/Link_rot

   [11]: http://bitly.com

