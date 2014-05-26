gorilla
=======

Good Old Fashioned AI
---------------------

Successful computer Go programs typically rely on a mixture of tree search,
knowledge-based systems, and machine learning. Implementing these, especially
the pattern matching components, is no small task.

This AI has a minimalist design. It is an experiment on the failures of a
previous project, which used Monte-Carlo methods. Even though its skill wasn't
as bad as it had been expected, it was suspected, and noticed, that trivial AIs
which worked thousands of times faster --because they performed no look-ahead--
were as good, if not better than our statistical approach.

So this AI does just that: no look-ahead. It has simple knowledge of stone
formations, and acts upon this: improves the liberties of current groups, and
attempts to expand these by joining nearby stones. This is interesting because
such knowledge is typically non-trivial to construct if redone from scratch at
each move, but building it along during the game is very simple.

Example game
------------

This is a game that was played between this AI (black) and the montecarlo one
(white). The result as a total victory for black.

![alt text](http://imgur.com/mBEoB8B "sample game")

Other games have been well balanced, or even total white victory.
