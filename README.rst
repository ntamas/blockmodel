==========
blockmodel
==========
----------------------------------------------------
Fitting stochastic blockmodels to empirical networks
----------------------------------------------------

:Author: Tamas Nepusz
:Version: 0.1
:License: GPL

This repository contains ``block-fit``, ``block-gen`` and ``block-pred``,
a suite of three programs to work with stochastic blockmodels (both
degree-corrected and standard ones). ``block-fit`` fits a standard or
degree-corrected blockmodel to a given graph, ``block-gen`` generates graphs
from a fitted model and ``block-pred`` calculates the probability of existence
for each possible edge in a grpah from a fitted stochastic blockmodel. More
details about the usage of each program are to be found in the ``doc``
subfolder. Please also read the references [1]_ [2]_ [3]_ [4]_ if you are
interested in how these models work.

Precompiled binaries
====================

Sorry, we do not provide precompiled binaries yet - you have to compile the
tools on your own.

Compiling from source code
==========================

Requirements
------------

- igraph_ 0.7.1 or later. This is the library that we use to work with graphs.

- CMake_ to generate the makefiles (or the project file if you are using
  Visual Studio).

.. _igraph: http://igraph.org
.. _CMake: http://www.cmake.org

Compiling using ``cmake`` and ``make``
--------------------------------------

These instructions are for Linux or Mac OS X and assume that igraph_ is
installed in a way that CMake can figure out automatically where it is.
(This usually involves using ``pkg-config``; if you run ``pkg-config --cflags igraph``
and it works, then it should work with CMake as well)::

    $ git submodule update --init
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

The first command is required only after you have checked out the source code
from GitHub for the first time. The command fetches the source code of the
C++ interface of igraph_ from GitHub and adds it to the source tree.

Bugs, questions?
================

Have you found a bug in the code? Do you have questions? Let me know.
I think you are smart enough to figure out my email address by googling
for my name. Or just drop me a message on GitHub.

References
==========

.. [1] Snijders TAB, Nowicki K (1997) Estimation and prediction for stochastic
       blockmodels for graphs with latent block structure. *J Classif*
       **14**:75-100.

.. [2] Nepusz T, Négyessy L, Tusnády G, Bazsó F (2008) Reconstructing cortical
       networks: case of directed graphs with high level of reciprocity. In:
       Bollobás B, Kozma R, Miklós D, editors, Handbook of Large-Scale Random
       Networks, Springer, volume 18 of *Bolyai Society Mathematical Studies*,
       pp. 325-368.

.. [3] Karrer B, Newman MEJ (2011) Stochastic blockmodels and community
       structure in networks. *Phys Rev E* **83**:016107.

.. [4] Nepusz T, Paccanaro A (2013) De-noising protein-protein interaction
       networks with random graph models. In preparation.
