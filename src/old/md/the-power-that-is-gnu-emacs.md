% The Power That is GNU Emacs
% emacs, gnu, editor, #pinned
% 2009-04-30


If you've never been convinced before that [Emacs][1] is the text editor in
which dreams are made from, or that inside Emacs there are unicorns
manipulating your text, don't expect me to convince you.

I'm not going to sit here and type out a Top 10 list of reasons _you_ should
be using GNU Emacs, nor am I going to tell you that you are an idiot for using
VI (I was a hardcore, "down with emacs," VI user in the late 90s and early
00s). Instead, I'm going to talk about why Emacs works for _me_. If you're so
inclined to give it a try after reading this, then so be it. Searching
[Google][2] is bound to get you a ton of good information, or you could just
start Emacs by typing `emacs` at the terminal and typing `C-h` (`C-` means
hold down the control key and hit `h` [in this case]. `M-_key_` means hold
down the Meta key [ALT on most keyboards] and hit `_key_`. If there's not a
`-` between them, you can release Control or Meta before hitting the _key_).

I first started using Emacs in the spring of 2003, when an acquaintance at the
time suggested that Emacs was superior and that I could even use [viper-
mode][3] to simulate VI. The idea that a text editor was powerful enough to
simulate another caught my curiosity. I fired it up, and hit the backspace
key. Within moments I was put off when the Emacs equivalent of [Clippy][4],
came to my rescue.

See, Emacs responds logically to `C-h`, with "Help." The problem is that on
many modern keyboards, the backspace key and `C-h` send the same value to the
terminal.

So, I fired up [Phoenix][5] (or was it Firebird then? Maybe it was
Fireweasel?) and searched Google for "emacs backspace problem." Within .004
seconds I had over 3 million pages telling me how to fix the problem. It said
to add:

`(global-set-key "\C-h" 'delete-backward-char)`

to my ~/.emacs file. So, I went to my terminal and did what anyone would have
done in my situation--I typed `vi ~/.emacs`, pasted the snippet and `ESC
:wq!`'d

But, despite my trouble I was intrigued. First of all, I had never seen a Unix
program configured the way it was. Why didn't Emacs just use something like
[INI][6] files, or some other simple format? Later, I realized that I wasn't
just configuring Emacs--no, I was _programming_ the editor to behave the way
_I_ preferred.

And, you'll say, "Well, how is that any different than configuration?" To,
which I'll reply, "Every command you invoke, even the command that inserts the
letter 'j' into this quote here, is calling a function written in the language
I used before to set `C-h` to behave the way I preferred."

You see, Emacs isn't a text editor. Emacs is a programming language that you
can use to write your _own_ text editor. That's why `viper-mode` exists.
Someone used Emacs to write VI.

And, as I read more about Emacs, and about all the editors people have written
(that is to say, editors for writing C, Perl, Python, etc), I got more
excited; that is until I discovered `M-x tetris`. Why does my editor need
[Tetris][7]? It doesn't, and when I discovered more games, I got even more
disgusted.

I'm not going to lie--not even a little bit, in 2003, Emacs was slow. Machines
were getting faster, [RAM][8] like always was cheap, but it was torture to
type `emacs _filename_` more than once a day. The games, and all this bloat
were obviously the cause right? Well no. It turns out that Emacs has smart
ways to load code, so you can save your precious memory. As a naive user,
though, I was a bit upset.

So, off to Google again. "Google, How do I speed up Emacs?" "Well, my young
Padawan," Google replied, "you should use the server. Add `(server-start)`
(`M-x describe-function RET server-start RET` if you want more info) to your
`~/.emacs` file and connect with `emacsclient`." This was a few days later,
and I had a cheat sheet on my desk, so I didn't escape to the terminal and
start up VI. This time, I knew to chord `C-x C-f`, make my changes and chord
`C-x C-s`. Easy-peasy.

The common theme here is that documentation for Emacs was easily accessible
and there was a seemingly endless community of people answering questions
about common pitfalls when getting started with Emacs. As a newbie, this fact
made it extremely easy to want to continue learning, and as an advanced user
today (can one ever gain the rank of Emacs expert?) this fact still draws me
in.

