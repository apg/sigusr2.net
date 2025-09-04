<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
  <title>{{site_title}}</title>
  <link href="{{site_url}}/feed.xml" rel="self"/>
  <updated>{{UTCNOW}}</updated>
  <id>{{site_url}}/</id>
  <author>
    <name>{{author_name}}</name>
    <email>{{author_email}}</email>
  </author>
  <subtitle>{{site_description}}</subtitle>

{{#pages}}
  <entry>
    <id>{{parent.site_url}}/{{name}}.html</id>
    <title>{{title}}</title>
    <link href="//{{name}}.html" />
    <published>{{publish_date}}T00:00:00Z</published>
    <updated>{{publish_date}}T00:00:00Z</updated>
    <summary>post</summary>
    <content type="xhtml"><div xmlns="http://www.w3.org/1999/xhtml"> <p>Read <a href="{{parent.site_url}}/{{name}}.html">{{title}}</a></p></div></content>
  </entry>
{{/pages}}
</feed>
