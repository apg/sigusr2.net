% A "One" Line Echo Server Using "let" in Python
% python, hacks, whyohwhy
% 2014-11-12

This morning I realized something incredibly stupid. Stupid, because
it has no real value except as a curiosity--which is to say, its
perfect for this blog, which has featured [other][withdispatch],
[things][withworld], of a [similar][withpatterns] [nature][callcc].

Anyone familiar with Python and functional programming, will lambast
Python for its measly one-line anonymous functions. What if they could
do more?  Well, they can! And, they can even introduce lexically
scoped bindings as one would get if they had just used a better
language to begin with, all in a "somewhat"[^1] nice, easy, and
clean way.

I present to you, a one-line[^2] echo server that will eventually
exhaust all of your memory, and slow to a crawl... 

    import socket
    import itertools
    
    (lambda port=9000, s=socket.socket(socket.AF_INET, socket.SOCK_STREAM):
          s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) == None and 
          s.bind(('', port)) == None and
          s.listen(5) == None and
          list(map(lambda c:
                      c[0].sendall(c[0].recv(1024)) and
                      c[0].close(),
                   (s.accept() for _ in itertools.count(1)))))()

Wait, what? Yeah. Something special, isn't it? There's a lot going
on here, so let's break it down a bit.

    lambda port=9000, s=socket.socket(socket.AF_INET, socket.SOCK_STREAM):
    
This is the best part of the trick, and the reason I threw in "let" in
the title. In lexically scoped languages, such as Scheme, it's common
to see the use of the `let` form which introduces new bindings that are
lexically bound, *i.e.*, local to the body of the `let` expression. These
are isomorphic to "block-scoped" variables one might find in other
languages.

    (let ((port 9000))
       <some body>)

In Scheme, the above `let` defines `port` only within `<some
body>`. That construct is isomorphic to the following Scheme (and in
fact many implementations just macro expand `let` into this):

    ((lambda (port) <some body>) 9000)

We utilize default arguments to assign names to values in the Python
version, and exploit the fact that a `lambda` by itself doesn't bind
to a name as `def` does[^3], and doesn't commonly appear in the global
scope.  What this means is that our "bindings" are evaluated when the
anonymous function is created, which means it's safe to use within a
loop, or some other nested scope. 

This `lambda` perversion, therefore, is isomorphic to the following Python:

    (lambda port: <utilize port somehow>)(9000)
    
Now, that we've established this baseline, let's move on.
    
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) == None and 
    s.bind(('', port)) == None and
    s.listen(5) == None and
    
Here, we exploit the fact that the `and` operator evaluates all of its
operands provided they return truthy values. Since these functions return
`None`, we just check to see if they returned `None` and continue to the
next statement. This is of course roughly isomorphic to the following:

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('', port))
    s.listen(5)

Simple enough.

Continuing on, we get to the trickiest part of it all:

    list(map(lambda c:
                 c[0].sendall(c[0].recv(1024)) and
                 c[0].close(),
             (s.accept() for _ in itertools.count(1))))

Ignoring the outer `list` for now, we see a simple call to `map`,
which maps the `lambda` over the values produced by the generator
expression. The generator expression produces an infinite list
connected sockets, but since `s.accept()` blocks until a client is
connected, this implements a typical listen-accept loop. The roughly
isomorphic, plain ole Python looks like so:

    while True:
        c = s.accept()
        c[0].sendall(c[0].recv(1024))
        c[0].close()

Why roughly? Because `map` is lazily evaluated, and produces a *new*
generator. We need a forcing function in order for each connected
socket to be applied to the `lambda` we have passed to the `map`
call. Enter `list` which does that beautifully, though *ensures* that
we'll eventually completely exhaust memory if we accept a lot of
connections. That list being generated will store the value `None` for
every client that exists.

The only thing left to do is invoke our echo server, by wrapping `()` around
the `lambda` and tacking on `()` to call it.


[^1]: As in "sort of," if you ignore the extra `()`, the `and` connector, and all the other little gotchas...

[^2]: ... and 2 imports

[^3]: There's nothing about this trick that *won't* work with `def` by the way, and the following decorator eliminates the need for immediate calls which we must do in the pure `lambda` method: `let = lambda func: func() and func`

[withdispatch]: http://sigusr2.net/dispatching-with-with.html

[withpatterns]: http://sigusr2.net/pattern-matching-with-with.html

[withworld]: http://sigusr2.net/python-worlds.html

[callcc]: http://sigusr2.net/call-cc-for-python.html

