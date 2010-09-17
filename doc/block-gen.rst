===========
 block-gen
===========

------------------------------------------------------
Generating graphs according to a stochastic blockmodel
------------------------------------------------------

:Author: Tamas Nepusz <tamas@cs.rhul.ac.uk>
:Date: @DATE@
:License: GPL
:Version: 0.1
:Manual section: 1
:Manual group: scientific tools

SYNOPSIS
========

  block-gen [options] inputfile

DESCRIPTION
===========

OPTIONS
=======

-h, --help            Shows a help message and exits
-q, --quiet           Quiet mode (less output)
-v, --verbose         Verbose mode (more output)
-V, --version         Shows the version number and exits

-o FILE, --output FILE
                      Write the results to the given *FILE*. If no output
                      file is given, the default is the standard output.

-c N, --count N       Sets the number of graphs to be generated. If this is
                      larger than 1, the output must not be the standard output
                      and the output file name must be a C-style format string
                      where `%d` will be replaced by the index of the generated
                      graph. Indexing starts from zero.

PROBLEMS
========

None that I am aware of.

BUGS
====

None that I am aware of.
