% The Failure of Bayesian Filtering For Automated Screening of Med School Applications
% somis, intelligence, programming, upenn
% 2008-09-18


OK, I'll admit it. I _failed_. I failed to apply a simple Bayes net to a
simple problem, which humans can do with a little bit of reading and some
coffee.

At my place of employment, we have a problem. Our solutions to this problem,
right now, relies too much on time consuming human interaction and, certainly,
too much on the knowledge gained from history. The problem is that of a
multitude of pre-med students hoping to be granted the opportunity to come
here for an interview regarding admission into the world renouned medical
school.

It'd be both impractical and impossible to interview everyone, so there exists
a rigorous screening process before interview decisions are made. We have some
tools that automatically look through an application and parameterise it for
easier searching. We also have some tools that automatically weed out
applications that have extremely low MCATs and GPAs. By and large, these
processes get the number of applications the screening team must thoroughly
inspect down to a manageable number of very competitive candidates, but the
number is still too large.

Now, immediately you must be thinking, Why can't they just change the
parameters to eliminate more people? And the answer is that we could, but what
the screeners look for isn't just numeric. The screeners are looking at many
different aspects of the candidates past. Many of the students accepted to the
school weren't the top students in their class. They certainly weren't
horrible students academically, but they had diverse experiences in school, or
otherwise, that the professors of the school felt qualified them as being an
outstanding addition to this premier institution.

As a programmmer, I see this as a fun and interesting challenge. A challenge
where I can apply some "computer sciency" things to add just a touch of
intelligence to the automated process and filter out those candidates that
maybe are not as "rock star" as they appear in numbers. My first though was to
use a [Naive Bayes classifier][1].

A medical school applicant writes a lot. [AMCAS][2] asks of students to submit
information on past experiences (e.g. research, personal, travel,
professional, community service), and for information about publications
they've (co)authored, and a personal statement. In addition, applicants submit
some other free form text to us directly. This data seems like a natural fit
for training a classifier.

So, I go with it; I mention to my project manager that it might be possible to
probabalistically predict the scores generated from the manual, time consuming
screening process, using historic applicant entered data as a training corpus.
As his eyes lit up, I explained the basic idea of a Naive Bayes classifier and
said, it's the basis of many SPAM filters. This was enough to sell it to him,
and to our immediate client, the Admissions office. I now had permission to
play around and see how well I could predict the past.

From the database, I pulled out as much data as I possible could and build two
buckets; successful and unsuccessful. Successful candidates had the property
that they were all likely invited for interviews, and well unsuccessful
weren't. I tried to make it roughly even, taking as many of the least
successful candidates as I could to match as closely as possible to the number
of successful candidates. I proceeded to write a basic Naive Bayes classifier,
made it general enough that I could add and subtract some ideas easily, and
setup a simple cross validation. I was set.

I couldn't have been more excited to see things pass the screen like:


    Run 0 -------------

    Good: S 57 / F 17 = 0.773333

    Bad: S 22 / F 49 = 0.319444

    Run 1 -------------

    Good: S 71 / F 3 = 0.960000

    Bad: S 8 / F 63 = 0.125000

    Run 2 -------------

    Good: S 70 / F 4 = 0.946667

    Bad: S 13 / F 58 = 0.194444

    Run 3 -------------

    Good: S 61 / F 13 = 0.826667

    Bad: S 12 / F 59 = 0.180556

But the results didn't look good to me. The first line "Good: ... " states
that of the 74 attempts at classifying a known "successful" candidate only 57
were predicted correctly. Even worse, only 22 of the "unsuccessful" candidates
were predicted successfully. Surely, this could be fixed, I thought. It's
simply a matter of doing better smoothing, and eliminating the common words
with high frequencies.

Still nothing. Stem the words? Worse. Eliminate more words? Worse. Smoothing?
No better. Discouragement sets in.

I look into it a bit further and discover something interesting. All these
essays, all these experiences sound exactly the same! They all talk about
community service and late nights in the research labs. They all discuss the
fact that for as long as they can remember they've wanted to become
physicians. It's as if all the candidates are the same person.

They aren't. Unlike a note from my wife and an offer for viagra or cialis,
their goals are the same, and their language is in the same domain.

Seems like I need another idea.

   [1]: http://en.wikipedia.org/wiki/Naive_Bayes_classifier (Naive Bayes on
WikiPedia)

   [2]: http://www.aamc.org (American Medical College Application Service)

  *[GPA]: Grade Point Average

  *[MCAT]: Medical College Admission Test

