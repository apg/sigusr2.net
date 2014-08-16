% When
% c, unix, utilities
% 2013-12-12


The other day I released a utility which I called `[when][1]`. The idea of
`when` is to `[watch(1)][2]` a command only until it succeeds, and perform
some action when it does. There was a lot of confusion around this. People
suggested that I missed the boat since the shell can do this natively with a
simple loop. However, what people failed to realize is that `when` solved a
different problem as well.

The other problem that `when` set out to solve is that notification that a
long running process _succeeded_ isn't always enough. Perhaps I wanted to see
if the long running process _started_ successfully (this assumes that your
long running process can fail quickly, though that's what the `-n` option is
for). More on that in a second.

In some ways it's rather silly to have `when` launch the second process when
it's something the shell can easily do--I agree[[1]][3]. So, the default mode
(`-z` mode) might be better described as "retry until 0". Then, basic usage
would look like:


    when "cat /file/that/will/exist" && grep "ERROR" /file/that/will/exist

Of course, the above example is silly since you could do grep instead of cat
to begin with, but I digress. However, if I change `when` to adopt those
semantics, then an invocation need not quote the commands because the shell
will parse it as:


    when cat /file/that/will/exist

and


    && grep "ERROR" /file/that/will/exist

In a bourne compatible shell, `when` never sees anything past the `&&`, which,
with our argument parsing, means that we can call `execvp` directly on `argv`
after incrementing past the options, making the code simpler, and eliminate
the subshell invocation all together! [[2]][4]

What does this do to `-t` mode though, the reason `when` exists to begin with?
Well, that's a bit trickier. In essence it'd make sense to be able to share
the same argument passing semantics--"unquoted" when possible, but the shell
can't help us complete what we're looking for. So, perhaps we do:


    when ssh _temporarily unreachable host_ **-alarm** xmessage 'connected' &&
xmessage 'closed'

This forces us to do more work to parse arguments, but allows us to work in
non-quoted mode, and makes it a bit more clear what's happening. Plus, we
share the same "finished" semantics as in the default mode.

The usage of `-alarm` sort of implies what's happening. Run `xmessage
'connected'` after some event occurs. A user would just have to understand
that the alarm goes off after `N` seconds of the command not being retried.

The only question then becomes whether or not `when` is the proper name for
this.

(thanks to the [suckless-dev][5] list for the discussion.)

  1. By far the biggest complaint!

  2. There's a tradeoff though. We still want to be able to support complex
pipelines to watch, so we a way to determine whether or not a subshell is
needed. Simplicity of always using a subshell might win out.

   [1]: https://github.com/apgwoz/when

   [2]: http://linux.die.net/man/1/watch

   [3]: #note-complaint

   [4]: #note-subshell

   [5]: http://suckless.org

