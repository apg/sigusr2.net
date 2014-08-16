% Stripe CTF 3.0 Recap
% fun, challenge, ctf
% 2014-01-31


I recently took whatever free time I had, and a couple of very late nights to
play around with the problems in the [Stripe CTF 3.0][1] challenge. I was
hoping to receive a coveted t-shirt, but I in fact, won't since I didn't
complete Level 3, let alone Level 4. The levels were quite interesting, and
definitely challenging.

The setup for the challenges was all git based and worked very well, though
I'm sure Stripe's operations engineers were on call the entire time to ensure
this was the case. Submissions were done via a `git push`, and a test harness
was provided to ensure that you produced correct results, and typically
provided some measure against their naive implementation.

### Level 0: "Spell Checker"

Level 0 was a simple optimization problem. The 10 lines of mysterious code ran
in time linearly proportional to the input sizes. The goal was to ensure that
despite the input sizes, the time to run was always the same, and of course
acted as a tutorial to ensure you understood how the submission and testing
process worked.

After looking at the "mysterious" code, it was obvious that it implemented a
basic spell checker based on whether or not a word was contained in a
dictionary. Further investigation showed that the test to see if the word was
"spelled" correctly did a linear scan of the list of "valid" words.

This was written in Ruby, a language that I am not very versed in. I was able
to fumble around a bit and make some changes which didn't amount to much,
before I realized exactly what was happening. After that, the challenge was
solved by using a `Set`.

This alone didn't net me a high score, so I'm guessing that some people
rewrote the program in another language, or know some crazy Ruby optimizations
to squeeze every last cycle imaginable out of it.

### Level 1: Gitcoin mining

With success in Level 0 after a half hour or so, the Gitcoin challenge seemed
like it was going to be orders of magnitude harder. The idea of the Gitcoin
challenge? Generate a commit whose hex encoded SHA1 hash is lexographically
less than some difficulty factor--similar to the difficulty factor of Bitcoin.
There was also a race involved. You passed this challenge by pushing your
changes and mining a gitcoin before Stripe's bot did.

The script that Stripe included in the challenge worked, but was incredibly
slow, since it forked off a git process for every hash it tried to compute.
The obvious plan of attack was to write a program that could compute
compatible SHA1s given a generated string representing a commit object. This
was fairly easy to pull out of the shell script, but something tiny held me up
until I went to sleep deep into the night.

In the morning, I started investigating what `git hash-object` actually does,
by looking at unzipped commit objects in a git repo. The trick that I
discovered? Before hashing the object, git prepends `[type of object] [size in
bytes]\0`. Once I figured this out, my multithreaded, java based "miner" beat
the bot 30 seconds after the git push. Total lines of code written? About 100
--probably about half of it is debug related.

### Level 2: Web Shield

When I picked back up, I was presented with a Level 2, which simulated a
denial of service on an HTTP service. The task was to build a proxy that would
block malicious traffic and let through as much legitimate traffic as
possible.

I tried a bunch of approaches here, starting with the basic "flip a coin"
approach to understand the code and how it all was put together. It was a
node.js app, which is something that I've used quite a bit in the past, so it
wasn't troublesome for me to get started.

I turned to the KillBotz paper, implemented a basic version of that without
the CAPTCHA, and nothing really worked. Well into the night, I still hadn't
gotten past it, which is when I realized that I had recently solved a similar
problem at work. When I got up, I quickly coded it up, tested a few different
sets of parameters and sure enough, challenge defeated.

The solution wasn't the highest ranking solution, but it did well enough for
me to move on, which was fine for me. I wasn't trying to *win*, I was just
trying to *finish*. It's basically a simplified version of leaky bucket. In my
version, each client (here defined by IP address) is allowed some number of
requests, if it exceeds the request rate, it's denied for that time period.
Using a limit of 5 requests per second, completely the challenge and then
some. The malicious traffic was incredibly bursty. I played around with some
small changes to this, adjusted rates, and setup a basic feedback mechanism.
It was stated in the description that a malicious client is always malicious,
so if I could have figured out a good way to ban clients who constantly blow
the limit, I'd have done a lot better. However, the simulation lasted 20
seconds, which makes it pretty hard to differentiate between constantly
malicious versus accidentally going to fast for a while.

