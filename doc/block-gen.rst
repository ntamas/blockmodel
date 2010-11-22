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

-f FORMAT, --input-format FORMAT
                      Sets the format of the input file. The default value is
                      **plain**, which is a simple plain text format. See
                      the manual page of ``block-fit`` for more information on
                      the supported file formats.

-F FORMAT, --output-format FORMAT
                      Sets the format of the output file. The default value is
                      **edgelist**, which simply dumps the edge list to the
                      output file. See the section on `Output formats`_ for
                      more details.

--model MODEL         Selects the model to be used. The following options are
                      available:

                      uncorrected
                        Standard undirected blockmodel. This model aims to
                        keep the expected number of edges.

                      degree
                        Degree-weighted undirected blockmodel. This model aims
                        to keep both the expected number of edges and the
                        expected degree of each vertex.

OUTPUT FORMATS
==============

This section lists all the output formats supported by the ``-F`` option.

edgelist
    Simple numeric edge list format. Each line encodes an edge of the graph,
    the first column being the source vertex of the edge, the second column
    being the target vertex. Vertices are referred to by numbers from 0 to
    *n*-1, where *n* is the number of vertices.

leda
    The LEDA native format, supported by LEDA and GraphWin.

PROBLEMS
========

None that I am aware of.

BUGS
====

None that I am aware of.
