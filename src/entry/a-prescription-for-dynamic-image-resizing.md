% A Prescription for Dynamic Image Resizing
% servers, images
% 2012-12-18


Everyone seems to have a story for how they do image resizing for a website.
Some resize in advance, while others resize dynamically. [Phizer][1] (PHoto
resIZER), in some ways, does both.

At [Labs][2], one of our products, [Combosaurus][3] requires that we serve a
lot of images, in at least 3 different size configurations. This of course
meant that we had to make a choice[[1]][4].

Do we do resizing up front, storing each image N times, where N is the number
of size configurations, or do we do the resizing on the fly?

Our sister company, [OkCupid][5], choose the latter. It makes a lot of sense
really. Dynamically resizing images means that a designer isn't constrained,
seemingly forever, to image sizes determined before the site has any users.
And, while yes, one could go through and resize all the images to accommodate
new sizes, it's obviously going to take a lot of time and effort to resize
millions of images--so we chose to follow suit. They also have a CDN in front
of their resizer, something that we'll almost certainly do in the future, but
haven't needed to do yet.

One interesting thing to note about the problem of dynamic resizing is that
the image resizing operations are fairly cheap. In fact, in everything I've
tested in solving this problem, the IO is always the bottleneck--never the
resize. (This is perhaps counter-intuitive to most people, as I've had
numerous people virtually gasp at that thought).

But, because of this fact, it's relatively easy to write something that
performs exceptionally well even in a dynamic language like Python--so that's
what we did.

For our first implementation, we enlisted Python's [multiprocessing][6],
[BaseHTTPServer][7] and the [Python Imaging Library][8]. We knew this wouldn't
scale forever, but by pre-forking 30 processes, and creating a shared cache
between them, we were able to quickly prototype the system, and launch it to
our early alpha testers. With browser side caching, images loaded snappily and
things were looking up.

Last month the day came to replace the aging system[[2]][9]. We have a lot
more testers now than we had when we started, and the whole thing was due for
a redesign. We wanted even more snappy loading of images, and more
concurrency.

The design of the original system was simple, and the new design shares quite
a bit of the original high-level architecture. We use S3 to host all of the
full-size[[3]][10] images. Upon request, we check the full-size image cache,
and if it's in there we resize, otherwise we download from S3, stick it in the
cache and resize, before serving it back to the client[[4]][11].

To avoid some potential craziness, we pre-white-list a few different size
configurations for images. When a request is parsed, we interpret the code
that was given in the URL and respond with an appropriately sized image. With
this we get some of the benefits of pre-resizing[[5]][12], and the obvious
benefits of dynamic resizing.

And, to avoid a 404 while a file is in the process of going through the
system, we temporarily serve photos through the resizer, via the webserver it
was uploaded to--in other words, we have a single master (S3), and each
webserver is a slave.

The only thing that really hurt in the old system was BaseHTTPServer. We
blocked everytime we needed to pull an image from an external machine. With a
fixed number of processes and unknown latency, this meant that a (uncached)
full page load (with maybe 40 images) would potentially spin for a while. This
sucks.

One solution is obvious, use evented IO to download the images, and to handle
requests. Seems reasonable, but as everyone knows, doing anything even
somewhat computationally intensive[[6]][13] with evented IO is a bad idea,
right?

Well, that's where having multiple cores really comes in handy, and that's
what we did--we made use of them.

So, the rewrite of phizer does a few things differently. First, instead of
using 30 pre-forked processes, it uses 5 (customizable of course)
[Tornado][14] based processes. The first process maintains a resized-image
cache as well. When a requested image (and size) is not cached, it proxies the
request to an appropriate worker[[7]][15], which asynchronously downloads the
image, resizes it appropriately and delivers it back to the master. The worker
stores the full size download in a cache, too, so future fetches for a
larger/smaller thumbnail take even less time.

Images in cache are served in sub milliseconds. Images needing to be
downloaded fully take some time--but this is mostly S3 latency. We're seeing
200ms or more in some cases on our development machines. All in all, we're
happy with the performance thus far, and can certainly scale horizontally
(we're on one machine right now).

Anyway, we're releasing [phizer][1]. Feel free to hack on it and make it even
better. We certainly will be.

  1. It's certainly possible that we'll make a new choice later, but we're set
on this for now.

  2. The system _really_ didn't like me pointing 4 different domains at it to
take advantage of browser request parallelization.

  3. though scaled down to something more reasonable before being archived on
S3

  4. The new architecture actually caches the thumbnails as well, but I'm
getting ahead of myself

  5. Great cachability, constraints on the designer [which can easily be
broken, of course]. Dynamic resizing of course is very flexible and non-
committal.

  6. And resizing images, despite being quick, does use a decent amount of CPU
of course

  7. CRC32(image-filename) % number backend processes

   [1]: https://github.com/apgwoz/phizer

   [2]: http://www.okcupidlabs.com/#ny

   [3]: http://combosaurus.com

   [4]: #note-maybe-not-the-last

   [5]: http://www.okcupid.com

   [6]: http://docs.python.org/2/library/multiprocessing.html

   [7]: http://docs.python.org/2/library/basehttpserver.html

   [8]: http://www.pythonware.com/products/pil/

   [9]: #note-parallelization

   [10]: #note-resize

   [11]: #note-caching

   [12]: #note-benefits

   [13]: #note-event-driven-cpu

   [14]: http://tornadoweb.org

   [15]: #note-balance

