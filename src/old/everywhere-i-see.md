% Everywhere I see...
% https, go
% 2015-02-03

[HTTPS Everywhere][he] is by far one of the most important browser
plugins, perhaps ever. It attempts to ensure that a user is
communicating securely with a remote site. It does this by comparing
the URL being requested to a database of *known* HTTPS (read:
"secure") endpoints, and rewrites the requested URL if it knows of
a suitable replacement. (It, of course, falls back to the plain ole,
HTTP endpoint as necessary)

In addition, and optionally, it participates in the
[HTTPS Observatory][ho], a project that collects information about SSL/TLS
certificates in use in the real world, by real web sites, and studies
them, alerting users of potential problems in real-time, and capturing
and analyzing trends to help ensure our future communications remain
secure.

I recently built a library, in Go, for the [rulesets][rs] that the
HTTPS Everywhere plugin utilizes to define the HTTP to HTTPS
mapping. The obvious next thing to do was to wrap it up in a little
proxy, that runs on localhost, such that one can safely utilize the
web when a HTTPS Everywhere supporting browser *isn't* in use.

Checkout [heproxy][heproxy] on Github. There are likely a million
improvements, and next steps (like incorporating some
[privoxy][privoxy] type things, or maybe just porting this *into*
[privoxy][privoxy] to begin with?) to be made, but have at it, at your
own risk of course.

[heproxy]: https://github.com/apg/heproxy
[he]: https://www.eff.org/https-everywhere
[ho]: https://www.eff.org/observatory
[rs]: https://www.eff.org/https-everywhere/rulesets
[privoxy]: http://www.privoxy.org/
