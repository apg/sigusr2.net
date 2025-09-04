% From Concept to Reality in 30 Days
% 
% 2006-04-04

The funny thing about [Django][1], is that once you start using it, it's not
easy to look at web applications the same. It's quick, easy to use and written
in Python, my 3 favorite things. So it's not uncommon for someone like me to
have this sort of view. This doesn't mean it's entirely true and by no means
am I saying "my company sucks."

Where I work, our primary language is ColdFusion, which we use in conjunction
with the Fusebox framework. This makes it pretty easy for us to build
different components which we can swap out of one project and into another,
much like [Django][1] applications. When we swap things out however, there are
always little changes that need to be done, usually adding fields or making
something required or not required. The problem here is that we then have to
add view code for the admin side of things, add it to all the stored
procedures, etc, etc, etc. With [Django][1], a change like that is two or
three lines, in probably 50% of the cases.

In [Django][1]'s default install (0.91), a developer gets a huge portion of
what our company offers, right out of the box, by way of Generic Views and the
default Admin (with user management and permissions). Most of the basic
functionality, our CMS handles, involves pulling things straight out of the
database, and onto the page, all of which could be done with generic views.
Quite often we do this without a pager ([Django][1] could do this for us),
which for whatever reason our clients usually decide is unneccessary.

On the Admin side, it is usually straight CRUD, just giving the user an
abstract look at the database. This is nothing [Django][1] couldn't handle out
of the box. In fact, [Django][1] would probably do this better since it keeps
a history of who did what and when it happened. (We don't keep these records).

Our content management system, for all intents and purposes could be rewritten
in [Django][1] in a few hours at most and would result in a much faster
development cycle and possibly be faster than enterprise level ColdFusion. Not
only that, but it would be much cheaper (ColdFusion + MSSQL costs big bucks),
and provide better urls than our solution, which in turn would be better for
Search Engine Optimization. Since [Django][1] runs on LAMP, LAPP (for postgres
users), and LASP (for sqlite3), a [Django][1] application could be deployed
for the cost of a server, and a few hours of development time.

Am I saying that my place of employment isn't smart for using the quickest
(and probably better) tool for the job? Not at all. The solutions they provide
are of high quality, and they've been around for over 10 years serving pages
for lots of major clients.

I'm just saying that with the help of [Django][1] at this shop, we could
increase the number of projects we're working on, and the number of projects
that go live within a month. And the month is only because of design rounds
and production.

   [1]: http://www.djangoproject.com (The Django Project)

