% Yacking About Yet Another Yak Shave Called Lack
% hack-and-tell
% 2015-05-22

*This is an adaptation of a talk I gave at [Hack and Tell, Round 33][round-33], in NYC*

Yaks are beautiful, gentle creatures that will (probably) trample you if you get in their way.

Shaving. Well, shaving is a useless tradition, at least in my
opinion. Anyone who knows me personally knows that I don't shave my
face (or cut my hair for that matter) for periods of sometimes
years. In other words, I actually just look like a yak half the time.

But, let me tell you about a time when I shaved a yak. The job is
still unfinished, and things are already starting to grow back.

A while back, I worked on the backend for
[Heroku's Dashboard Metrics](https://blog.heroku.com/archives/2014/8/5/new-dashboard-and-metrics-beta). It
utilizes [InfluxDB](https://www.influxdb.com), but we engineered for
*"get it out the door"* and not for *"you can hit it with whatever you
got."* We still operate it as a best effort service, but have
(luckily) had very few problems with it.

Like most people in the industry, my job currently revolves around
figuring out what the hell AWS can do for me now, and so I embarked on
a proof of concept, spare time, side project to test if InfluxDB could
be replaced with [Redshift](https://aws.amazon.com/redshift/).

The task was easy. Take some portion of the petabytes of log data we
currently have in S3, download it to my local machine, parse out all
the relevant metrics, upload CSV files back to S3, tell redshift to
import all the data, realize that I screwed up the schema and fix it,
reimport, and then start running queries against it to see what it
does.

This wasn't a big data problem in my eyes. This is a problem that I
could solve with grep, AWK, and curl.

So, I embarked on downloading a day's worth of logs. That's all I
needed, because that's all that the dashboard shows. It didn't
download 5 minutes of the day before I had a gigabyte of logs sitting
on my machine. Clearly this wasn't going to work as planned. But,
something like 80% of the log lines were irrelevant.

What I really needed to do was just filter and transform as the files
were streaming to me. Then I could upload each part, delete it, and
let the whole thing run over night, or for 2 days, or however long
necessary.

One AWK script later, I had all the data I needed, in the format I
needed, and I was just about to wire it all up, but, I realized
something. A filtering tool specific to log lines with [logfmt](https://brandur.org/logfmt),
might be both fun and useful. There are, of course, tools out there
that understand logfmt, but I wanted to be able to *query*.

Since I write a lot of Go, and deal with log parsing a lot, I figured,
I'll just write a tool in Go and I'll have something pretty useful
that I can expand upon. I just wanted the ability to query fields
(e.g. `bytes_transfered<100`), and reformat lines into arbitrary
output formats, eliminating unnecessary fields. That thing was called
[lack](https://github.com/apg/lack).

How do you parse the query? You use a tool like [Yacc](https://en.wikipedia.org/wiki/Yacc).

After about 6 hours of work, I had a tool that could arbitrarily
transform logfmt messages into CSV (or anything else for that matter),
and could do somewhat advanced queries against the data. It processed
a half gigabyte log file in about 15 seconds.

Surely, it can be done faster though, right?

I tried
[Compiling to Closures](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.90.6978)
instead of walking the generated
[AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree) for each
line. I've successfully employed this strategy before for other
[languages](https://github.com/malgorithms/toffee/pull/12). Quite
simply, you analyze the AST once, then create a tree of closures which
you call later.

Unfortunately, this didn't speed anything up. If I had to guess, I'd
guess that closures in Go are just implemented as an
[object](https://people.csail.mit.edu/gregs/ll1-discuss-archive-html/msg03277.html),
with a single method, Apply. No different from any other object in Go.

I had planned to make use of a feature that would allow me to query
for things like "before 2 days ago," but wasn't actually using it. The
profiler said that all this time parsing was expensive, and I wasn't
parsing it on demand. I eliminated it, and the time to parse half a
gigabyte went down to 5 seconds.

The profiler then showed memory allocations and map operations taking
up a large slice of time. I'd seen this before in Go, and had some
strategies to employ. Use stack allocated storage, and reuse it at
that. This was easy because I was writing a filter and only cared
about a single line of input at a time. Using arrays and linear search[^1],
we're down to 3 seconds per half gigabyte.

As a last attempt, I cut the time in half by just doing lazy
conversions of values. Half the key value pairs within a line are
never utilized by the query, and therefore the work to convert them to
integers, or floats, or whatever they are, is strictly
unnecessary. Now I was down to 1.5 seconds per half a gigabyte.

It was already acceptable at 15 seconds, of course, but it sure felt
like a great win.

I had to ask myself, though, *"How would it compare if I wrote it in C?"*

Enter [qryp](https://github.com/apg/qryp).

I'm still working on it. It's been a fun little diversion which I have
big plans for. Lack supported a lot of different query operators. qryp
will too, but in addition, I'd like to add field aggregations, so that
I can do things like, select the maximum bytes transferred by resource
requested--or just sum the bytes transfered for a given application.

### Sometimes shaving a yak produces something of value.

Lack is a more expressive grep, basically. Grep is a fine tool, but
regexes are limited in what they can do. AWK one liners which filter
based on the values of key-value pairs within data are not for the
faint of heart. And, even with lack's limited utility, I've saved a
ton of headache and a ton of time when sorting through log files.

### Sometimes shaving a yak is a learning experience.

And, not in some meta-philosophical way. If there's one thing *I*
understand completely it's how to procrastinate.

But, so far lack / qryp has done a bunch of things for me:

#### It reenforced the idea of quick experiments and prototypes.

Lack was written in 6 hours. I spent an additional hour on some
optimization while my kids were napping. Despite it's prototype
status, it's been extremely useful. Like I said before, I use it quite
a bit.

Aggregations and other functionality are at this point unproven, so
I'll tread cautiously. I may be able to just compose with other tools
and be fine.

Also, I'm not saying that Go is only good for prototyping. The fact
that I'm rewriting lack as qryp in C is for me. Lack was prototype
quality. I cut a lot of corners to get it working, but could easily
go back and rewrite it with good principles in mind and produce
something both of value, and fast.

I just want to keep my C skills "fresh" before those muscles atrophy.

#### Lack reenforced some key ideas in regards to performance optimization.

* Don't spend lots of time on "magic" fixes. *QUICKLY* prove or
  disprove the validity of a theory and move on.
* Only look at the critical performance problems. Use available
  measurements and metrics!
* Don't do unnecessary work (parsing time, lazy conversion)

#### Validation that Go is *fast* enough

My job doesn't give me the latitude to ask, "What if this were
rewritten in C?" The discipline required to write something in C,
makes C a poor choice for most things.

Go on the other hand, is *easy* to write, and while it requires
discipline (like any other programming language), it's much more
forgiving. It also produces relatively efficient code, and the
compiler gets better all the time.

### Sometimes shaving a yak produces surprising results.

At this point in the presentation, I showed a picture of myself back
in 2014 -- the last time I both shaved my beard and cut my hair
short. I look completely different in that picture than I do now. But,
back in 2014, I saw a lot of the same people. They *knew* what I looked
like, but had to be reminded of it. 

### And sometimes...

And sometimes shaving a yak means you'll never understand whether
Redshift will be a suitable replacement for InfluxDB. Anyone have any
thoughts on the matter?


[^1]: The number of logfmt key value pairs per line is relatively small--10 would be a lot in most cases. Reusing the storage allocated for an array in which I'm linearly searching means I only ever need to reallocate when the line exceeds the maximum number of pairs, which is rare. And, of course, for loops on arrays scanning linearlly tend to be pretty efficient...
[^2]: Or, there's just something deeply wrong with me...

[round-33]: http://www.meetup.com/hack-and-tell/events/222504163/

