% All This Talk About OSX...
% 
% 2006-04-25

I'm a Mac user.

Actually, I'm a UNIX user, who came from the brave [GNU][1] world in the late
90s, and never looked back. Now, don't get me wrong, I've loved Macs since the
early 90s when I was introduced to them, but not coming from a family with
money enough to even have a computer, I certainly didn't have a Mac. When I
finally saved enough money to buy my first PC, I had to learn enough about it
before I could take the [GNU][1] plunge, but when I did boy was it refreshing.

But, In today's [GNU][1] world, the abundance of different distributions of
GNU is way too overwhelming. In my opinion, it's actually hurting the adoption
of [GNU][1], it certainly turned me away (I now use [FreeBSD][2] for my left
over PCs, though I try to install and use the [GNU][1] stuff I've become
acustomed to over the years). When I had the chance, and my bank statement
said I could, I purchased a Powerbook. Now, I'm not going to say that I'm
unhappy with my Powerbook (it's the best computer I've ever owned), but I am
unhappy with lots of things related to it.

For one, OSX really isn't built on [FreeBSD][2] like everyone claims. What
really happened is that MacOS Developers decided to do something the [HURD][3]
people couldn't ever get right, use the [Mach][4] microkernel. They then built
a FreeBSD compatibility layer on top of it, but most of the system calls that
a programmer makes are not what [FreeBSD][2] would really do. And of course
this has to be true. Most resources in OSX are managed by [Mach][4], not the
[FreeBSD][2] layer.

Sorry to mislead you, but the real reason for unhappiness, isn't [FreeBSD][2]
related, it's X11 related. Apple, I'm sorry to say this, but X11 integrates
extremely poorly with OSX, and I'm not sure how anyone can use it daily along
side of Aqua. My biggest complaint by far is the fact that CMD-Q kills all of
my X applications. Now, in defense of Apple, they provide you a confirmation
box, which stops me most of the time from my habitual CMD-Q routine, but it's
not enough.

I'm aware of the technical hurdles that would need to be overcome to even get
CMD-Q to work on the application level, but I'm sure that the engineers hired
to build OSX could also come up with a solution to the problem.

What's more, QuartzWM is poor. Sure, it displays windows, but really does
nothing more. I suppose this is the main purpose of a window manager (display
windows), but it's helpful in no other way. I have to launch a terminal and
then xkill to destroy a window and it's subprocesses (when something breaks,
Apple did a good job of making the (x) work). Now, surely I can put that in
the applications menu, but I'd rather use the Applications menu for things
like Applications, not something the window manager should give me support
for!

Apple, please hear my cries and do something to seemlessly integrate the X
protocol into Aqua. Ok? Thanks!

**UPDATE April 25, 2006** I think it's finally about time to build the search
and something to automatically add archive information to the sidebar.

   [1]: http://www.gnu.org (GNU's Not Unix)

   [2]: http://freebsd.org (FreeBSD)

   [3]: http://www.gnu.org/software/hurd/hurd.html (Hird of Unix Replacing
Daemons)

   [4]: http://www.cs.cmu.edu/afs/cs/project/mach/public/www/mach.html (Mach)

