# Tries to find igraph
# Once done, this will define
#
#   IGRAPH_FOUND - system has igraph
#   IGRAPH_INCLUDE_DIRS - the include directories of igraph
#   IGRAPH_LIBRARIES - link these to use igraph

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(IGRAPH_PKGCONF igraph)

# Include directory
find_path(IGRAPH_INCLUDE_DIR
          NAMES igraph/igraph.h
          PATHS ${IGRAPH_PKGCONF_INCLUDE_DIRS})

# Finally the library itself
find_library(IGRAPH_LIBRARY
             NAMES igraph
             PATHS ${IGRAPH_PKGCONF_LIBRARY_DIRS})

# Final touches
set(igraph_PROCESS_INCLUDES IGRAPH_INCLUDE_DIR)
set(igraph_PROCESS_LIBS IGRAPH_LIBRARY)
libfind_process(igraph)
