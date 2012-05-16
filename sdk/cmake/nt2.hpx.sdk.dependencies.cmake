################################################################################
##         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
##         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
##
##          Distributed under the Boost Software License, Version 1.0.
##                 See accompanying file LICENSE.txt or copy at
##                     http://www.boost.org/LICENSE_1_0.txt
################################################################################

#This is a temporary fix : TODO extend support to other platforms.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules")

include(FindHPX)

if(NOT HPX_VERSION)
  set(NT2_HPX_DEPENDENCIES_FOUND 0)
endif()

set(NT2_HPX_COMPILE_FLAGS ${HPX_FLAGS} CACHE INTERNAL "" FORCE)

set(NT2_HPX_DEPENDENCIES_EXTRA sdk core unit)