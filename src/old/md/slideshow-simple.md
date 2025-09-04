% Simplifying Slideshow
% racket, slideshow, projects, #pinned
% 2017-11-04

*I gave a talk at <tt>[(seventh racket-con)][con]</tt> last month on new Racket `#lang` called slideshow/simple that is a simpler way to harness the power of [slideshow][slideshow] in a DWIM sort of way. What follows is a post adapted from the talk.*

I've been dabbling with Lisps, and specifically Scheme since about 2005, and though I've written
quite a few little things here and there, nothing has ever really "stuck." When I worked at Meetup,
I did manage to sneak in a tiny Clojure app into production, and was successful at providing a Leinigen interface to our aging Java codebase, but I don't believe the Clojure app survived much longer after I departed.

This is probably a common scenario for many Lisp enthusiasts, and as one you're bound to get the all too common reactions: 

> "Isn't that for AI?"

> "There's just too many parentheses for me"

> "It's just not practical and only useful for academics!"

If you're like me, I've grown accustomed to these reactions. I've grown so accustomed to these reactions that I just stopped bothering with efforts to adopt Lisp in a professional setting. But, that's actually rather silly, especially if what I've always said about Lisp, and what I've always heard claimed about Lisp has any truth to it. 

So, I began to ask myself why haven't I written anything significant? Why haven't *I* adopted Lisp, specifically [Racket][racket], for anything more than occasional curiosity? 

I know some of the actual reasons it doesn't show up at work. The inertia of a framework or language already established at a company. The existing bias against Lisp, and s-expressions. The fact that most Lisps (save for the smash hit for the JVM, Clojure) don't coexist well with code bases in other languages. These are all very good, and legitimate reasons, of course. But, these aren't reasons that should stop myself from adopting Racket more and more.

And with that, I've started to do just that. I pick up Racket whenever I have a quick task I need to automate. I'll use it as a desk calculator. And, there are many other things I'd *like* to use it for, like documentation via [Scribble][scribble]. Specifically, I'd like to have some graphviz/diagramming tools that integrate well with Scribble, but I digress.

There's a hundreds of different ways that I can bring Racket into my semi-daily life, and I've only scratched the surface.

So, a while back I signed up to give a talk at [San Diego Dev Ops][sddo]. Like always, I procrastinated a bunch on preparing slides. My typical talk preparation goes something like this: Come up with an idea, write what is essentially a speech, sprinkle some stupid jokes in there, develop slides, and then deliver a more natural, less rehearsed version of the speech.

This strategy tends to work well, except for the fact that pointing and clicking to make slides is frustrating and horribly complicated. So, with a couple of hours before I was set to give a talk, I, of course, decided to give [Slideshow][slideshow] a try. 

What I found is that while Slideshow is immensely powerful, it's not simple. It's like "[HyperCard lite][hypercard]", for experts--which is kind of ironic.

That's where `#lang slideshow/simple`, hopefully, comes in.

It's implementation is a simple reader that spits out regular slideshow presentations in a DWIM sort of way. The syntax is inspired by [sent](http://suckless.org/sent), but it's got a bunch more bells and whistles that I find myself looking for.

[slideshow/simple][ss], I believe, is excellent for [Takahashi](https://en.wikipedia.org/wiki/Takahashi_method) style presentations, but you don't have to stick to just a couple of words per slide. You get bulleted and numbered lists, code blocks, quotations, images, paragraphs in a way that doesn't depress you, and in a way that is, I assume, readable by an audience.

But, perhaps, most importantly you get the ability to take advantage of the excellent features of slideshow, like a next slide preview window, speaker notes, and printable slides.

The thesis of my talk, and I conclusion I've made recently is that while it might be extremely hard to adopt Racket (or another Lisp) for "production code," it should be relatively straightforward to grow it's usage within your organization by building tools that break down barriers to doing our jobs well. If you do a lot of data analysis, build some of your tools on top of Racket's numeric tower, and things like [plot][plot]. If you need to draw pictures, draw them with [pict][pict]. Simple GUIs can be done with `racket/gui.` And, last minute presentations can be done without hassle, using [slideshow/simple][ss], built directly on top of slideshow, itself built directly on top of the extremely flexible, and infinitely extensible, language creating programming language called [Racket][racket].



[con]: https://con.racket-lang.org/
[slideshow]: https://docs.racket-lang.org/slideshow/index.html
[ss]: https://github.com/apg/slideshow-simple
[scribble]: https://docs.racket-lang.org/scribble/index.html
[racket]: https://racket-lang.org
[sddo]: https://github.com/sddevops/presentations/blob/gh-pages/shard-your-shelf/shard-your-shelf.pdf
[hypercard]: https://en.wikipedia.org/wiki/HyperCard
[plot]: http://docs.racket-lang.org/plot/index.html
[pict]: http://docs.racket-lang.org/pict/index.html
