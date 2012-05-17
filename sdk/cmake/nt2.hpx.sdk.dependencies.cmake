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

if(NOT NT2_HPX_FOUND)
  set(NT2_HPX.SDK_DEPENDENCIES_FOUND 0)
endif()

set(NT2_HPX.SDK_DEPENDENCIES_INCLUDE_DIR ${NT2_HPX_INCLUDE_DIRS})
set(NT2_HPX.SDK_DEPENDENCIES_LIBRARIES ${NT2_HPX_LIBRARIES} hpx_component_dataflow hpx_component_iostreams hpx_serialization)
string(REPLACE ";" " " NT2_HPX.SDK_DEPENDENCIES_COMPILE_FLAGS "${NT2_HPX_CFLAGS}")
string(REPLACE ";" " " NT2_HPX.SDK_DEPENDENCIES_LINK_FLAGS "${NT2_HPX_LDFLAGS}")

set(NT2_HPX.SDK_DEPENDENCIES_EXTRA sdk core unit)
