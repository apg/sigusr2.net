% Either continue, or error
% golang
% 2016-07-01

In functional languages such as Haskell, or Scala, there's a common
pattern, a monad even, called Either, which has two variants--left, or
right.

The left variant is commonly used to propagate an error value, while
the right variant represents success. Either's associated functions
then sequentially apply one or many operations and track the return
value. If any of the operations return left, no other operations will
be run. If right is returned, well, the computation will keep right on
trucking.

In Go, this pattern is typically expressed via a stack of if
statements:

```
if err := foo1(); err != nil {
  return "", err
}
if err := foo2; err != nil {
  return "", err
}
```

since [errors are just values][errors-values], one might be tempted to
create a type containing an error, and silently fail to apply an
operation, raising the error only at the end.

This is fine, and arguably idiotmatic, since the idea was published on
the [Go blog][errors-values]. Why not generalize it a bit further,
such that the abstraction need not constantly be reinvented? Why not
create something "Either like" that simply attempts to apply an error
producing closure and returns the first error generated, while
ignoring subsequent operations? Well, I did, and it works beautifully.

I'm not entirely happy with the result, but it's better than nothing.

```
package errutils

// Pipeline sequences a set of actions to be performed via calls to
// `Maybe` such that any previous error prevents new actions from being
// performed.
//
// This is, conceptually, just a go-ification of the Maybe monad.
type Pipeline struct {
	err error
}

// Maybe performs `f` if no previous call to a Maybe'd action resulted
// in an error
func (c Pipeline) Maybe(f func() error) {
	if c.err == nil {
		c.err = f()
	}
}

// Do performs `f` regardless of the current error value, and replaces
// the saved value with the new result.
func (c Pipeline) Do(f func() error) {
	c.err = f()
}

// Error returns the first error encountered in the Error chain.
func (c Pipeline) Error() error {
	return c.err
}
```

[errors-values]: https://blog.golang.org/errors-are-values
