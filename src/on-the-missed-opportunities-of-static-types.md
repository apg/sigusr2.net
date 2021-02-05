# On the Missed Opportunities of Static Types

Recently, a colleague spent a bunch of time working on a bug. When
attempting to perform some action, the system would provide feedback
that everything was going as planned, and then "go dark," seemingly
out of no where. It's important to realize that this is a complex
distributed system, and that there is a network connection between the
"part that worked" and "the part that went dark." But, there wasn't a
change to the connection logic, and it worked before, so that
"couldn't possibly be it." _(Yes. I already told you where this is going&mdash;Ed)_

Suppose you're writing a Go program and you want to perform a different
action based on the value of another value. You might do this:

```
func Greeter(lang, name string) {
	switch lang {
	case "english":
		fmt.Printf("Hello, %s!\n", name)
	case "spanish":
		fmt.Printf("Hola, %s!\n", name)
	default:
		fmt.Printf("excuse me?\n")
	}
}
```

Based on `lang`, the function will (naively) print to the screen a
greeting, for `name`, in the specified language. 

```
Greeter("spanish", "Andrew")  // outputs: Hola, Andrew!
Greeter("english", "Andrew") // outputs: Hello, Andrew!
```

_(Now, you can argue with me about whether strings are appropriate here, when an `iota` would do, but that would be silly, as that's not the point. In the bug, strings were used, so that's what we're going to use here.&mdash;Ed)_

In the case of `Greeter`, there's no dependency between `lang`
and `name`. `name` is perfectly valid for each `lang`, unless you're
fancy and do a translation of "Andrew" to "Andres". Again, not the point.

In a more complicated setting there _is_ a dependency between the two
values. Let's take a look at a Go API many of us are familiar with
because it's front and center on the package documentation for
[net](https://golang.org/pkg/net/).

```
conn, err := net.Dial("tcp", "golang.org:80")
if err != nil {
	// handle error
}
```

This demonstrates something beautiful; flexibility! If I want to
`Dial` a `unix` socket, the right parameters are `"unix",
"/path/to/socket"`, and `hostname:port` is meaningless. This works
because `Dial` implements validation logic to ensure that the
address provided makes sense for the protocol, and returns an error if
not. This is helpful in reducing the amount of code required to
dynamically `Dial` a socket specified from external configuration, but
it pushes error checking to _runtime_ which is an aphorism for "you're
eventually going to be woken up due to this bug."

If we redesign the API, we can get the compiler to ensure we're
not doing something silly; but we need to make a few concessions.

`DialUnix(addr UnixSocket)` could ensure that `addr` is a path to a
unix socket. It may _not_ delegate everything to the compiler, but
obvious errors could be detected. You wouldn't be able to, say,
`DialUnix(HostPort("golang.org", 80))` since `HostPort` is not of type
`UnixSocket`. This particular example works better if file paths
aren't represented as strings, a [sore point of
sorts](https://fasterthanli.me/articles/i-want-off-mr-golangs-wild-ride). Then,
`UnixSocket` can be an effective alias to `Path`, for instance.

But what is the cost here? Well, in the case of `Dial`, the interface
that the `net` package exposes becomes much larger, creating more
cognitive overhead for programmers, and `net` developers. Go `net` developers
have to create a new function, and type for every new type of supported
socket. Programmerss must adopt those new functions, increase the things
they know about, and no one is truly happy with the experience.

Go doesn't provide an obvious mechanism to make this more generic, and
while the generics proposal _may make solving this possible_ that's not
going to help us for quite some time.

So what is a programmer to do?

To recap the problem here: when using strings as arguments to simplify
an interface, the compiler cannot create assurances that the two
strings relate to each other in any meaningful way, which means that
all validation must be done at runtime, which almost certainly means
bugs are gonna slip into production, despite the fact that we already
pay for static types in the language, and the compiler.

The other possibility, which keeps a singular function, is to have
`Dial` and an interface instead, say, `ProtoAddr`:

```
// SIDENOTE: This, effectively, already exists as net.Addr ... :tableflip:
type ProtoAddr interface {
    Protocol() string // wait, wuh????
    Addr() string
}

func Dial(pa ProtoAddr) (net.Conn, err) {
    return originalDial(pa.Protocol(), pa.Addr())
}
```

We can then create helper functions and types that _constrain_ our
inputs and allow for more assurances that the caller isn't doing
something silly, like providing a filesystem path instead of an IP
address. Want to `Dial` an IP address?
`Dial(TCPIPv4Addr{net.IPv4{127,0,0,1}, 8000})`. 

Similar types can just as easily be constructed for `hostname:port` addresses,
Unix Sockets, etc. The important aspect here is that the caller's intentions
are stated, in a way that the compiler can catch, and that the library
author can quickly assert validity of.

```
type TCPIPv4Addr struct {
    IP IPv4
    Port uint16
}

func (t TCPIPv4Addr) Protocol() string { return "tcp" }
func (t TCPIPV4Addr Addr() string { return fmt.Sprint("%s:%d", t.IP.String(), t.Port) }

func NewTCPIPv4Addr(ip IPv4, port uint16) TCPIPv4Addr ...

type UnixAddr struct {
    Socket filesystem.Path // made up, because, in Go, strings are totally fine for everything, including file paths.
}

func (u UnixAddr) Protocol() string { return "unix" }
func (u UnixAddr) Addr() string { return u.Socket.String() }

```

In the above code snippets, you'll notice something odd--that
`string`'s are ultimately passed into
`originalDial` as before. This is meant to address and illustrate how you might
take `Dial` and evolve it, just as you might evolve instances in your
own codebase. This whole idea, of course, fundamentally is not backward compatible.

_"But wait a minute, Andrew! What the hell? All you've done is add more boilerplate for me to type out, and I already have enough of that with Go's error checking. What's this got to do with me? Why is this any better?"_, you say.

The primary job of a program is to take input and produce some output.
This is vague; yes. The primary job of a programmer is to ensure that
the input given to a program makes sense in order to produce the
desired output.

In other words, the fact that we build APIs that take arbitrary
strings, like `Dial`, doesn't give us a pass from validating that the
input makes sense, and indeed `Dial` heavily validates input. But, why
not leverage the type system to check as much of your work as
possible at compile time?

Go's type system doesn't provide us with enough expressive power to solve
this simply, unfortunately. And so, as written, it's not possible to 
prevent a silly programmer from calling the new `Dial` with a `LOLAddr`
defined like so:


```
type LOLAddr struct { 
   What string
   Lol string
}

func (l LOLAddr) Protocol() string { return l.What }
func (l LOLAddr) Addr() string { return l.Lol }

Dial(LOLAddr{"unix", "localhost:8383"})
```

Because of this, you might think that the implementation I've described
is totally useless; I don't think so. Go _does_ provide a mechanism
that you can use which gives you all of the good properties of what I
described without the bad. 

```
type ClosedProtoAddr interface {
    Protocol() string
    Addr() string
    closed() struct{}
}
```

This interface is impossible to implement outside of the package it's
defined in, because `closed() struct{}` is not exported. Using this,
we can _constrain_ the input to known good values, and exhaustively
create all of the supported `Dial` compatible `ProtoAddr`s in the
package. `Dial` just has to handle the possibility that `nil` was
passed in, and validate that nothing sneaky is hiding in one of those
strings, something that is done right now in the old interface, too.

---

So about that bug? Well, it was especially sinister as it was
contained in a subsystem that implements a proxy with a virtual
network address overlay. The proxy's connection is _always_ supposed
to be alive and so it reconnects in a loop, without providing much in
the way of visibility in regards to its status. There's no timeout
errors. There's no error logs. There's an assumption made that if the
link goes down, a retry will eventually succeed. 

There are, also, two virtual address types. The two types of addresses are there to allow you to say "connect to this
specific node" or "connect to _any_ of the nodes in this
cluster". These are reasonable things. The addresses _look_
different, but are each represented by strings. The address type,
itself, is represented by strings.  There is no
`DialRandomNodeInCluster(clusterId)`. There is no
`DialHost(hostId)`. There is an analog to `Dial(type, addr)`,
and then a _lot_ of screaming when you realize that the feature you
are working on required you to dial a specific host&mdash;_not a random one_&mdash;and 
you didn't change the first argument that provides the context for how to interpret the address.

As developers, it is important to create clear guidance on how not to
be frustrated when attempting to use our libraries. This, too often,
is delegated to poorly written documentation, even when the best
documentation, the code, can be used to point out our misuse, by 
our friend the type checker.


_- published: 2021/02/05. written: 2020/11/15_
