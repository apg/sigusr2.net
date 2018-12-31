% Announcing the Return of Modest 
% blog, modest, python
% 2008-09-08

As a programmer, I pride myself in being lazy. This might at first seem like a
horrible thing to take pride in but hear me out.

Let me give you an example related to my day job. Two weeks ago I was given a
CSV file containing 2000+ rows with the header row of
`original_request_id,request_id,first_name,last_name,amount,status_cd`. Inside
was a list of credit card transactions that were taken from development and
migrated to the production credit card processor. But, as luck would have it
some credit credit cards had expired, had insufficient funds, or were just
flat out denied when the charge authorization was requested during this
migration.

So, obviously, someone had to go through and filter through this CSV file,
find the entries that had succeeded and make note that the user was paid up in
full. Additionally, we needed to filter the failed results, find the user and
tell them there was a problem with their payment. In other words, we needed 2
lists--payees and repayors.

At this point I knew I had a few choices. I could fire up GNU Emacs, run `M-x
keep-lines`, save the buffer, undo and then run `M-x flush-lines`, save and
have two lists. Then of course, I'd have to spend the afternoon looking up
everyone in the list and recording the proper data changes.

The other option would be to be lazy and write about 5 lines of Python and
turn the 2000+ row CSV file into 2 SQL queries, which Oracle would happily run
for me. The work it takes to craft the 5 lines, would almost certainly be less
work than the manual labor.

Naturally, as the first sentence suggests I choose the second option.



    lines = open('records.csv').readlines()[1:]

    payors = [x.split(',')[0] for x in lines if '(Accept)' in x]

    repays = [x.split(',')[0] for x in lines if '(Failure)' in x]

    print "UPDATE PAID SET paid = 1 WHERE pid IN (select pid from TRANS WHERE
rid IN ('%s');" % ','.join(["'" + i + "'" for i in payors])

    print "UPDATE PAID SET paid = 0 WHERE pid IN (select pid from TRANS WHERE
rid IN ('%s');" % ','.join(["'" + i + "'" for i in repays])


But, you gotta ask yourself, "is this laziness?"

### What Does This Have to Do With Modest?

There's a concern I have with some types of software where more work is being
done than is really necessary. The definitive example is of course blogs.

Back in the early days of blogs, the writer was tasked at creating a new
static page (probably from a template) and hand crafting the HTML that made up
the new post. If the blog had comments, they might include a file using SSI to
display the comments, and the CGI script that processed comments would do
nothing more than write the comment to the file included. And even when the
first blog "engines" came out, which automated the tedious process of copying
the template and hand assembling the new page and all of the indexes, they
were just a series of forms and CGI scripts that took the values of some form
fields and generated the static HTML files for you.

But somewhere along the lines, the process of compiling a blog was over
engineered. Writers felt that their posts needed to be JIT compiled, which
required that blogs actually serve dynamic HTML rather than just serving a
static file. Suddenly, storing posts in databases became the norm, which
resulted in high loads on servers when sites like [Slashdot][1] published a
link to your site on their homepage.

The Slashdot Effect, as it became known, resulted in many things like the
community mirroring of sites, and eventually, public CDNs started popping up
like [Coral Cache][2] to mirror the site automatically. This is mostly
unnecessary when you consider the fact that had blogs not been so reliant on a
database backend, servers could have withstood a much higher load, and stood
up by themselves under many situations.

In fact, the first thing that most shared hosting companies will do in the
event of the Slashdot Effect, or Digg Effect, seems to be to take a static
snapshot of the page and serve that.

Modest cuts out the middle man. Everything is served as static HTML, because I
believe that JIT compiling blog engines do too much work. Modest doesn't
require a database, doesn't generate content on the fly, but with a little
luck, some time and a little ingenuity, I feel that it will become as full
featured a blog engine as you can get for command line use.

### Return?

A few years ago, I wrote an initial version of Modest, in Python, which
generated static HTML from simple text files. It worked fairly well, but
wasn't flexible. The date of the post was based on the files `ctime`, and
there was no way to really add metadata to the post. This worked fine, but in
some cases (e.g. `cp post.txt post.txt.bak`) you simply loose the date
information. The quick fix I came up with was to cache the `ctime` in another
file, which was just a mess. And of course, this only solved storing one piece
of metadata.

The new version of Modest, which will be released for public consumption very
soon, had unlimited metadata potential, because the entry actually contains a
spot for it. Each post is defined simply by doing:



    Some Post

    2008-10-10 10:10:32

    something, strange, neighborhood, ghostbusters


    UnknownMetaData1: gets converted to a string

    UnknownMetaData2: False


    Post body goes here, but note--the Date field becomes a Python datetime,

    and Published becomes a Python boolean True. These values become available

    in the template's context.


Because those variables become available in the template's context,
interesting things can now occur. Suddenly you can, without changing your
database schema, make certain posts behave differently, just by including a
bit more metadata and adding the appropriate logic to your templates. It's not
very MVC, but who cares about that? The point is, you get flexibility for
free, and flexibility is another part of being lazy. This all happens to tie
into another project, but we're out of time for now. Look for more of Modest
soon.

   [1]: http://slashdot.org/

   [2]: http://www.coralcdn.org/

  *[JIT]: Just in Time

  *[MVC]: Model-View-Controller

  *[SSI]: Server Side Includes

  *[CDN]: Content Distribution Networks

