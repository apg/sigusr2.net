% The Hacker's Utility Belt: SSH
% ssh, tools
% 2009-05-07

Most hackers[[1]][1] aren't [Batman][2], but aside from [Batman's utility
belt][3], the Batcave and Alfred, he's no different than you and me.

He's got strength and training on his side, but I could probably take him if
he didn't have his belt and was blindfolded. Give him his utility belt though,
and I'd be doomed. That's not because his belt has super powers--instead his
belt contains tools that are useful in common situations of distress, and of
course detective work. The Bat-grappling hook for instance allows him to scale
buildings and walls, and tranquillizer darts allow him to temporarily disable
a foe. Batarangs, with practice, can be used to disable an opponent by aiming
for a body part, or to cut down a hanging chandelier, creating yet another
obstacle for foes to fight through.

Hackers have similar tools, but we carry them in "/usr/bin/" (though how cool
would it have been if the [Bell Labs folks][4] named it "/usr/belt/" instead).
The tools _we_ use, generally allow us to solve problems quickly and
efficiently--just like Batman.

The tool that I've come to rely on quite a bit recently is [SSH][5]. Most
hackers use SSH for working remotely on servers, and for copying files over
[SCP][6] or [SFTP][7]. Some may even use [FUSE][8] and [SSHFS][9] to mount
file systems over SSH. Its potential uses are endless, but the most useful
uses for me lately have been tunneling and [SOCKS][10] proxies.

### Tunneling

As a programmer who on occasion works from home (more so currently), it's
often a battle to connect to all of the company resources I need in order to
perform my job. Firewalls are good things, and are configurable to allow
remote people access to walled gardens, but with dynamic [IP addresses][11]
being the common norm, opening up resources to many different addresses gets
hard for sysadmins to track. Besides, they've got a ton on their plate
already.

To combat this problem, [VPN][12]s were created. Basically, a VPN allows one
to connect networks together as if they were local to each other. When I use
my companies VPN, it's as if I'm sitting in the office.

Often though, this isn't enough. When dealing with networks that clients own,
they may provide us access through only one entry point, a single development
server for instance, which is locked down.

This is where SSH comes in handy. In the [OpenSSH][13] implementation of the
SSH client tools, the `-L` option allows one to setup forwarding of traffic to
another host. Suppose, I wanted to connect to a client's Oracle server,
listening on port 1521:

`$ ssh -N -L 9999:clients.oracle.server.name:1521 user@your.companies.host `

Now instead of pointing our SQL*Plus client to
_clients.oracle.server.name_:1521, we point it to localhost:9999, and traffic
is forwarded via _your.companies.host_ as if we were connecting directly from
_your.companies.host_.

This is extremely powerful, and simple.[[2]][14]

### SOCKS Proxy

Similar to the tunnelling example above, OpenSSH can act like a [SOCKS][10]
proxy, allowing you to forward outbound traffic to a trusted source, which
will then carry out the request on your behalf. This is _great_ for browsing
in public [WI-FI][15] spots where attackers might be sniffing for passwords
being sent in plain text over HTTP, or some other unencrypted
protocol.[[3]][16]

And, of course, setting it up is as easy as passing the `-D` option to ssh
when connecting to you@remoteserver.

`$ ssh -N -D 8000 user@remoteserver`

Then, you can setup your computer (or web browser) to use a SOCKS proxy, and
point it to localhost:8000.

This isn't without its problems though. For one, the network could block all
outbound traffic except HTTP/HTTPS. One obvious workaround is to have a remote
server listening for SSH connections on port 143 or port 80, instead of the
protocol default of 22. But, you'll of course need your own box, or [VPS][17]
for this, as your shared hosting account will probably not allow you to change
sshd's listening port.[[4]][18]

Of course, since SSH is a protocol for sending encrypted network traffic,
there are [many][19] [other][20] uses for it. These are just the two
alternatives I find the most useful lately.

  1. I use the [jargon file's][21] definition of hacker, not the commonly
confused term [cracker][22].

  2. It can of course get much, much more complicated

  3. Unencrypted WIFI traffic is extremely easy to capture, with a tool like
[tcpdump][23]

  4. The thought of a "loopback" SOCKS proxy, "loopback" tunnel or some
combination of the two cannot work, as it would only ever forward traffic to
your localhost's sshd, which would then forward traffic to the destination,
unencrypted, over the local network--this is the exact situation we were
trying to avoid. Plus, if you're trying to forward traffic in this way, over a
port that's being blocked, it's still blocked.

   [1]: #f1

   [2]: http://en.wikipedia.org/wiki/Batman

   [3]: http://en.wikipedia.org/wiki/Batman%27s_utility_belt

   [4]: http://en.wikipedia.org/wiki/Unix

   [5]: http://en.wikipedia.org/wiki/Secure_Shell

   [6]: http://en.wikipedia.org/wiki/Secure_copy

   [7]: http://en.wikipedia.org/wiki/SSH_File_Transfer_Protocol

   [8]: http://en.wikipedia.org/wiki/Filesystem_in_Userspace

   [9]: http://en.wikipedia.org/wiki/SSHFS

   [10]: http://en.wikipedia.org/wiki/SOCKS

   [11]: http://en.wikipedia.org/wiki/IP_address (Internet Protocol Address)

   [12]: http://en.wikipedia.org/wiki/VPN (Virtual Private Network)

   [13]: http://www.openssh.com/

   [14]: #f2

   [15]: http://en.wikipedia.org/wiki/Wi-Fi (Wireless Fidelity)

   [16]: #f3

   [17]: http://en.wikipedia.org/Virtual_private_server (Virtual Private
Server)

   [18]: #f4

   [19]: http://www.ssh.com/support/documentation/online/ssh/adminguide/32/X11
_Forwarding.html

   [20]: http://unixwiz.net/techtips/ssh-agent-forwarding.html

   [21]: http://www.ccil.org/jargon/jargon_23.html#TAG833

   [22]: http://www.ccil.org/jargon/jargon_18.html#TAG365

   [23]: http://www.tcpdump.org