Emacs has since gotten faster as a result of extremely cheap processing power,
but VI of course still wins in startup time. It's not a very good metric for
comparing the two pieces of software though. For one, I mentioned that Emacs
had a server that you could use to keep Emacs going and connect with
`emacsclient`, but I should also mention that I hardly ever use it. Emacs has
an extensive file browser (with tab-completion!), and support for editing
multiple files (called buffers). The other thing Emacs has is a mode called
[Tramp][9], which allows Emacs to edit files over [SSH][10], [FTP][11] and
many more. Since this is the case, keeping one Emacs session alive forever
makes startup time nil. VI, on the other hand encourages relaunching with new
files. I'm fairly sure modern VIs support editing more than one file at once,
but I'm also pretty convinced that most VI users know about 15 things about
VI, and multiple file editing isn't one of them.

The reason I even still have `(server-start)` in my `~/.emacs` has to do with
other utilities that need an editor. Invoking `emacsclient _filename_` will
open `_filename_` in the Emacs instance that the server is running in. No
startup lag--but you do unfortunately have to context switch.

Naturally, the Emacs community has solved this problem of context switch for
many common cases. One of the places where you're bound to need to startup an
editor as a programmer is when interacting with [SCM][12] tools such as
[Subversion][13], [Mercurial][14] or [Git][15]. These tools allow you to leave
messages when you commit changes back to the repository. Committing is common,
as is updating files and working with version control repositories in general,
so it's only natural that your editor be version control aware. Emacs is, and
uses [`vc-mode`][16].

And the more you think about interactions with computers the more you realize
how much of it is still text based. [IRC][17], for instance is entirely text
based, so why make it hard on yourself to switch back and forth between
running programs when you can switch to an IRC buffer in Emacs? I do that, and
it's wonderful.

Todo lists, calendars, calculators, Emacs does it all. It's no wonder that
mere mortals joke about Emacs, saying "I'm happy with my operating system, I
just want a decent text editor!" I said the same thing, and once I tried it,
and took the time to learn it, I could never go back.

### Extending

Over the years, I've discovered lots of things about Emacs, and new modes to
play with. There's a never ending list of things to play with on
[EmacsWiki][18] if you're so inclined.

As a programmer, writing my own enhancements was a matter of learning a bit
about [Emacs Lisp][19], the [Lisp][20] dialect that Emacs is built on. Coming
from a background in C, Perl and at this point in time Python, I laughed at
Lisp. "Why would anyone want to write that many parenthesis?" I've since come
to the conclusion that Lisp is the most powerful set of programming languages
available, but that is a topic for another day. (It is no coincidence, however
that the most powerful text editor is built upon a dialect of the most
powerful programming language.)

One of the first things I extended Emacs with was a way to convert Windows
text files, which use carriage return and a line feed to something more
appropriate for editing on Unix (line feed only). I came up with this with
some help from our friend Google:

`(defun dos2unix () "Convert this entire buffer from MS-DOS text file format
to UNIX." (interactive) (save-excursion (goto-char (point-min)) (replace-
regexp "\r$" "" nil) (goto-char (1- (point-max))) (if (looking-at "\C-z")
(delete-char 1)))) `

I'm fairly certain that I didn't write that completely from scratch, and I
know for certain that I lifted the `(if (looking-at...) ...)` from the
Internet somewhere as I'm not even sure why I would want to delete `C-z`
characters from the end of the file.

Either way, the point is the same, now, without a context switch to the
terminal, and the need to have the dos2unix package installed on my system, I
can open a Windows text file and say, `M-x dos2unix`, and I'm in business.

Another utility that I decided I needed was the ability to split the current
window in a 3/4, 1/4 configuration, instead of the default 1/2, 1/2 that you
get when you call `split-window`. The result looks something like this:

`(defun three-quarters-window () "Resizes current window big" (interactive)
(let ((size (- (truncate (* .75 (frame-height))) (window-height)))) (if (>
size 0) (enlarge-window size)))) `

