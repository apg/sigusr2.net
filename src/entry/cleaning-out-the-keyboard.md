% Cleaning out the Keyboard
% new job, growth, keyboards
% 2012-01-15



From job to job, I travel with my own keyboard. It's a happy, little
[thing][1], but, hands down, the best keyboard I've ever owned. I own two--a
PS/2 model and a USB model. The USB model gets more use these days, but I have
a PS/2 to USB dongle that, when hooked up with a PS/2 mouse as well, saves me
a USB port[^1].

Typing on it is a bit noisy, but it is not a buckling spring keyboard, like
the trusty [IBM Model M][3]. I find it to be the perfect mix of tactile
response and noise level. The noise is soothing to me--having this constant in
my work environment is comforting.

It's very disruptive for a programmer to start a new job. No two code bases
look the same.[^2] By "same," I mean a few things. For one, styles are
different. Some people might use an [IDE][5], which might handles TABs better.
Some (smart) people just prefer spaces to TABs. Others might build modules and
packages with convention X--or convention Y.

Where do utility functions go? There might be a junk drawer over here, as well
as over here--depends on what it relates to, or who wrote it, or when it was
written. Why is that function written so sloppily? Why does this use selection
sort instead of merge sort? A new programmer coming on to a project, in a new
(to them) code base, needs to learn all of the product's kludges, debt and
inefficiencies.[^3]

To make matters worse though, the new developer, though perfectly capable of
doing so, can't just start "fixing" things[^4], that are perceived by them
to be broken, or kludgy. That'd be a horrible move. So they deal with it, and
eventually learn to tolerate it, but they might mark up the code with `//
TODO: this should not declare 70 local variables, with names starting with
`topicList.`` hoping that they'll eventually get back to it and fix it--which
may or may not actually happen.

I'm starting a new job soon, but this time things will be different. There
isn't a lot of code to inherit. There isn't enough code for there to be major
debt. There isn't a crystal clear vision as to what's actually being built.
No, what there is though, is excitement. That new company smell where anything
and everything is possible with a bit of elbow grease. Where ideas are
flowing, and product people are frolicking in meadows, dreaming up the
impossible. It's my job to execute. I'm the executor, and execute it I will.

But first, I've got to go clean out my keyboard.

  [^1]: Using one only USB port is handy, like say for laptops. The reason I own the PS/2 though is that I bought it in the days when USB for keyboards and mice on GNU/Linux was preposterous.

  [^2]: Unless of course both companies are working on the same product in sync. But how often does that happen?

  [^3]: Think about that. Technical debt has to be learned by someone new. The known wrong way to do something has to be learned by someone new. That's unfortunate, though necessary.

  [^4]: I once worked with a guy who was so appalled by the mess of code that he just started rewriting things. This was in the first month of employment mind you. He did the responsible thing and even merged it back into mainline to be released with the next round of changes. Since we launched bi-weekly or so (launching was expensive, literally, as it meant a bit of downtime while changes propagated and things came back up), there were lots and lots of changes, and with his set of "fixes" that no one else really had any idea about, things got really ugly really quickly. Luckily though, version control made it possible to removing the offending code, but it was a lot of work to get the "right" (well, wrong really) code back in place.

   [1]: http://pfuca-store.stores.yahoo.net/haphackeylit1.html

   [2]: #note-usb-save

   [3]: https://en.wikipedia.org/wiki/Model_M_keyboard

   [4]: #note-no-two

   [5]: https://en.wikipedia.org/wiki/Integrated_development_environment

   [6]: #note-debt

   [7]: #note-fixing

