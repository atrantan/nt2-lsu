//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_CORE_DETAILS_STUBS_HPP_INCLUDED
#define NT2_CORE_DETAILS_STUBS_HPP_INCLUDED

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/applier.hpp>
#include <hpx/include/async.hpp>

#include <nt2/core/details/servers.hpp>

namespace nt2 { namespace stubs 
{ 
  struct transforms_server
    : hpx::components::stub_base<nt2::server::transforms_server>
  {
    template<class A0, class A1>
    static void nt2_nD_loopnest_async( hpx::naming::id_type const& gid
                                     , A0& a0
                                     , A1& a1
                                     , std::size_t const& outer_begin
                                     , std::size_t const& outer_end
                                     )
    {
      typedef typename server::transforms_server::nt2_nD_loopnest_action<A0,A1> action_type;
      hpx::apply<action_type>(gid, a0, a1, outer_begin, outer_end);
    }

    template<class A0, class A1>
    static void nt2_1D_loopnest_async( hpx::naming::id_type const& gid
                                     , A0& a0
                                     , A1& a1
                                     , std::size_t const& begin
                                     , std::size_t const& end
                                     )
    {
      typedef server::transforms_server::nt2_1D_loopnest_action<A0,A1> action_type;
      hpx::apply<action_type>(gid, a0, a1, begin, end);
    }

    template<class A0, class A1>
    static void nt2_nD_loopnest_sync( hpx::naming::id_type const& gid
                                     , A0& a0
                                     , A1& a1
                                     , std::size_t const& outer_begin
                                     , std::size_t const& outer_end
                                     )
    {
      typedef server::transforms_server::nt2_nD_loopnest_action<A0,A1> action_type;
      hpx::async<action_type>(gid, a0, a1, outer_begin, outer_end).get;
    }

    template<class A0, class A1>
    static void nt2_1D_loopnest_sync( hpx::naming::id_type const& gid
                                     , A0& a0
                                     , A1& a1
                                     , std::size_t const& begin
                                     , std::size_t const& end
                                     )
    {
      typedef server::transforms_server::nt2_1D_loopnest_action<A0,A1> action_type;
      hpx::async<action_type>(gid, a0, a1, begin, end).get();
    }
  };

} }

#endif
