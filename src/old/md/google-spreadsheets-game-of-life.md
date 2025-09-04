% Google Spreadsheets: Game of Life
% game-of-life, google, spreadsheets, platform
% 2010-06-25



Google Docs has become an incredibly useful tool for businesses looking to cut
costs on software and infrastructure, and it just keeps getting better.

For years, many businesses were tied to Microsoft Windows due to macros that
they made use of in Excel and Word, but that's about to change. Recently,
Google Spreadsheets released "[Apps Script][1]," which allows you to extend
Spreadsheets and/or integrate other Google products like Calendar.

Apps Scripts are written in JavaScript and the environment provides you with a
bunch of different [services][2], which allow you to access your contacts,
maps, XML, fetch URLs from the Internet, get Google finance information, send
email--all sorts of things that make it possible to create complicated
workflows and applications. I can imagine that there will be a marketplace
sometime soon, which will be bustling with great apps to buy and use.

For now though, you're on your own.

Anyway, yesterday, Google had a hackathon, and because I had no real business
oriented application ideas I thought I'd make a game. The trouble is, all
processing is done on the server side, so real time interaction is a bit
difficult--if not impossible. There are timer events, form events and menu
events, but you can't bind a key or anything like that--at least not that I
know of.

So, a game like Tetris or Snake was out, but not Conway's classic [Game of
Life][3]. The Game of Life is a zero-player game in which the initial board
evolves to form interesting patterns or disappears into nothing. It's normally
drawn on a grid of some sort, so I thought a spreadsheet was appropriate.

 ![][4]

I've put the source code up on [GitHub][5], along with some more instructions
if you wanna check it out. Fork it.

   [1]: http://www.google.com/google-d-s/scripts/scripts.html

   [2]: http://www.google.com/google-d-s/scripts/guide_chapter_01.html

   [3]: http://en.wikipedia.org/wiki/Conway's_Game_of_Life

   [4]: http://files.sigusr2.net/images/game-of-life-gosper.png

   [5]: http://github.com/apgwoz/app-script-game-of-life

