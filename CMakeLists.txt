cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0003 NEW)

#####################################################################
# General project properties
#####################################################################

project(blockmodel C CXX)
enable_testing()

#####################################################################
# Build options
#####################################################################

#####################################################################
# Version information
#####################################################################

set(BLOCKMODEL_VERSION_MAJOR 1)
set(BLOCKMODEL_VERSION_MINOR 0)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/block/version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/block/version.h)

#####################################################################
# Tweaking CMake
#####################################################################

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake;${CMAKE_CURRENT_SOURCE_DIR}/vendor/igraphpp/etc/cmake;${CMAKE_MODULE_PATH})

#####################################################################
# Find dependencies
#####################################################################

find_package(igraph REQUIRED)

#####################################################################
# Compiler flags for different build configurations
#####################################################################

set(CMAKE_C_FLAGS   "${CMAKE_ARCH_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_ARCH_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_ARCH_FLAGS} -O0 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_ARCH_FLAGS} -O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_ARCH_FLAGS} -O3 -DNDEBUG -Wall -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_ARCH_FLAGS} -O3 -DNDEBUG -Wall -s")
set(CMAKE_C_FLAGS_PROFILING "${CMAKE_ARCH_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_PROFILING "${CMAKE_ARCH_FLAGS} -pg")

include_directories(${igraph_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/igraphpp/include)

#####################################################################
# Process subdirectories
#####################################################################

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(vendor)

#####################################################################
# Add 'dist' target to archive everything with git
#####################################################################

set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${BLOCKMODEL_VERSION_MAJOR}.${BLOCKMODEL_VERSION_MINOR})
add_custom_target(dist
	COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD | gzip >${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

#####################################################################
# Add 'release' target to make a release automatically
#####################################################################

add_custom_target(release
	COMMAND etc/scripts/release.sh
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
