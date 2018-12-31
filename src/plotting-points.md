# Plotting Points

Yesterday, I captured some data from `pidstat` during a failure simulation. I had hypothesized that the components memory usage would be pretty stable regardless of failure, but wanted to confirm it just to be sure. At the end of the simulation, I had a bunch of `pidstat` records that looked like this (with the params I gave it):

    06:53:23 PM       PID    %usr %system  %guest    %CPU   CPU  Command
    06:53:28 PM     19437   76.05    7.78    0.00   83.83     1  lumbermill
    
    06:53:23 PM       PID  minflt/s  majflt/s     VSZ    RSS   %MEM  Command
    06:53:28 PM     19437     31.74      0.00 1669088 1142476   7.25  lumbermill
    
    06:53:23 PM       PID   kB_rd/s   kB_wr/s kB_ccwr/s  Command
    06:53:28 PM     19437      0.00      0.00      0.00  lumbermill

I wanted only column 8 of the memory related record, so I did something pretty ugly:

    $ grep -C 1 '%MEM' pidstat.testing | grep -v '%MEM' \  
      | grep -v -E "^$" | grep -E -v "^--" | awk '{print $8}'

This produced the right numbers in a single column, but there wasn't a way for me to easily *see* them. I could have prepped a simple `gnuplot` script of course, but that was more trouble than I was looking for, even if it would have been most trivial to do:

    $ grep -C 1 '%MEM' pidstat.testing | grep -v '%MEM' \
      | grep -v -E "^$" | grep -E -v "^--" \
      | awk 'BEGIN { i = 0} {print i"\t"$8; i += 1}' > /tmp/foo.dat
    
    $ gnuplot
    gnuplot> set terminal x11
    Terminal type set to 'x11'
    Options are ' nopersist'
    gnuplot> plot '/tmp/foo.dat' using 1:2

So, I did actually do that, but I wanted to investigate other ways to go about this:

I had an obsession with `[emacs calculator][calc]` the other week, so I switched buffers to `\*shell\*` ran the above pipeline, and got a single column of output. Selected it all, `C-x \* R` to load it into `\*Calculator\*`. Then did `V a -1`, to flatten the data into a 1d vector. Then, `g f` for (graph fast). I quickly realized that I needed a second vector for the `x` coordinates, so I ran the 2 column variant, and inserted the `x`s into the calculator, and had a graph to display. Since `emacs` just uses `gnuplot` for this, the graph was no different, but the method didn't involve actual `gnuplot` interaction to use it.

Surely, I thought, there must be something better. Well, this was *timeseries* data. I figured, maybe the venerable `rrdtool` can easily do this. Oh, how I was wrong about *easy*.

I created an rrd database, with something along the lines of:

    $ rrdtool create /tmp/pidstat.rrd --step 5 \
       --start 1406802180 DS:mem:GAUGE:5:0:100 \
       DS:cpu:GAUGE:5:0:100 RRA:AVERAGE:0.5:1:2000

And then, I needed TS:data pairs. Back I went to reextract that data out. This time with and *even* uglier pipeline and some more advanced `awk`ing:

    $ grep -C 1 '%MEM' pidstat.testing | grep -v '%MEM' \
      | grep -v -E "^$" | grep -E -v "^--" \ 
      | awk '{while ( ("date +%s --date=\"07/31/2013 "$1"PM""\"" \
            | getline result) > 0 ) { print result":"$8 } }'

That got me epoch time from the times in the file, and when I tacked on:

    | xargs rrdupdate /tmp/pidstat.rrd -t mem

It all was loaded into the database. Surely, I can generate a nice looking plot now?

No! Why? Because I need to invoke `rrdtool graph` with some crazy array of options to spit out anything, and I didn't even bother!

I could have writtten a quick HTML page with a [flot][flot] chart faster. Why isn't there a simple [DWIM][dwim] tool that allows me to tack on `| linegraph`?

_- 2014/07/31_



[calc]: https://www.gnu.org/software/emacs/manual/html_mono/calc.html

[flot]: http://www.flotcharts.org/

[dwim]: https://en.wikipedia.org/wiki/DWIM
