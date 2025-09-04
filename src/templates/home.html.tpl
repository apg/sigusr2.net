<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="icon" type="image/png" href="/favicon.ico" />
    <link href="/feed.xml" type="application/atom+xml" rel="alternate" title="Blog Atom Feed" />
    <title>{{title}}</title>
    <style type="text/css">
      {{<styles.css}}
    </style>
</head>
<body>

  <main>
    {{!content}}

    <h3>Posts by Recency</h3>

    <ul>
      {{#pages}}
      <li><strong><a href="./{{name}}.html">{{title}}</a></strong>&mdash;{{publish_date}}</li>
      {{/pages}}

      <li><em><a href="/posts.html">All Posts</a></em></li>
    </ul>
  </main>

  {{<./templates/layout-footer.html}}
</body>
</html>
