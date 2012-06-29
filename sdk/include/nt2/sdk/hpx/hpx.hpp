//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_HPX_HPX_HPP_INCLUDED
#define NT2_SDK_HPX_HPX_HPP_INCLUDED

#include <boost/dispatch/functor/forward.hpp>

namespace nt2 { namespace tag
  {
    template<class T>
    struct hpx_ : T
    {
      typedef T parent;
    };
  } }

BOOST_DISPATCH_COMBINE_SITE( nt2::tag::hpx_<tag::cpu_> )

////////////////////////////////////////////////////////////////////////////////
// Here we hack the entry point to end in a HPX thread from where every HPX
// based components can be called safely (every calls garanted after the 
// entire runtime is set up) 
////////////////////////////////////////////////////////////////////////////////
#include <nt2/sdk/hpx/user_main_fwd.hpp>
#define main nt2::user_main

#endif
