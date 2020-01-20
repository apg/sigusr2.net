# Serving Traffic "Directly" from SQLite

Late last week I was nerd sniped by the idea that you might be able to push more work into SQLite than just data storage. This question came from a [post on Lobsters](https://lobste.rs/s/ivpsdw/j4mie_sqlsite) about [sqlsite](https://github.com/j4mie/sqlsite/blob/master/README.md), which serves static websites "directly" from SQLite. 

Well, it uses a bunch of Python to do it "directly," so I wondered... how far could you actually take it? 

On Lobsters, I [suggested](https://lobste.rs/s/ivpsdw/j4mie_sqlsite#c_eudxo4) that a server could "queue" the request information into the database, such that a worker thread could pull the data out and write to the socket FD stored in the row. That doesn't really buy us much, actually, so we'll assume that the non-SQLite part of this hack can parse an HTTP request after accepting a socket, make a query to SQLite, and write back a response. 

With the scope of this hack in place, we first define some tables. We need _something_ to render dynamically, so we have a `templates` table.

```
CREATE TABLE templates (
  id int PRIMARY KEY,
  name text,
  body text
);
```

How do we check to see if a page can be rendered? We check the `routes` table, of course!

```
CREATE TABLE routes (
  id int PRIMARY KEY,
  path text,
  template_id int
);
```

The route gives as a path, an id, and a template_id to utilize. But, what do we splice into the template? That's where `route_data` comes in!

```
CREATE TABLE route_data (
  route_id int,
  var text,
  val text
);
```

We get key value pairs that can be spliced into a template, dynamically. This all becomes clear as we define 3 pages; 2 "blog posts" and an "about" page.

```
INSERT INTO templates (
  id, name, body
) VALUES
  (1, 'about', 'This is an about page about %NAME%.'),
  (2, 'post', 'This is a blog post titled %TITLE%, with the body %BODY%.');


INSERT INTO routes (
  id, path, template_id
) VALUES
  (1, '/about', 1),
  (2, '/post/hello-world', 2),
  (3, '/post/take-2', 2);


INSERT INTO route_data (
  route_id, var, val
) VALUES
  (1, 'NAME', 'Andrew Gwozdziewycz'),
  (2, 'TITLE', 'Hello, World'),
  (2, 'BODY', 'Hello, world. This is a post body!'),
  (3, 'TITLE', 'Hello, world: Take 2!'),
  (3, 'BODY', 'This is the post body for the Take 2 post!');
```

Great! As you can see these pages are pretty simple. While we could setup a foreign function to render these templates as markdown, or whatever else, I have spent way too much time thinking about this all as it is, so we'll leave that as an exercise to the reader. Same for control structures, arbitrary includes, and whatever else your fancy schmancy templating languages support these days. _(Back in my day, we typed out HTML for every response LIVE while the client was waiting on the other end&endash;Ed)_

Here's the punchline. With `RECURSIVE` Common Table Expressions, we have the ability to _split_ the template body up by the `'%'` character, and join the `route_data` with a potential variable name. If we then `group_concat` each of the parts, we collapse the exploded template back into a single row, ready to serve out.

```
WITH RECURSIVE
  render(p, rest) AS (
    select '',  t.body from routes r
      inner join templates t on r.template_id = t.id
      where r.path = '/about'
    UNION ALL
    select case when instr(rest, '%') = 0 then rest
                else substr(rest, 0, instr(rest, '%')) end,
           case when instr(rest, '%') = 0 then ''
                else substr(rest, instr(rest, '%')+1) end
    from render
    where rest != ''
  )
  SELECT group_concat(part, '')
  FROM 
       (select coalesce(rd.val, r.p) as part
        from render r
        left outer join route_data rd on r.p = rd.var)
```

This results in a single row:

```
This is an about page about Andrew Gwozdziewycz
```

If we want to take that even farther, we can generate the _whole_ HTTP response with just a bit more work:

```
WITH RECURSIVE
  found(yes) AS (select count(*) from routes where path = '/about'),
  headers(part) AS (VALUES ('\r\n'), ('Content-Type: text/html\r\nConnection: close\r\n\r\n')),
  render(p, rest) AS (
    SELECT '',  t.body FROM routes r
    INNER JOIN templates t ON r.template_id = t.id
    WHERE r.path = '/about'
    UNION ALL
    SELECT 
           CASE WHEN instr(rest, '%') = 0 THEN rest
                ELSE substr(rest, 0, instr(rest, '%')) END,
           CASE WHEN instr(rest, '%') = 0 then ''
                ELSE substr(rest, instr(rest, '%')+1) END
    FROM render
    WHERE rest != ''
  )
  SELECT group_concat(part, '')
  FROM (
    SELECT CASE WHEN yes = 1 THEN 'HTTP/1.1 200 OK' 
                ELSE 'HTTP/1.1 404 Not Found'
                END as part
    FROM found
    UNION ALL
    SELECT part from headers
    UNION ALL
    SELECT coalesce(rd.val, r.p) as part
    FROM render r
    LEFT OUTER JOIN route_data rd on r.p = rd.var
  )
```

Giving us:

```
HTTP/1.1 200 OK
Content-Type: text/html
Connection: close

This is an about page about Andrew Gwozdziewycz.
```

The `RECURSIVE` nature of this Common Table Expression allows us to build up a result set recursively until the building query returns no results. In the `render` result set, our goals are to:

1. Find the corresponding template to render (`SELECT body FROM routes JOIN templates... `)
2. _split_ the found body on `'%'`, which is our delimiter for our variable substitutions.

After success in each of those steps, we end up with a result set of:

```
p, rest
"", "This is a page about %NAME%."
"This is a page about ", "NAME%."
"NAME", "."
".",""
```

At this point, the template is "tokenized", and our goal is to issue a query which joins the `render` result set with the `var` in the `route_data` table, from which we select the associated `val`. The `coalesce` function evaluates to the first non-null value, and because we're doing a `LEFT OUTER JOIN` we're going to get a `NULL` if the `var` does not equal the `p` in the `render` set. In that case, the `coalesce` evaluates to the `p` giving us back `"This is a page about "` or `"."`. 

In order to get the actual body, it's just a matter of concatenating all the 1 column rows, with `group_concat`, using a blank delimiter and we get a rendered template.

The query related to the HTTP headers and status line complete the _full_ HTTP response. A quick shim to make this a _full_ web server (_I've done this in Go, but it could be done with bash, netcat, etc, etc, etc&endash;Ed_) follows (_obviously, don't use this in production&endash;Ed_):

```package main

import (
	"bufio"
	"database/sql"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"

	_ "github.com/mattn/go-sqlite3"
)

func errResponse(status string) string {
	const template = `HTTP/1.1 %s
Content-Type: text/html
Connection: close

<html><head><title>%s</title></head><body><h1>%s</h1></body></html>
`
	return fmt.Sprintf(template, status, status, status)
}

const query = `WITH RECURSIVE
  found(yes) AS (select count(*) from routes where path = ?),
  headers(part) AS (VALUES ('Content-Type: text/html
Connection: close

')),
  render(p, rest) AS (
    SELECT '',  t.body FROM routes r
    INNER JOIN templates t ON r.template_id = t.id
    WHERE r.path = ?
    UNION ALL
    SELECT
           CASE WHEN instr(rest, '%') = 0 THEN rest
                ELSE substr(rest, 0, instr(rest, '%')) END,
           CASE WHEN instr(rest, '%') = 0 then ''
                ELSE substr(rest, instr(rest, '%')+1) END
    FROM render
    WHERE rest != ''
  )
  SELECT group_concat(part, '')
  FROM (
    SELECT CASE WHEN yes = 1 THEN 'HTTP/1.1 200 OK
'
                ELSE 'HTTP/1.1 404 Not Found
'
                END as part
    FROM found
    UNION ALL
    SELECT part from headers
    UNION ALL
    SELECT coalesce(rd.val, r.p) as part
    FROM render r
    LEFT OUTER JOIN route_data rd on r.p = rd.var
  )
`

func getResult(db *sql.DB, path string) string {
	var result string
	err := db.QueryRow(query, path, path).Scan(&result)
	if err != nil {
		log.Printf("query failed: %s", err)
		return errResponse("500 Internal Server Error")
	}
	return result
}

func handleConnection(conn net.Conn, db *sql.DB) {
	defer conn.Close()
	req, err := http.ReadRequest(bufio.NewReader(conn))
	if err != nil {
		log.Printf("failed to read request: %s", err)
		conn.Write([]byte(errResponse("401 Bad Request")))
		return
	}
	if req.Method != http.MethodGet {
		conn.Write([]byte(errResponse("405 Method Not Allowed")))
		return
	}

	log.Printf("getting path=%s", req.URL.Path)
	result := getResult(db, req.URL.Path)
	conn.Write([]byte(result))
}

func main() {
	db, err := sql.Open("sqlite3", "./site.db")
	listener, err := net.Listen("tcp", "localhost:8080")
	if err != nil {
		log.Printf("failed to create listener: %s", err)
		os.Exit(1)
	}

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("failed to accept connection: %s", err)
			continue
		}

		go handleConnection(conn, db)
	}
}
```

I ran out of steam, but I think it's likely possible to _parse_ HTTP from within SQLite... or at least, the important part to extract that path. I'll leave that as an exercise to the reader. :)

_- 2020/01/20_
