% envjson: Config Checking for 12 Factor Apps
%
% 2016-09-28

The 12 factor app uses environment variables for configuration. This
has lots of advantages which you can read about in the
[config factor section][config]. As much as there are advantages,
there is at *least* one major disadvantage, too--there's no standard
way to ensure the config is complete.

All too often we screw up a deploy of an app on Heroku, inciting 2
minutes of panic while we struggle to attach a missing addon, or set
the forgotten config var. The app will just sit there spewing out
insults: "You forgot to set `MAX_BUFFER_SIZE`, moron!". Heroku's
watchdog will continually try to restart our process, but to no avail.

In the worst of cases, our app might have some downtime, because we
didn't actually notice that the app was crashing until it failed a
health check, or even worse, a customer reports to us that "shit's not
working."

Some software has a way to prevent this. Nginx, for instance, allows
you to pass a `-t` which will test that the config is OK, and allow
you to deploy with a bit more confidence.

What's a 12 factor app to do?

I recently released a first version of [envjson][envjson], which
attempts to solve this problem:

    $ heroku config --json | envjson - manifest.json

will exit with failure, which can be checked as a prerequisite to
deployment. This can be used conveniently as a pre-flight deployment
gate. Is everything ready to go? If not, *do not* deploy.

What is happening here is fairly straightforward. envjson by itself,
is very similar to [env(1)][manenv]. It sets up and runs a program in
a modified environment. The catch here is that envjson does a bit more
than that.

Based on the contents of `manifest.json`, it validates that all the
environment variables are provided, and if they are marked as required
have a non-empty value.

This means that you can use envjson in two key ways. As part of your
pre-flight deployment checklist, with the command above, or as part
of the command in your Procfile:

    $ cat Procfile
    web: envjson manifest.json /path/to/12-factor-web-server`

As an extended bonus, the manifest is then a sort of "executable"
documentation for the configuration of your app. As such, entries in
manifest can also have a `doc` field, so that you can explain what the
variable actually does.

Check out [envjson][envjson] on GitHub.

[config]: https://12factor.net/config
[manenv]: https://linux.die.net/man/1/env
[envjson]: https://github.com/apg/envjson
