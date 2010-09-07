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

-g K, --groups K      Sets the desired number of groups to ``K``. The
                      default is -1, which means autodetection.

Advanced algorithm parameters
-----------------------------

--log-period COUNT    Shows a status message after every ``COUNT``
                      steps with the current and best log-likelihood
                      and several other information.

PROBLEMS
========

None that I know of.

BUGS
====

None that I know of.
