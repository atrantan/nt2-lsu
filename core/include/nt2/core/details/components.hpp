//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_CORE_DETAILS_COMPONENTS_HPP_INCLUDED
#define NT2_CORE_DETAILS_COMPONENTS_HPP_INCLUDED

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/components.hpp>

#include <nt2/core/details/stubs.hpp>

namespace nt2{ namespace details
{
  class transform_actions
    : public hpx::components::
      client_base< transform_actions
                 , stubs::transforms_server
                 >
  {
    typedef hpx::components::
    client_base< transform_actions
               , stubs::transforms_server
               > base_type;

  public:
    transform_actions()
    {}

    transform_actions(hpx::naming::id_type const& gid)
      : base_type(gid)
    {}

    template<class A0, class A1>
    void nt2_nD_loopnest_async( A0& a0
                              , A1& a1
                              , std::size_t const& outer_begin
                              , std::size_t const& outer_end
                              )
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::nt2_nD_loopnest_async(this->gid_, a0, a1, outer_begin, outer_end);
    }

    template<class A0, class A1>
    void nt2_1D_loopnest_async( A0& a0
                              , A1& a1
                              , std::size_t const& begin
                              , std::size_t const& end
                              )
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::nt2_1D_loopnest_async(this->gid_, a0, a1, begin, end);
    }

    template<class A0, class A1>
    void nt2_nD_loopnest_sync( A0& a0
                              , A1& a1
                              , std::size_t const& outer_begin
                              , std::size_t const& outer_end
                              )
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::nt2_nD_loopnest_sync(this->gid_, a0, a1, outer_begin, outer_end);
    }

    template<class A0, class A1>
    void nt2_1D_loopnest_sync( A0& a0
                              , A1& a1
                              , std::size_t const& begin
                              , std::size_t const& end
                              )
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::nt2_1D_loopnest_sync(this->gid_, a0, a1, begin, end);
    }
  };

} }

#endif
