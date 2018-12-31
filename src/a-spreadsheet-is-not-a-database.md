# A Spreadsheet is Not a Database

[Thinking of Databases as an Excel File][db-excel] is too simplistic a metaphor and
misses the main point that differentiates the two.

But, Justin isn't completely wrong. Sure, a spreadsheet can represent rows and
columns of data (_i.e._ tabular). And, some spreadsheet packages actually go so
far as to allow you to have multiple "sheets", which, with a bit of stretching
and (perhaps) misuse can even be related to another table for use in a custom
input (e.g. a drop down list) or who knows what. There's also this concept of
[pivot tables][pivot-tables]--the equivalent in a relational database being an aggregate query.<sup><a id="one-return" href="#one">1</a></sup>

So, why is his metaphor too simplistic? It relates<sup><a id="two-return" href="#two">2</a></sup> to the fact that a
spreadsheet's data is "live." It's a living thing, reacting to changes in
input, changes in formulas, newly added formulas, etc.. The difference between
a relational database and a spreadsheet is simple. The spreadsheet _is_ a
live, running program, and the tables in a relational database are nothing
more than rotting bits.

_- 2013/09/24_

### Footnotes

1. <a id="one"/>Hopefully you don't notice, but it's possible in some RDBMSes to create what are known as views (or materialized views, or both). With these, you could essentially define a query (or a procedure) that computed the same data as could be found in your spreadsheet, in a sort of "live" fashion. Updates either occur on some timer, or when you request it via a query.<a href="#one-return">#</a>
2. <a id="two"/>No pun intended<a href="#two-return">#</a>



[db-excel]: http://justin.abrah.ms/mentoring/databases-as-an-excel-file.html
[pivot-tables]: http://en.wikipedia.org/wiki/Pivot_table
