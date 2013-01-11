============
 block-pred
============

---------------------------------------------------------------
Predicts edge probabilities from a fitted stochastic blockmodel
---------------------------------------------------------------

:Author: Tamas Nepusz <tamas@cs.rhul.ac.uk>
:Date: @DATE@
:License: GPL
:Version: 0.1
:Manual section: 1
:Manual group: scientific tools

SYNOPSIS
========

  block-pred [options] inputfile

DESCRIPTION
===========

block-pred reads a fitted stochastic blockmodel produced by block-fit and
calculates the probability of each possible edge in the underlying graph
according to the fitted model. It also has the capability of sampling several
possible stochastic models starting from the fitted model and predict according
to the consensus (i.e. average) of the sampled models. See the documentation of
block-fit(1) for more details on the input format.

OPTIONS
=======

-h, --help            Shows a help message and exits
-q, --quiet           Quiet mode (less output)
-v, --verbose         Verbose mode (more output)
-V, --version         Shows the version number and exits

-o FILE, --output FILE
                      Write the results to the given *FILE*. If no output
                      file is given, the default is the standard output.

-c N, --count N       Sets the number of samples to be taken from the Markov
                      chain. The default is 1, indicating that only the initial
                      model will be used.

-f FORMAT, --input-format FORMAT
                      Sets the format of the input file. The default value is
                      **plain**, which is a simple plain text format. The JSON
                      format is currently not supported.

-s, --sort            Sorts the output in decreasing order of predicted
                      probabilities.

--model MODEL         Selects the model to be used. The following options are
                      available:

                      uncorrected
                        Standard undirected blockmodel. This model aims to
                        keep the expected number of edges.

                      degree
                        Degree-weighted undirected blockmodel. This model aims
                        to keep both the expected number of edges and the
                        expected degree of each vertex.

--log-period COUNT    Show a status message after every COUNT steps

--name-mapping FILE   Reads vertex names from the given FILE (one for each
                      line) and uses these names instead of the vertex indices
                      in the output.

--sampling-freq P     Take a sample from the Markov chain at every step with
                      probability P. Smaller P values decorrelate the Markov
                      chain at the expense of larger sampling time. The default
                      is 0.1, meaning that a sample is taken after every 10
                      steps on average.

--seed SEED           Use the given number to seed the random number generator.

PROBLEMS
========

None that I am aware of.

BUGS
====

None that I am aware of.
