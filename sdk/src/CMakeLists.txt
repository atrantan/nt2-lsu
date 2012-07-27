################################################################################
##         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
##         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
##
##          Distributed under the Boost Software License, Version 1.0.
##                 See accompanying file LICENSE.txt or copy at
##                     http://www.boost.org/LICENSE_1_0.txt
################################################################################

include(NT2Module)
nt2_module_source_setup(hpx.sdk)
nt2_module_configure_toolbox(hpx 1)

set(BUILD_SHARED_LIBS FALSE)
set(SOURCES nt2_hpx.cpp command_line.cpp)
nt2_module_add_library(nt2_hpx ${SOURCES})
set(BUILD_SHARED_LIBS TRUE)
set(SOURCES_COMPONENTS nt2_cmdline_components.cpp)
nt2_module_add_library(hpx_component_granularity ${SOURCES_COMPONENTS})
set(BUILD_SHARED_LIBS FALSE)