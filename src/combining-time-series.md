# Combining Similar Time Series

This week was [Monitorama](https://monitorama.com) 2019, in Portland. I was there. Many in the
monitoring community were there. A great time was had by many. (_note:_ I've never really considered myself
part of the monitoring community, but perhaps should make more of an effort?)

[Evan Chan](https://velvia.github.io/) gave a talk entitled: *"Rich
Histograms at Scale: A New Hope."* ([slides](https://www.slideshare.net/EvanChan2/histograms-at-scale-monitorama-2019)) In it, he made everyone aware of
how much error exists if you do histograms the way that's easy in
systems like Prometheus--"a couple of linear buckets"--and showed that
to get even close to a 10% error rate for a span of values from 1000 -
6e10, you need over 600 *exponential* buckets.

This presents a problem.

While it's not that big of a problem to query 10 time series to
reconstruct latency estimates with histograms, it _is_ a problem to make 600 queries
to retrieve the buckets to reconstruct latency estimates with histograms.

Not to worry, though! Evan suggested that a richer histogram model,
which stored all of the buckets in a single, rich time series, would provide
better scalability here without sacrificing accuracy. And, using
[delta encoding](https://en.wikipedia.org/wiki/Delta_encoding), 188
buckets would be quite cheap to store (on the order of 1.8 bytes per
bucket).

At the end of the talk I messaged one of my colleagues who was in the
trenches with me when we rebuilt the system that hosts [Heroku Metrics](https://blog.heroku.com/heroku-metrics-there-and-back-again)
\*(post is years out of date at this point). You see, co-locating
similar time series data was exactly the design we used with InfluxDB
(v0.0.76 or something) before 2015, and exactly the design we knew
we'd keep in the new system. Because we knew our graphs would be
composed of, say, 4 memory related metrics, we put them together in
the same way you could put 188 histogram buckets together.

Non-Relational databases always talk about how you've got to design
your schemas for how you want to query it. Relational databases say
similar things about indexing strategies. There doesn't seem to be an
equivalent mantra in the time-series world--perhaps because we're too
busy worrying that if we can't _write data fast enough_, reads don't
matter anyway?

Not sure! But, co-locating metrics that
will be read together, or written together is _very_ worth it, in much
the same way that denormalizing relational data can be _very_ worth
it, or designing NoSQL table spaces with reads in mind is simply a requirement.

In Heroku's case, our metric co-location strategy uses the following
general schema (stored compressed in Cassandra):

```
message Observation {
  string source = 1;              // e.g. collection-type://identifier/labels
  int64  timestamp = 2;           // unix timestamp in seconds
  Measurements measurements = 3;
  MetricType type = 4; // histogram, gauge, counter -- aggregation rules.
}

// HDRHistogram fields, aggregatable using the Merge function after reifying
// back into HDRHistogram objects
message HistogramSnapshot {
  int64 lowest_value = 1;
  int64 highest_value = 2;
  int32 significant_figures = 3;
  repeated int64 counts = 4;
}

message Measurements {
  repeated string names = 1;  // e.g. ["memory_cache", "memory_swap", ...]
  repeated double values = 2; // e.g. [0.15, 0.1, ...]
  HistogramSnapshot histogram_snapshot = 3;
}
```

This currently supports value cardinalities from 2 to 500, depending
on the use case (the high cardinality stuff is typical for data store
metrics), with an average of about 7.

A natural extension of this would be to turn `Measurements.values`
into a matrix and do delta encoding. That would provide options for
higher resolution data, or wider data across a single tag value (say,
dyno in Heroku's case).

So, while Evan put forward that a richer model for histograms is
needed for better accuracy, I'm here to suggest that, while maybe not
trivial, we shouldn't be scared that his ideas can't or won't
scale. They definitely can. 

_- 2019/06/07_
