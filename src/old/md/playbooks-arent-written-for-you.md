% Playbooks Aren't Written For "You"
% retrospective, operations, playbooks, #pinned
% 2017-06-22

[Heroku][heroku] engineers are on call for the services they operate,
which has great benefits, unless the team size is 2, or 3 people. To
reduce our pager burden, my team of 3, and a few other teams of 3,
combined our on call resources into a single pool, a decision that
I could never even begin to regret[^1].

While this post isn't necessarily about that, it's important to share
a few details. There are 3 different weekly rotations: Primary,
Secondary, and a per team "Expert." Collectively, each engineer
rotates through the Primary and Secondary rotations. Engineers on a
given team rotate through that team's "Expert" rotation,
separately. Sometimes your Primary shift will overlap with your Expert
shift (congratulations! You won the lottery!) The intention, of course,
is to be the escalation point for a service the Primary or Secondary
is not working with daily. It's *not* expected that an "Expert" will
be woken up under normal circumstances, and the requirement is that
they are available by phone, and within an hour of their laptop.

How does a Primary or Secondary take a page for a service they don't
work on? Well, each team attaches a URL to every page which has
information and steps to take to resolve it--a so called "playbook"
(or runbook if you will). In order to avoid being escalated to, as
an Expert, the individual teams are incentivized to a) make their
services more reliable, and b) keep their playbooks up to date,
relevant, and extremely useful.

A recent retrospective held to assess how it is we're doing with 
this shared burden, and suggestions for improvements, cast an
interesting light on our playbooks--namely, that playbooks *need* to
be written in a way that implies no previous understanding of the
service they are for. They aren't written for the service owner,
after all.  They're written to be used by someone with less context
than that, which, given the way our brains swap data out, might
actually *be* you (the service owner) in 6 months.

If you're an oncall engineer, ask an engineer in the hallway to review
the playbooks for your service, making sure that, assuming they have
*access* to the resources required, they can bring your service back
to life, even if they have absolutely no idea what it even does. You'll
be glad you did.

[^1]: We actually took it even farther given this shared pool of resources splits almost equally across two continents. The result of this is that folks in Europe are on call for 12 hours a day, roughly 06:00 - 18:00, and folks in Pacfic time are 10:00 - 22:00. This is for both Primary and Secondary only.

[heroku]: https://heroku.com