Bound to a key, `(global-set-key "\C-x7" 'three-quarters-window)`, makes it
easy to keep an eye on an IRC buffer or something else that's changing a few
lines at a time, while remaining relatively focused on the buffer you're
really editing.

As I continue to try out new major modes and spend time programming Emacs to
behave the way I want it to, it's only natural that my `~/.emacs` file has
become a bit large and hard to read. So I built something to fix it. I built
something that allowed me to separate customizations into logical units.
Basically, it works similarly to the [init][21] process from Unix.

I call my version activator. It's basically a few functions and a few
overridable variables that will load a set of files with a specifically
formatted file name in some specific location on your file system.

Using it is as simple as including a few lines in your `~/.emacs` file:

`;;;; Need to tell Emacs which directory has activator.el (setq load-path
(cons _path to directory containing activator.el_ load-path)) (require
'activator) (activator-start) `

By default activator will look for files in `~/.emacs.d/activator.d/` with the
file name format `_XX_description.el`, where _XX_ is a two digit number used
for dependency resolution. In other words, `00settings.el` will be loaded
before `01utils.el` if you were to have those files in your activator load
path.

The code for doing this is surprisingly simple:

`(defun activator-start () "Starts activator, thereby running all the files in
`activator-load-path' that match the `activator-filename-pattern`"
(interactive) (if (not (boundp 'activator-load-path)) (error "Please set
`activator-load-path`") (mapcar 'activator-load-file (activator-get-files))))
(defun activator-get-files (&optional path pattern) "Gets files from path
specified by `activator-load-path', or from the optional path" (let ((path (or
path activator-load-path)) (pattern (or pattern activator-filename-pattern)))
(directory-files path t pattern))) (defun activator-load-file (file) (load-
file file)) `

Of course, the [real code][22] defines the variables `activator-filename-
pattern` and `activator-load-path`, which must be overridden before
`(activator-start)` is run, should you decide you want to change them.

But, the real power in this is that I can rename configuration files to
something that doesn't match the filename pattern if I don't want them to
load, say because my configuration needs to be different at work, rather than
at home. This makes my Emacs customizations pretty portable, and with a few
more tweaks to the way activator works, I could make it even more portable.

I'm not going to be naive and think that I'm the first one to come up with
something like this. I know for sure I'm not, and the only reason I did it
myself was for the exercise of writing Emacs Lisp. All my customizations to
Emacs are loaded currently with activator on all the machines that I use Emacs
on. It's pretty great.

As a programmer who spends many hours a day using a text editor, the
companionship that Emacs provides is a joy, and our relationship just keeps
growing better as I uncover the stories and talents, my little friend has to
share.

   [1]: http://www.gnu.org/software/emacs

   [2]: http://www.google.com/search?q=emacs+tutorial (Google Search Query for
'emacs tutorial')

   [3]: http://www.emacswiki.org/emacs/ViperMode (Viper Mode - VI keybindings
for Emacs)

   [4]: http://en.wikipedia.org/wiki/Clippy

   [5]: http://www.getfirefox.com

   [6]: http://en.wikipedia.org/wiki/INI_file

   [7]: http://en.wikipedia.org/wiki/Tetris

   [8]: http://en.wikipedia.org/wiki/RAM

   [9]: http://www.gnu.org/software/tramp/

   [10]: http://www.openssh.com/ (Secure SHell)

   [11]: http://en.wikipedia.org/wiki/FTP (File Transfer Protocol)

   [12]: http://en.wikipedia.org/wiki/Software_configuration_management
(Source Control Management)

   [13]: http://subversion.tigris.org/

   [14]: http://www.selenic.com/mercurial/wiki/

   [15]: http://git-scm.com/

   [16]: http://www.emacswiki.org/emacs/VersionControl

   [17]: http://en.wikipedia.org/wiki/IRC

   [18]: http://www.emacswiki.org

   [19]: http://en.wikipedia.org/wiki/Emacs_lisp

   [20]: http://en.wikipedia.org/wiki/Lisp_(programming_language)

   [21]: http://en.wikipedia.org/wiki/Init

   [22]: http://gitorious.com/projects/emacs-
configuration/repos/mainline/blobs/master/activator.el

