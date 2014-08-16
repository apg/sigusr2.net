% Maps are Broken, for Some Definition of Broken
% algorithms, java, python, rant
% 2010-06-22

Map datatypes are extremely useful for a variety of tasks in programming. But,
they are often painful to use; take for example the following task.

In Java, I have a [HashMap][1] and I wish to get a random key. Well,
[AbstractMap][2] doesn't define a way to get a random key, but it does provide
a way to get a [Set][3] of keys. Does Set have a way to get a random element?
No, but you can create an Array from a Set with the `toArray()` method on Set.

We end up with the following:


    public String randomKey() {

        // Assuming: map = HashMap<String, String>;

        Set<String> set = map.keySet();

        Object[] strings = set.toArray();

        Random random = new Random();

        if (strings.length > 0) {

            return (String)strings[random.nextInt(strings.length)];

        }

        return null;

    }


Now, this isn't necessarily bad, but we have to create a new array, and a new
set each time we want a random key. We can of course be smarter about this by
caching the array and/or set, but then we run into synchronization issues. We
also get screwed when we attempt to implement the `popRandom()` operation,
which could be implemented like so:


    public String popRandom() {

        String key = randomKey();

        if (key != null) {

            String value = map.get(key);

            map.remove(key);

            return value;

        }

        return null; // or more appropriately, throw an exception

    }


So, we're doing all this extra copying, allocating and deleting, when all we
really need is an iterator, to solve this generically in `O(n)` time.


    public String randomKey() {

        // randomKey method in O(n) using imaginary iterator() on AbstractMap

        int size = map.size();

        if (size > 0) {

            int index = new Random().randInt(size);

            Iterator<String> keys = map.iterator();

            while (keys.hasNext()) {

               if (index-- == 0) {

                   return keys.next();

               }

               keys.next();

            }

        }

        return null;

    }


This sort of thing isn't necessarily true for dynamic languages like Python
which normally have ways to iterate over keys in a map, dictionary or set.
They still don't have a way to get a random element from either out of the box
without resulting to the `O(n)` iteration method, or converting to a list and
using a random index approach.


    >>> import random

    >>> random.choice(set([1, 2, 3]))

    Traceback (most recent call last):

      File "<stdin>", line 1, in <module>

      File "/System/Library/Frameworks/Python.framework/Versions/2.5/lib/pytho
n2.5/random.py", line 248, in choice

        return seq[int(self.random() * len(seq))]  # raises IndexError if seq
is empty

    TypeError: 'set' object is unindexable



    >>> random.choice({'1': 'world', '2': 'galaxy', '3': 'universe'})

    Traceback (most recent call last):

      File "<stdin>", line 1, in <module>

      File "/System/Library/Frameworks/Python.framework/Versions/2.5/lib/pytho
n2.5/random.py", line 248, in choice

        return seq[int(self.random() * len(seq))]  # raises IndexError if seq
is empty

    KeyError: 2


And of course that makes sense given how `random.choice` is implemented, since
there's not necessarily an order for the elements of a set or dictionary, so
you can't expect to subscript them. However they do provide an order when
iterating over them and traversing the structure they exist in, so you could
certainly use the same `O(n)` approach from above.

If there's some other less obvious way to do this in Java using a
[EnterpriseFactoryObserverFactoryFactoryCreator][4], please leave a comment.

**Update: I overlooked something important, which was pointed out by
[gojomo][5] on Hacker News. Set, which is returned from `keySet()` on HashMap,
has an iterator. Thus:**


    public String randomKey() {

        int index = random.nextInt(map.size());

        for (String key: map.keySet()) {

            if (index-- == 0) {

                return key;

            }

        }

        return null;

    }


   [1]: http://java.sun.com/j2se/1.5.0/docs/api/java/util/HashMap.html

   [2]: http://java.sun.com/j2se/1.5.0/docs/api/java/util/AbstractMap.html

   [3]: http://java.sun.com/j2se/1.5.0/docs/api/java/util/Set.html

   [4]: http://en.wikipedia.org/w/index.php?title=Dependency_injection&oldid=2
60831402#A_code_illustration_using_Java

   [5]: http://news.ycombinator.com/item?id=1452619

