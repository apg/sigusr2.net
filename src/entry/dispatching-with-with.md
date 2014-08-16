% Dispatching With "with"
% python, with, web, ideas
% 2009-03-04



Since [Python][1] 2.5, we've had access to a new contstruct called the
[`with`][2]-statement. Using them is as simple as implementing the [context
manager protocol][3] for classes.

The `with`-statement makes it possible to factor our `try`/`finally`
statements that are commonly used to clean up resources created temporarily.
Suppose for example, you have a multi-threaded application that makes use of
mutexes to guard shared information. As a general rule, once you lock and
perform the update on the shared variable, you need to unlock. Failure to
unlock can lead to horrible problems, such as starvation or some other
inconsistent state.

And, what happens if an exception occurs before you unlock? Quite frankly,
you're out of luck unless you use `try`/`finally` to clean up regardless of
what happens. It might look something like this:

`lock.lock() try: # perform some action finally: lock.unlock() `

This ensures that `lock` is unlocked after the action is performed.

Now, using the `with`-statement we get something more like:

`with lock: # perform some action `

The beauty is that we don't have to worry about remembering to call `unlock`,
or wrap it up in the `try`/`finally` block. It does it for us.

But, we're not worried about locks here. We're interested in using `with` for
another purpose, dispatching based on a requested url.

### URL Dispatching

In many web frameworks these days for Python, such as [web.py][4], resources
are dispatched to based on a regular expression that gets matched against the
REQUEST_URI environment variable. This is normally pretty powerful, but in the
case of web.py, the way this is specified is often a bit awkward. Take for
instance:

`import web urls = ( '/([a-zA-Z]*)', 'Hello' ) app = web.application(urls,
globals()) class Hello: def GET(self, name): if not name: name = 'world'
return "Hello,", name def POST(self, name): # name can still be in the url i =
web.input(name='world') return "You posted your name! Hello,", i.name if
__name__ == '__main__': app.run() `

which is a whole web application written in web.py. The awkwardness comes from
the fact that the regular expression used for dispatch, is in no way connected
to the resource itself. It'd be nice to use decorators for this so you could
get something like:

`@web.expose('/([a-zA-Z]*)') class Hello: ... `

but Python didn't get class decorators til Python 3.0, which many people
aren't using yet, the author included. And what's the logic behind `def
GET(**self**, ...)`? It's required in Python, but is just extraneous when
defining a resource for web.py.

What if we implemented the context manager protocol in the object returned by
`web.application()` above that looked like this (or something similiar):

`import inspect --snip-- def __enter__(self): return self def __exit__(self,
*args): frame = inspect.currentframe() get = frame.f_back.f_locals.get('get',
None) post = frame.f_back.f_locals.get('post', None) resource = {} if get:
resource['get'] = get if post: resource['post'] = post if not (post or get):
raise ValueError("with must have a get or post function")
self._resources.append((re.compile(self._last_url), resource)) if get: del
frame.f_back.f_locals['get'] if post: del frame.f_back.f_locals['post'] def
expose(self, url): self._last_url = url return self ... `

Then, the simple hello application instead looks like:

`import web app = web.application() with app.expose('/([a-zA-Z]*)'): def
get(name): if not name: name = 'world' return "Hello,", name def post(name): i
= web.input(name='world') return "You posted your name! Hello,", i.name if
__name__ == '__main__': app.run() `

It's declarative, simple, and eliminates a lot of noise, but we no longer get
the packaging, or the ability to easily seperate our resources into multiple
files. But, for one file web apps, maybe it's useful.

   [1]: http://www.python.org

   [2]: http://www.python.org/dev/peps/pep-0343/

   [3]: http://www.python.org/doc/2.5.2/lib/typecontextmanager.html

   [4]: http://webpy.org

