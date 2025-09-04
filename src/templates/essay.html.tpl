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
  {{<./templates/layout-nav.html}}

  <main>
    <h2>{{title}}</h2>

    {{!content}}

    <em>&mdash;{{publish_date}}</em>

    <footer>
      <strong>incoming: </strong>
      {{#BACKLINKS}}
      <span><a href="/{{name}}.html">{{title}}</a></span>
      {{/BACKLINKS}}
    </footer>
  </main>


  {{<./templates/layout-footer.html}}
</body>
</html>
