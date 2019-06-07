% Small, Life Problems? Just a Matter of Programming
% c, dwm, hack, keep-it-simple-stupid, #pinned
% 2012-03-23


Last year, we had a daughter, and it became my mission to spend at
least 15 minutes a day with her. I'd love to spend much more time with
her daily, but her bedtime (and spastic sleep schedule) nearly
overlaps with my work schedule
+ commute.

People in NYC, for whatever reason, like to go to work late and stay
late. I can't help but wonder if this is related to some perception of
avoiding crowded trains, but most of the jobs I've had have done
similar things. And we all know that if everyone is doing something to
avoid the same problem, no one is avoiding it at all.

Until the switch to [DST][1], I was doing fairly well ensuring that I
left the office right at 6 o'clock (a half hour earlier than others in
the office--I come in at least a half hour earlier to make up for
this) to catch the train that would get me home by 6:45. But, as the
days got longer, there suddenly was no visual cue that it was time for
me to go.

I use [dwm][2] as my desktop environment/window manager. If you're not
familiar, it's a very simple tiling window manager for [X][3], which,
out of the box doesn't even have a clock. It's stupidly easy to get a
clock on the screen of course, but for whatever reason I hadn't taken
the time to configure it.[^1]

It dawned on me yesterday that if dwm supported multiple colors for
the status bar, I could turn the clock a different color just before 6
o'clock--my target departure. That'd be my visual cue.

Well, it turns out that multiple colors in the status bar is a [solved
problem][5], and so setting this up [was just a small matter of
programming][6]. Now, at 5:45, the background of my clock turns red
indicating to me that it's time to start wrapping things up for the
day.

Is it the most robust solution out there? It works, but certainly
not. When I need to create some other notification, monitor some stat,
or watch the weather--well, I've learned that it's just a simple
matter of programming.[[2]][7]

  [^1]: Actually, this isn't entirely true. While dwm didn't display a clock, my [GNU screen](http://gnu.org/software/screen) status bar did--of course that only shows on tag 1--which I don't see when my screen is fullscreen [Emacs](http://gnu.org/software/emacs).

  [^2]: "HA! You idiot. This is not how the Jargon file defines SMOP!" Right you are. I'm not using the ironic definition. I'm saying that sometimes simple problems have stupidly simple solutions that aren't elegant but do the job anyway.

   [1]: http://en.wikipedia.org/wiki/Daylight_Savings_Time

   [2]: http://dwm.suckless.org

   [3]: http://en.wikipedia.org/wiki/X11

   [4]: #footnote-screen-clock

   [5]: http://dwm.suckless.org/patches/statuscolors

   [6]: https://github.com/apgwoz/dwm/blob/master/dwmstatus.c#L114

   [7]: #footnote-matter