For this level, it would have been smart of me to build a way to test
different parameters rather than the "change, compile, test" cycle that I did.
I wasted a lot of time doing that.

### Level 3: Instant Code Search

I spent the most time on Level 3, and ultimately gave up after 2 late nights
(probably about 8 hours total) before realizing that it's was a time sink, and
completing it won't give me enough time to start working on and finish Level
4. On my local machine, I got to 3x their benchmark (4x was required) with
correct results, but in my quest to optimize the remaining bits, I fought with
my unfamiliarity with Scala and Twitter's libraries for "distributed systems
in a box."

Quite simply, the problem has you take some amount of text and index it, with
searches to be performed on 3 different "search servers." My original approach
was simply to use an Inverted Index. The simple approach ultimately failed
because there was a time limit on index creation, and I easily blew past that.
I quickly realized that allocation and garbage collection were probably a bulk
of the time, so I reduced the amount of objects I was creating. This lead to
me with a Map from token to a set of longs, which represented an index into an
array of filenames in the high 32-bits, and the line number in the low
32-bits. This sped index creation up to a tolerable level, but provided
incorrect results.

The problem was my tokenization. The included searcher essentially performed
an "indexOf" operation on the whole file, which meant that when searching for
"cat", you'd get back files that contained "catastrophe," 'catacombs" and
"cats". My initial approach wouldn't find any of those since it looked at
whole words only, not substrings.

I fixed this by creating an additional suffix array, which with a few changes
allowed me to get correct results, but only 3x their estimated benchmark (on
my local machine). I submitted, thinking that maybe just maybe, the local
estimate was bad, but the index creation timed out with more data being read
by the "production" version. The changes were pretty simple. If you find the
string in the Array, walk forward until a "startsWith" function fails. All of
the suffixes there are found for that search term.

But, back to the drawing board.

Upon further investigation of the code, I realized that I had been doing
something very silly. Each search server had the exact same copy of the index
in memory, and on disk, and I was sending the query to all three, and
disregarding all but one of the responses. I vowed to fix it, but failed to
produce something that worked at all after the changes.

#### Change 0: Reduce memory by using a SortedMap

I realized that I could eliminate the suffix array by using a SortedMap from
suffix -> postings (the file, line pairs). Most of what the suffix array
provided me was a way to get all of the inter word matches, but it came with
the cost of storing the all the suffixes twice. Using a SortedMap reduced
that, since it basically combined everything into one thing. I'm not sure it
really helped, since there's overhead in for storing pointers in the tree
nodes. It's hard to say for sure without proper tools, which I didn't use for
such a silly side project (Note to self: This would have been smart!).

#### Change 1: "shard" the index

It turns out there's not a really easy way to shard an index up front on
anything other than the whole file unless you want every indexer to touch
every file (which I was trying to avoid for time reasons). Therefore, I made a
change to distribute requests for indexing files to a different server, which
*roughly* balanced the index sizes across the 3 servers. I say roughly,
because the indexes on the test data were 3M, 9M and 8M, so obviously it
wasn't fair.

#### Change 2: Query should perform query on all search servers and combine
the results

This is where the major problem occurred. Each of the search servers returned
a JSON result, and the "Master" essentially proxied back the response without
touching it that it got from a slave. In order for me to do "scatter gather" I
had to parse the JSON, extract the results, combine them, and reassemble the
result. This sounds easy, but everything was wrapped up in a Future, JSON
parsing isn't trivial in statically typed languages, and it just flat out
didn't work in the time I allotted myself.

### Accepting failure, and moving on.

The whole thing was incredibly fun and I learned a lot. Having not
participated in CTF 2.0, and regretting it, I'll definitely participate (and
plan more time) to participate in CTF 4.0. Thanks to Stripe for doing an
incredible job with facilitating this!

My free time will now be spent working on [MicroCorruption][2] for a while.

   [1]: http://stripe-ctf.com

   [2]: https://microcorruption.com

