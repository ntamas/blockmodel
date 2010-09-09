============
 blockmodel
============

----------------------------------------
fitting stochastic blockmodels to graphs
----------------------------------------

:Author: Tamas Nepusz <tamas@cs.rhul.ac.uk>
:Date: @DATE@
:License: GPL
:Version: 0.1
:Manual section: 1
:Manual group: scientific tools

SYNOPSIS
========

  blockmodel [options] inputfile

DESCRIPTION
===========

OPTIONS
=======

General options
---------------

-h, --help            Shows a help message and exits
-V, --version         Shows the version number and exits
-q, --quiet           Quiet mode (less output)
-v, --verbose         Verbose mode (more output)

Basic algorithm parameters
--------------------------

-g K, --groups K      Sets the desired number of groups to ``K``. The default is
                      -1, which means autodetection.

Advanced algorithm parameters
-----------------------------

--block-size N        Sets the block size used when determining the convergence
                      of the Markov chain. Consecutive blocks of size N will be
                      taken and their average log-likelihood will be compared.
                      The average should be roughly equal if the Markov chain converged
                      to the stationary distribution. The default block size is
                      10000 samples.

--init-method METHOD  Uses the given initialization method to select the first
                      state of the Markov chain. The following options are
                      available:

                      random
                        starts from a random configuration

                      greedy
                        uses a simple greedy optimization scheme to find a
                        configuration that is thought to be close to the mode
                        of the likelihood distribution.

                      The default method is the greedy one.

--log-period COUNT    Shows a status message after every ``COUNT`` steps with
                      the current and best log-likelihood and several other
                      information. The default value is 8192.

--seed SEED           Seed the internal Mersenne Twister random generator with
                      the given number (and make the result deterministic).

PROBLEMS
========

None that I know of.

BUGS
====

None that I know of.
