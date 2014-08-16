% Anagrams Predicate
% algorithms, python
% 2009-01-08

Recently, I spent some time thinking about a simple problem. How do you test
if two strings are [anagrams][1] of each other?

There are many ways, but the naive solution is to simply sort both strings,
character-wise, and then compare the results. In [Python][2], you might do
that like so (We'll leave out the fact that anagrams are actually real words
and phrases. We also work in a case-sensitive manor [e.g. "JimMorrison" and
"MrMojoRisin" isn't truthy, though a simple `s.lower()` goes a long way.]):

`def isAnagram(str1, str2): if len(str1) != len(str2): return False return
sorted(str1) == sorted(str2) `

Here, I'm using `sorted`, a Python `__builtin__`, that takes an iterable and
produces a sorted list of that iterable. I'm then taking advantage of the fact
that Python lists can be compared element-wise with the `==` operator. Doing
this produces a function that will work on any string, and it's simple to see
why. Sort the string "parental" and you get "aaelnprt." Sort the string
"paternal" and you still get "aaelnprt." Obviously "aaelnprt" and "aaelnprt"
are equivalent. This of course returns `False` for the strings "snowman" and
"iceman" since they don't compare sorted equally (or non-sorted for that
matter).

However, this solution isn't the most efficient use of resources. For one,
most sorting algorithms are only _O(n log n)_, which means in the best case
`isAnagram` is too. It also needs to allocate two lists to store the results
returned by `sorted`.

There is of course a way to do better. You just have to think about the
problem for a little longer than a minute:

`def isAnagramN(str1, str2): if len(str1) != len(str2): return False counts =
defaultdict(lambda: [0, 0]) for c1, c2 in izip(str1, str2): counts[c1][0] += 1
counts[c2][1] += 1 for k, v in counts.iteritems(): if v[0] != v[1]: return
False return True `

This code does not allocate proportionally to the size of the strings, but
instead on the diversity of the strings. In other words,
`isAnagramN("aaaaaaa", "bbbbbbb")` allocates 1 defaultdict, and 2 lists of
size 2. Why? Because, the algorithm simply counts up how many times each
letter occurs in each string. Of course Python also has to allocate the
generators to use for `izip` and `counts.iteritems()`, but that isn't
significant. The big win here of course is that given strings of _any_ length,
the algorithm uses only as much space as the diversity of the contents
contained in the strings!

As if that wasn't a win enough, this algorithm runs in _O(n)_ on the length of
the strings!

But, does it actually make a difference? The answer of course is yes. For
strings of significant length, `isAnagramN` runs almost 2x as fast as
`isAnagram`.

The proof is in the bacon, so let's take a look at some numbers. Using
Python's `timeit` module, I tested strings of length 1 through 100,001,
incrementing by 10,000 (I've tested other lengths as well, and reach a similar
conclusion). At each length, the test was repeated 50 times. The results are
below:

Length Time isAnagram Time isAnagramN

1

0.00043511390686

0.000530958175659

10001

0.657309055328

0.38395690918

20001

1.25437283516

0.793761968613

30001

1.91431283951

1.15374517441

40001

2.55181908607

1.53560996056

50001

3.14615797997

2.07976388931

60001

3.76745486259

2.30041193962

70001

4.48913788795

2.81828999519

80001

5.155148983

3.09482097626

90001

5.67185592651

3.477850914

100001

6.33614587784

3.94285678864

Download the code: [anagram.py][3]

   [1]: http://en.wikipedia.org/wiki/Anagram

   [2]: http://www.python.org

   [3]: http://files.sigusr2.net/anagram.py

