% Remote Objects
% python, rest, api, plug
% 2009-11-20


Back in May, I was tasked with migrating a few years worth of [Roundup][1]
data to [activeCollab][2]--a more featured project management and
collaboration tool.

The move was justified, as Roundup no longer matched the way our organization
wished to conduct business. It was a good move, but as with any migration
there are bound to be some hiccups along the way.

Right away I hit one. The data models for these 2 very different pieces of
software are insanely different (who knew?). Roundup makes _use of_ a [SQL][3]
database, but not in a traditional way. And, activeCollab combines almost all
of its entities into one table. This of course makes querying with SQL
incredibly difficult, and a SQL to SQL translation close to impossible.

Luckily, in moving to activeCollab, we inherited a somewhat [RESTful][4], API
that allows for the modification and creation of entities. As a result, it
became apparent that the logical way to migrate this was to pull the data out
using the Roundup's libraries and make the appropriate API calls with the
translated data.

To make it easy on myself, I developed a quick [ORM][5]-like interface to the
APIs. I had the idea that if I could make it work for activeCollab, it must be
generalizable enough to work for other services and APIs as well, which I
could do later.

And, I was right. It could be more generalized, and it could be useful. In
fact, the idea was so useful that 3,000 miles away in the San Francisco office
someone else was already secretly working on the same idea!

### Enter % Remote Objects

The result of the secretive effort, that I became aware of, is called [%
Remote Objects][6], which identifies itself as An object RESTational model.
[Six Apart][7] has graciously released it as part of the requirements to run
[TypePad Motion][8], which is also available, under a [BSD License][9], on
[github][10].

But, just because its main intentions were for being used against the [TypePad
APIs][11], doesn't mean that it _only_ works with them.

In fact, the developers had the foresight to think that this would be useful
in other cases too, just as I did, as can be seen in the [examples][12] that
come with the package.

Needless to say, I abandoned my implementation of this idea and will adopt %
Remote Objects in the future. Have a look. Hopefully it is useful in your
toolbox too.

   [1]: http://roundup.sourceforge.net/

   [2]: http://www.activecollab.com/

   [3]: http://en.wikipedia.org/wiki/SQL

   [4]: http://en.wikipedia.org/wiki/Representational_State_Transfer

   [5]: http://en.wikipedia.org/wiki/Object_relational_mapper

   [6]: http://github.com/sixapart/remoteobjects/

   [7]: http://sixapart.com/

   [8]: http://www.typepad.com/go/motion

   [9]: http://en.wikipedia.org/wiki/BSD_License

   [10]: http://github.com/sixapart/typepad-motion

   [11]: http://developer.typepad.com/api/rest.html

   [12]: http://github.com/sixapart/remoteobjects/tree/master/examples

