% More Modest
% blogs, lolz
% 2014-08-16

A few days ago, I discovered [discount][discount], a very nice
[markdown][markdown] processing tool. It wasn't until I downloaded the
source and investigated it a bit though that I saw it's potential!

See, I'm a fan of static blogs, but I've never been one to go download
one of the many [jekyll][jekyll] like things that exist out
there. Why? Quite frankly, I find them to be rather ridiculous. Lots
of random dependencies, the need for a specific version of Ruby, or
whatever the jekyll variant is written in, etc.

This blog was once powered by a 30 line Python script. It was then
powered by [modest][modest], which (I think) predates jekyll, and is
about 500 lines of unfinished Python and had way more features than I
ever cared to even bother using.

So, when I encountered the `theme` utility in the discount source
tree, I saw a great opportunity to solve blogging once and for all!
Never again will you need to worry about whether or not your Ruby is
compatible with the one on your VPS, or whether that random templating
engine is installed somewhere correctly.

This blog is now "powered" by a `Makefile`, and the little utility
from discount called `theme` that I found. The `Makefile` is less than
100 lines of [GNU]() `make` and uses Unix utilities that you already have on your
box. I use `awk`, for instance, to munge a bit of text, `sort` to
well, sort entries by date, `head` and `tail` to select lines out of
the markdown files--in fact, [it's just unix][unix].

And this thing is fast, has a lot of features, and I'm going to
proclaim that it's much better than *your* blog engine, in nearly
every single way. Just look at all these features!

* **Fast compilation times** -- `make` knows how to build only the
    things that need to be built! Amazing!
* **Static** -- get Slashdotted[^1] over and over, your VPS won't blink!
* **Almost completely depdendency free** -- just get
    [discount][discount], you almost certainly have the rest already!
* **Pages** -- cause every blog has an about page!
* **Templates** -- everyone needs a design!
* **Atom Feeds** -- I believe in syndication!
* **Markdown** -- forget HTML!
* **An index** -- just one, the front page!
* **Agnostic of version control** -- bring your own!
* **Agnostic of file layout** -- just configure it in the Makefile!

So, then, umm, how did I "upgrade" from the old Modest? 

    for n in `egrep '[pP]ublished: False' *.txt | cut -f1 -d:`; do
       mv $n $n.unpublished
    done
    
    sed -i 's/^[Tt]itle: //' *.txt
    sed -i 's/^[Tt]ags: //' *.txt
    sed -i 's/^[Dd]ate: //' *.txt
    
    sed -i 's/^[pP]ublished: True$//' *.txt
    sed -i 's/^[cC]omments: True$//' *.txt
    
    for n in *.txt; do
        rawlineno=$(grep -n '^---$' $n | cut -d ':' -f1)
        lineno=`expr $rawlineno - 1`
        echo $rawlineno ":" $lineno
        OLDIFS=$IFS
        IFS=$'\n'
    
        for line in `head -n $lineno $n`; do
            sed -i "s|$line|% &|" $n
        done
        IFS=$OLDIFS
    done
    
    for n in `ls *.txt`; do
        sed -i 's/^---$//' $n
        sed -i 's/^$//' $n
        output=${n%%.txt}.md
        ./html2text.py $n > $output
    done
    
Discount has this nice little feature which allows you to use [pandoc]()
headers in your markdown files. `theme` knows how to read those
fields, and gives them names. Since Modest worked via a similar
mechanism, the above shell script more or less turns the old headers
into the new ones. I had to do a little manual work on some posts
which had different header orderings.

Because I'm a fool, Modest made me author blog posts in HTML, which
gave me flexibility, but was tedious. I used [Aaron Schwartz's][aaronsw]
[html2text]() script to convert that HTML into Markdown. There's a few
things that still need cleanup--mostly around footnotes, but
whatever. No one reads this blog anyway!

[^1]: Slashdot was a site on the Internet that used to be a big deal.

[discount]: http://www.pell.portland.or.us/~orc/Code/discount/

[markdown]: http://daringfireball.net/projects/markdown/

[jekyll]: http://jekyllrb.com/

[GNU]: https://gnu.org

[pandoc]: http://johnmacfarlane.net/pandoc/

[modest]: /announcing-modest.html

[unix]: http://tomayko.com/writings/unicorn-is-unix

[html2text]: http://www.aaronsw.com/2002/html2text/

[aaronsw]: http://www.aaronsw.com/
