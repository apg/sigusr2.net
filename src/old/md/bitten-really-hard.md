% Bitten... Really Hard
% bugs, python, http
% 2009-09-09


Last Friday I was bitten really hard by something I would not have, in my
wildest dreams think would bite me, Python's [`urllib2][1]`.

It is the ever so common case that you always hear, It works fine here, it
must be your fault. And, boy did I come up with some interesting theories as
to why it was not the fault of my code.

Basically, I'm POSTing some data to a resource that is protected by [HTTP
Basic Authentication][2], and then reading the response for further
processing. It is a basic task, and one that I've done many times in the past.

In fact, I've even used `urllib2` to do this before using the following
method:



    headers = {'User-Agent': useragent}

    req = urllib2.Request(url, data, headers)


    passmgr = urllib2.HTTPPasswordMgrWithDefaultRealm()

    passmgr.add_password(None, url, username, passwd)

    handler = urllib2.HTTPBasicAuthHandler(passmgr)

    opener = urllib2.build_opener(handler)

    connection = opener.open(req)



In this case, the following behavior is seen:

  1. Client (urllib2) POSTs data leaving out the `Authorization` header

  2. Server responds with a [401][3]. Client (urllib2) never sees the 401
error, and instead gets a [SIGPIPE][4]

  3. Client attempts to reconnect, going right back to step 1

Naturally, the only way I discovered this was to fire up [tcpdump][5] and look
at what was being sent. It quickly became obvious that there was never an
`Authorization` header being sent, and therefore the 401 was completely
justified. The broken pipe seems to have come from the server responding
before reading all of the posted data to thwart off [denial of service][6]
attacks.

My only explanation for why this works perfectly fine on my laptop and not in
either of the data centers I tried running it from is latency, though I'm
shocked, and not yet convinced, by that result.

Incidentally, I solved the problem by sending the `Authorization` header on
first request, rather than waiting for the confirmation that, yes indeed, the
resource needs the Authorization header sent, as `urllib2` likes to do.

`headers = {'User-Agent': useragent, 'Authorization': 'Basic ' +
base64.b64encode(username + ':' + passwd)} req = urllib2.Request(url, data,
headers) connection = urllib2.urlopen(req) `

... and now it works fine.

This whole experience will make me think twice before laying the blame
anywhere until I have uncovered the real truth, something I think all
programmers should learn to do, if they don't know already.

   [1]: http://docs.python.org/library/urllib2.html

   [2]: http://en.wikipedia.org/wiki/Basic_access_authentication

   [3]: http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.4.2

   [4]: http://en.wikipedia.org/wiki/SIGPIPE

   [5]: http://en.wikipedia.org/wiki/Tcpdump

   [6]: http://en.wikipedia.org/wiki/Denial-of-service_attack

