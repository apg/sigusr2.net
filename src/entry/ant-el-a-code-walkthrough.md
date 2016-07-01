% ant.el: A Code Walkthrough
% emacs, ant, walkthrough
% 2010-10-29

In April of last year, I blogged about [how powerful Emacs][1] is, and shared
my enlightenment story.

Today, I'm even closer to Emacs than I was before, having an extra year and a
half of experience than I did then. My daily interactions with Emacs are still
very much in the same vain--I am still a programmer after all, but I've also
discovered some more frustrations which were fairly easy to rectify.

In July of this year, I started [a new job][2][[1]][3]. My role at Meetup has
been different in many regards than my previous jobs. It's the first role I've
had since entering into the industry that someone doesn't see the output of my
code. In fact, if they do see the output of my code, I've sort of
failed.[[2]][4] It also represents the first time that I've primarily used a
compiled language (outside of college), targetted the [JVM][5] and used
[Java][6] for the bulk of my work.

Naturally, Emacs could help me. [Compilation Mode][7], for instance allows you
to do `M-x compile` to run [make][8], or [ant][9] (with `-emacs`) or some
other build tool that generates compilation mode compatible output, that the
mode will then mark up and allow you to easily jump to places the compiler
thinks[[3]][10] are errors.

Compilation Mode by default is fine, but complicated Java source trees are
normally nested quite complexly and ant isn't the greatest at locating a
suitable build file. What I needed were some interactive functions that I
could use to run ant properly, in Compilation Mode using the project's build
file. What I [came up with][11] after a few hours, works fairly well.

Basically, instead of `M-x compile`, I type `M-x ant`, or `M-x ant-compile`,
or `M-x ant` and then TAB complete all the available build targets. This
allows me to save some typing, and save some precious brain cells, since I
don't have to remember all 40 build targets.

The code is fairly simple, and I think it's a great candidate for a walk
through on how to solve your own problems using Emacs Lisp, so I'd like to go
through it here.

The code starts out simply enough:


    (defvar ant-last-task "compile")
    (defvar ant-build-file-name "build.xml")
    (defvar ant-command "ant -emacs")
    (defvar *ant-tasks-cache* '())
    (defvar *ant-tasks-command* "grep -e '<target.*name=\"[^\-][^\"]*.*$'")
    (defvar ant-tasks-default '("compile" "test" "clean"))

We just define a bunch of global variables that can be overridden by a user if
they need to be. We do however create `*ant-tasks-cache*` and `*ant-tasks-
command*` which are meant to be internal state. In elisp, and other Lisps,
wrapping * around a variable normally indicates that the variable is special
in someway, here it means it's a global and shouldn't be modified outside of
the functions defined within.

Next we have a helper function, `ant-find-tasks`, which, given a directory,
issues a shell command `*ant-tasks-command*` that is used to extract the lines
from the ant build file that declare targets:


    (defun ant-find-tasks (directory)
      (let ((output (shell-command-to-string (concat *ant-tasks-command* " "
                                                     directory "/"
                                                     ant-build-file-name))))
        (if (> (length output) 0)
            (mapcar '(lambda (x) (replace-regexp-in-string
".*<target.*name=\"\\([^\-][^\"]*\\).*" "\\1" x))
                    (split-string output "[\n]"))
          nil)))

Essentially, we first save the output of the shell command that gets built up
to the variable `output`. If the length of `output` is greater than 0, we
split the output (`split-string`) into individual lines, and iteratively
replace the junk in the line with just the name of the target (`replace-
regexp-in-string`). The only non-obvious thing in here is `mapcar` which is a
fancy way of transforming a list into another list via the function passed as
the first argument.

That gives us all the tasks defined in the build file--well for most cases
anyway. It doesn't handle all possible, valid build.xml files, but should work
if the `target`'s declaration and name attribute appear on the same line.

When we need a list of tasks for a project, we call `ant-tasks`. This function
really just caches the returned value of `ant-find-tasks` into the global
variable `*ant-tasks-cache*` for the current project:


    (defun ant-tasks (directory)
      (let ((tasks (assoc-string directory *ant-tasks-cache*)))
        (or tasks
            (progn
              (let ((newtasks (or (ant-find-tasks directory) ant-tasks-default)))
                (setq *ant-tasks-cache*
                      (cons (cons directory newtasks) *ant-tasks-cache*))
              newtasks)))))

The only interesting thing about the above code is the use of `or`. In Lisp,
`or` short circuits and returns the first truthy value--it doesn't convert it
to a boolean, so it can easily be used to select the first truthy value from a
list of values. That's what's happening there.

