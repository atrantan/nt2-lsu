################################################################################
##         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
##         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
##
##          Distributed under the Boost Software License, Version 1.0.
##                 See accompanying file LICENSE.txt or copy at
##                     http://www.boost.org/LICENSE_1_0.txt
################################################################################

#This is a temporary fix : TODO extend support to other platforms.

find_package(PkgConfig)
pkg_check_modules(NT2_HPX QUIET hpx_application)

find_package(Boost 1.48.0 QUIET COMPONENTS serialization regex filesystem program_options system thread chrono)

if(NOT NT2_HPX_FOUND OR NOT Boost_FOUND)
  set(NT2_HPX.SDK_DEPENDENCIES_FOUND 0)
endif()

set(NT2_HPX.SDK_DEPENDENCIES_INCLUDE_DIR ${NT2_HPX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
set(NT2_HPX.SDK_DEPENDENCIES_LIBRARY_DIR ${NT2_HPX_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
set(NT2_HPX.SDK_DEPENDENCIES_LIBRARIES ${NT2_HPX_LIBRARIES} hpx_component_dataflow hpx_component_iostreams hpx_serialization ${Boost_LIBRARIES})
string(REPLACE ";" " " NT2_HPX.SDK_DEPENDENCIES_COMPILE_FLAGS "${NT2_HPX_CFLAGS_OTHER}")
string(REPLACE ";" " " NT2_HPX.SDK_DEPENDENCIES_LINK_FLAGS "${NT2_HPX_LDFLAGS_OTHER}")

file(MAKE_DIRECTORY ${NT2_BINARY_DIR}/examples/frames )
file(GLOB_RECURSE NT2_SIGMADELTA_IMG frames "*.pgm") 

file(COPY ${NT2_SIGMADELTA_IMG}
     DESTINATION ${NT2_BINARY_DIR}/examples/frames
     USE_SOURCE_PERMISSIONS
     FILES_MATCHING PATTERN "*.pgm"
    )

set(NT2_HPX.SDK_DEPENDENCIES_EXTRA sdk core unit)
