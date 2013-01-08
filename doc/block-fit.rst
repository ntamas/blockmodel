===========
 block-fit
===========

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

  block-fit [options] inputfile

DESCRIPTION
===========

OPTIONS
=======

General options
---------------

-h, --help            Shows a help message and exits
-q, --quiet           Quiet mode (less output)
-v, --verbose         Verbose mode (more output)
-V, --version         Shows the version number and exits

Basic algorithm parameters
--------------------------

-F FORMAT, --output-format FORMAT
                      Sets the format of the output. The default value is
                      **plain**, which is a simple plain text format.
                      See the section on `Output formats`_ for more details.

-g K, --groups K      Sets the desired number of groups to *K*. The default is
                      -1, which means autodetection.

-o FILE, --output FILE
                      Write the results to the given *FILE*. If no output
                      file is given, the default is the standard output.

-s N, --samples N     Sets the number of samples to be taken from the Markov
                      chain after convergence to *N*. The default is 100000.

Advanced algorithm parameters
-----------------------------

--block-size N        Sets the block size used when determining the convergence
                      of the Markov chain. Consecutive blocks of size *N* will
                      be taken and their average log-likelihood will be
                      compared.  The average should be roughly equal if the
                      Markov chain converged to the stationary distribution.
                      The default block size is 10000 samples.

--init-method METHOD  Uses the given initialization method to select the first
                      state of the Markov chain. The following options are
                      available:

                      random
                        starts from a random configuration

                      greedy
                        uses a simple greedy optimization scheme to find a
                        configuration that is thought to be close to the mode
                        of the likelihood distribution.

                      The default method is **greedy**.

--log-period COUNT    Shows a status message after every *COUNT* steps with
                      the current and best log-likelihood and several other
                      information. The default value is 8192.

--model MODEL         Selects the model to be used. The following options are
                      available:

                      uncorrected
                        Standard undirected blockmodel. This model aims to
                        keep the expected number of edges.

                      degree
                        Degree-weighted undirected blockmodel. This model aims
                        to keep both the expected number of edges and the
                        expected degree of each vertex.

--seed SEED           Seed the internal Mersenne Twister random generator with
                      the given *SEED* (and make the result deterministic).

OUTPUT FORMATS
==============

This section lists all the output formats supported by the ``-F`` option. In
each output format, vertex types are referred to by numbers from 0 to *k*-1,
where *k* is the number of types in the model.

plain
    Plain text output format which lists the types for each vertex in the input
    graph and the probability matrix for the best model found. The output is
    divided into sections, where consecutive sections are separated by empty
    lines.  A section always starts with a heading that defines what the
    section will contain. There are at least three sections in the output:
    ``INFO``, which contains the original input file name, the date when the
    dump was created, the number of vertices, the number of types, the
    log-likelihood and the AIC of the model, ``TYPES``, which contains the name
    and type of each vertex in a tabular format, one vertex per line, and
    ``PROBABILITIES``, which contains the *k* by *k* probability matrix. For
    degree-corrected blockmodels, ``PROBABILITIES`` is replaced by ``RATES``,
    which contains the *k* by *k* rate matrix, and there is also
    a ``STICKINESSES`` section, which contains the "stickiness" value for each
    vertex.

json
    The result is presented as a JSON dictionary with keys similar to the
    section names in the plain format (but in lowercase). The info section is
    presented as a JSON dictionary associated to the ``info`` key. The
    remaining sections are presented in JSON dictionaries associated to the
    ``parameters`` key. Within the ``parameters`` key, the types
    subkey refers to the list of vertex types (no vertex names are reported
    here), the ``probabilities`` subkey stores a list of lists that encode the
    matrix, and the ``names`` subkey contains the list of vertex names from the
    input file if you need to match them back to the type vector.
    For degree-corrected blockmodels, the ``probabilities`` subkey is replaced by
    ``rates`` (as in the plain format) and there is also a ``stickiness`` key,
    whose value reports the stickiness value of each vertex.

PROBLEMS
========

None that I am aware of.

BUGS
====

None that I am aware of.