I mentioned that I wanted TAB completion on task names so as to not clutter my
brain matter with useless task names:


    (defun ant-get-task (directory)
      (let ((task (completing-read-multiple (concat "Task (default): ")
                                            (ant-tasks directory))))
        (if (> (length task) 0)
            (mapconcat 'identity task " ")
          "")))

Well, here it is. Emacs has built in completion via the `completing-read`
function. Here, we want the ability to issue one or more tasks, so we use
`completing-read-multiple` to get the job done. Notice we're calling our
function from above, `ant-tasks`.

You'll notice `mapconcat` above. It's like `mapcar` from above, in that it
takes a list of things and transforms them, but instead of returning a new
list, it concatenates the elements into a string using the last argument (here
just a space) as a separator. `completing-read-multiple` returns to us a list,
which we need to turn into a string with spaces between the targets in order
to issue the build command.


    (defun ant-find-root (indicator)
      (let ((cwd default-directory))
        (while (and (not (file-exists-p (concat cwd indicator)))
                    (not (string-equal (expand-file-name cwd) "/")))
          (setq cwd (concat cwd "../")))

        (if (file-exists-p (concat cwd indicator))
            (expand-file-name cwd)
          nil)))

The above function locates the base directory of the project, given the
current file being edited. It just loops up the file system looking for the
first directory it finds that has a build file in it. When I originally wrote
this function, I didn't realize that Emacs already had this functionality
built into it with the function `locate-dominating-file`.[[4]][12]

`ant-kill-cache`,

    (defun ant-kill-cache ()
      (interactive)
      (setq *ant-tasks-cache* '()))

does exactly what it says it does. It destroys the cache that is built up from
`ant-tasks`.

The main entry point, `ant`, is fairly trivial as well. It sets the variable
`default-directory`, which is Emacs' current working directory, to the project
directory, reads a task from the reader (if being called interactively) and
calls `compile`, which is the main entry point into Compilation Mode.


    (defun ant (&optional task)
      "Run ant `task` in project root directory."
      (interactive)
      (let ((default-directory (ant-find-root ant-build-file-name)))
        (if default-directory
            (let ((task (or task (ant-get-task default-directory))))
              (setq ant-last-task task)
              (compile (concat ant-command " " task)))
          (message "Couldn't find an ant project."))))

In the first code block, there was a variable, `ant-last-task`, defined that
is used above to store the last target run. After a target is given, it's
saved off in there.

    (defun ant-last ()
      "Run the last ant task in project"
      (interactive)
      (ant (or ant-last-task "")))

    (defun ant-compile ()
      (interactive)
      (ant "compile"))

    (defun ant-clean ()
      (interactive)
      (ant "clean"))


    (defun ant-test ()
      (interactive)
      (ant "test"))

The rest of the code above, just defines some convenient, interactive commands
for common targets. `M-x ant-compile` will just issue the compile target,
likewise for `ant-clean`. The only moderately interesting interactive command
here is `ant-last` which reuses the variable `ant-last-task` from above to
redo the last compilation.

It doesn't feel like much code, and in all reality it's not. However, it has
saved me quite a bit of time, as I don't have to go running to a terminal
(either switching buffers, or switching windows entirely) in order to issue an
ant task. I just do something I've grown very accustomed to--I issue another
Emacs command, and let Emacs take care of it for me.

  1. We're hiring for [most positions][13].

  2. Most of what I do is related to our product's infrastructure--not
interaction, and not engineering new features. My work would be visible if
some error leaked to your browser, and that'd probably be bad.

  3. It's fairly difficult to trick the compiler, but in Java, for instance,
you can write type-safe code using Generics that it can't prove is safe. If
you do that though, you should make use the SuppressWarnings annotation to
tell the compiler it's OK.

  4. Actually, I wasn't surprised that Emacs actually had the functionality,
just that the name is non-trivial. I gave up looking for it after a couple of
minutes and just rolled my own. I'll replace it in a future version, but the
existing code works for now.

   [1]: http://sigusr2.net/2009/Apr/30/the-power-that-is-gnu-emacs.html

   [2]: http://www.meetup.com/

   [3]: #hiring-1

   [4]: #output-2

   [5]: http://en.wikipedia.org/wiki/JVM

   [6]: http://java.sun.com/

   [7]: http://www.gnu.org/software/emacs/manual/html_node/emacs/Compilation-
Mode.html

   [8]: http://en.wikipedia.org/wiki/Make_(software)

   [9]: http://ant.apache.org/

   [10]: #errors-3

   [11]: http://github.com/apgwoz/ant-el

   [12]: #dominating-4

   [13]: http://www.meetup.com/jobs

