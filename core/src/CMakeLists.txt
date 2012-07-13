################################################################################
##         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
##         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
##
##          Distributed under the Boost Software License, Version 1.0.
##                 See accompanying file LICENSE.txt or copy at
##                     http://www.boost.org/LICENSE_1_0.txt
################################################################################

include(NT2Module)
nt2_module_source_setup(hpx.core)
nt2_module_configure_toolbox(hpx 1)

set(SOURCES nt2_hpx_components.cpp)

nt2_module_add_library(nt2_hpx_components ${SOURCES})