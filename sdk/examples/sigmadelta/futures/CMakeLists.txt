################################################################################
##         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
##         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
##
##          Distributed under the Boost Software License, Version 1.0.
##                 See accompanying file LICENSE.txt or copy at
##                     http://www.boost.org/LICENSE_1_0.txt
################################################################################

SET(SOURCES
    sigmadelta.cpp
   )


################################################################################
# For each filename
################################################################################
FOREACH( EXAMPLE ${SOURCES})
  STRING(REGEX REPLACE ".cpp" "" EXAMPLE ${EXAMPLE})
  nt2_module_add_example(hpx.futures.${EXAMPLE}.sample ${EXAMPLE}.cpp)
  # set_target_properties(hpx.futures.${EXAMPLE}.sample PROPERTIES
  #                       COMPILE_FLAGS ${NT2_HPX_CFLAGS})
  # target_link_libraries(hpx.futures.${EXAMPLE}.sample ${NT2_HPX_LIBRARIES})
ENDFOREACH()
