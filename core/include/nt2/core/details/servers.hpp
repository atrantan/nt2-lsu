//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_CORE_DETAILS_SERVERS_HPP_INCLUDED
#define NT2_CORE_DETAILS_SERVERS_HPP_INCLUDED

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/local_lcos.hpp>

namespace nt2 { namespace server
{
  class transforms_server
    : public hpx::components::managed_component_base<transforms_server>
  {
  public:
    transforms_server() {}
    
    template<class A0, class A1>
    void nt2_nD_loopnest( A0& a0
                        , A1& a1
                        , std::size_t const& outer_begin
                        , std::size_t const& outer_end
                        );

    template<class A0, class A1>
    void nt2_1D_loopnest( A0& a0
                        , A1& a1
                        , std::size_t const& begin
                        , std::size_t const& end
                        );

    template <class A0, class A1>
    struct nt2_nD_loopnest_action
      : hpx::actions::make_action< void(transforms_server::*)(A0&,A1&,std::size_t const&,std::size_t const&)
                                 , &transforms_server::template nt2_nD_loopnest<A0,A1>
                                 , boost::mpl::false_
                                 , nt2_nD_loopnest_action<A0,A1> 
                                 > {};

    template <class A0, class A1>
    struct nt2_1D_loopnest_action
      : hpx::actions::make_action< void(transforms_server::*)(A0&,A1&,std::size_t const&,std::size_t const&)
                                 , &transforms_server::template nt2_1D_loopnest<A0,A1>
                                 , boost::mpl::false_
                                 , nt2_1D_loopnest_action<A0,A1> 
                                 > {};

  };

} }

#include <nt2/core/details/servers_impl.hpp>

HPX_SERIALIZATION_REGISTER_TEMPLATE_ACTION( (template <class A0, class A1>)
                                          , (nt2::server::transforms_server::nt2_nD_loopnest_action<A0,A1>)
                                          )

HPX_SERIALIZATION_REGISTER_TEMPLATE_ACTION( (template <class A0, class A1>)
                                          , (nt2::server::transforms_server::nt2_1D_loopnest_action<A0,A1>)
                                          )


#endif
