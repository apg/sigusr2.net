% Reduce, Reuse, Recycle
% philosophy, engineering
% 2015-03-06

The concept of [recycling][] has existed since, probably, the dawn of
humanity. Somewhere along the lines, some clever marketing person gave
this concept a brand, and a motto *"Reduce, Reuse, Recycle."* Of course,
this *isn't* necessarily a motto for *recycling* specifically, but
more for waste control in general. There is a direct parallel to some
philosophy of software development:

### Reduce

Ephemeralization is a term coined by [R. Buckminster Fuller][rbf], in
his 1938 book, *[Nine Chains to the Moon][ncthm]*. The term can be
summed up by the following quote: "more and more with less and less
until eventually you can do everything with nothing."

We talk about this all the time in software -- it's the core principle
of [abstraction][], even. Abstraction hides the complicated parts
behind a new face, and gives us a mechanism to do much more with
less. And, while we'll never get to "do everything with nothing,"
especially when you consider that abstraction is a facade, in
principle, it is the same.

### Reuse

The [Free software movement][fsm] is the primordial example of reuse
in software[^1]. It started, in part, because of a printer driver
which was unavailable for reuse due to proprietary concerns. <abbr
title="Richard M. Stallman">RMS</abbr> wasn't able to fix a bug he
encountered while printing, because he didn't have access to the
source code.

Since then, we as engineers have enjoyed the ability to build our
software on the backs of others, whether it be due to a research
paper, a compiler, a crypto library, or a debugged version of <abbr
title="Transmission Control Protocol over IP">TCP/IP</abbr>.

It's important that we continue to build upon existing resources,
existing libraries, and incorporate existing pieces of infrastructure
into our architectual designs, *whenever possible*.

Stressed, should be the point that *whenever* does not mean *always.*
There are real and practical benefits of starting from scratch. But,
when possible, at least Recycle.

### Recycle

Some software components need to be thrown out. Luckily for our
planet, there's very little environmental impact from *trashing*
digital copies of software. We turn some electricity into a little bit
of heat, and it's gone.

However, software is almost never thrown out entirely. A large portion
of the time, derived software reuses some of the code from the
original, but even if it doesn't, there are important raw materials to
recycle--experiences.

What caused us to make the *trashing* decision in the first place? What
do we know now about constructing software in this domain that will
make us better next time? These experiences are as good, and often
better than the ability to just flat out re-include pieces of old
software.

### In Closing...

We as humans have realized that for our planet to survive, we must not
waste resources -- we simply don't have an unlimited supply. We need
to make concious efforts to ensure that we're making the most of
the resources we do have. Software is no different. In order for us
as engineers to grow and innovate, and do so with increasing velocity,
we must consider the core principles, *"Reduce, Reuse, Recycle."*



[^1]: Of course, the Free software movement likely wouldn't have started without the MIT AI Lab, which had a very strong culture of sharing. In essence, the Free software movement is an extension of the AI Lab.

[recycling]: https://en.wikipedia.org/wiki/Recycling
[abstraction]: https://en.wikipedia.org/wiki/Abstraction_%28computer_science%29
[fsm]: https://en.wikipedia.org/wiki/Free_software_movement
[rbf]: https://en.wikipedia.org/wiki/Buckminster_Fuller
[ncthm]: https://openlibrary.org/works/OL4109898W/Nine_chains_to_the_moon
