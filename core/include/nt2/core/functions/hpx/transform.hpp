//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_CORE_FUNCTIONS_HPX_TRANSFORM_HPP_INCLUDED
#define NT2_CORE_FUNCTIONS_HPX_TRANSFORM_HPP_INCLUDED

#include <hpx/hpx.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/applier.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/local_lcos.hpp>

#include <nt2/include/functions/run.hpp>
#include <nt2/sdk/config/cache.hpp>
#include <nt2/sdk/hpx/details/granularity.hpp>
#include <nt2/include/functions/numel.hpp>

#include <boost/simd/sdk/simd/native.hpp>
#include <boost/fusion/include/at.hpp>

namespace nt2 { namespace server
{
  class transforms_server
    : public hpx::components::managed_component_base<transforms_server>
  {
  public:
    transforms_server() {}
    
    template<class Site,class A0, class A1>
    void nt2_transformer( A0& a0
                        , A1& a1
                        , std::size_t const& it
                        , std::size_t const& in
                        , std::size_t const& out
                        )
    {
      nt2::functor<tag::transform_,Site> transformer;
      
      // External and internal size
      std::ptrdiff_t inner = in;
      std::ptrdiff_t outer = out;
      
      // Current number of threads
      std::ptrdiff_t threads = stubs::granularity::get_sync(hpx::find_here());
      std::ptrdiff_t p = hpx::get_worker_thread_num();

      // A table is "thin" if its outer dimension is at least twice as small
      // as its inner dimension or less than available threads
      // TODO: see if we shouldn't be less dogmatic here
      bool  is_thin( (inner > 2*outer) || (threads > outer));
      
      // How many inner block to process & dispatch w/r to "thinness"
      std::ptrdiff_t inner_block    = inner / (is_thin ? threads : 1);
      std::ptrdiff_t inner_leftover = inner % (is_thin ? threads : 1);
      
      // How many outer block to process & dispatch w/r to "thinness"
      std::ptrdiff_t outer_block    = outer / (is_thin ? 1 : threads);
      std::ptrdiff_t outer_leftover = outer % (is_thin ? 1 : threads);

      // Perform load balance of remaining elements per threads
      std::ptrdiff_t c_outer = outer_block*p + std::min(outer_leftover,p);
      std::ptrdiff_t c_inner = inner_block*p + std::min(inner_leftover,p);
      
      outer_block += outer_leftover ? ((outer_leftover > p) ? 1 : 0) : 0;
      inner_block += inner_leftover ? ((inner_leftover > p) ? 1 : 0) : 0;
        
      // Move forward starts of each block
      std::size_t i = it + (is_thin ? c_inner*outer : c_outer*inner);
      
      // Call transform over the sub-architecture in the memory hierachy
      transformer(a0,a1,i,inner_block,outer_block);
    }

    template <class Site, class A0, class A1>
    struct nt2_transformer_action
      : hpx::actions::action5< transforms_server
                 , hpx::actions::component_action_arg5
                 , A0&, A1&, std::size_t const&, std::size_t const&, std::size_t const&
                 , &transforms_server::template nt2_transformer<Site,A0,A1>
                 , hpx::threads::thread_priority_default
                 , nt2_transformer_action<Site,A0,A1> 
                 > {};

  };

} }

HPX_REGISTER_ACTION_DECLARATION_TEMPLATE( (template <class Site, class A0, class A1>)
                                          , (nt2::server::transforms_server::nt2_transformer_action<Site,A0,A1>)
                                          )

namespace nt2 { namespace stubs 
{ 
  struct transforms_server
    : hpx::components::stub_base<nt2::server::transforms_server>
  {
    template<class Site, class A0, class A1>
    static void nt2_transformer_async( hpx::naming::id_type const& gid
                                     , A0& a0
                                     , A1& a1
                                     , std::size_t const& it
                                     , std::size_t const& in
                                     , std::size_t const& out
                                     )
    {
      typedef server::transforms_server::nt2_transformer_action<Site,A0,A1> action_type;
      hpx::apply<action_type>(gid, a0, a1, it, in, out);
    }

    template<class Site, class A0, class A1>
    static void nt2_transformer_sync( hpx::naming::id_type const& gid
                                     , A0& a0
                                     , A1& a1
                                     , std::size_t const& it
                                     , std::size_t const& in
                                     , std::size_t const& out
                                     )
    {
      typedef server::transforms_server::nt2_transformer_action<Site,A0,A1> action_type;
      hpx::async<action_type>(gid, a0, a1, it, in, out).get;
    }
  };

} }


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

    template<class Site, class A0, class A1>
    void nt2_transformer_async( A0& a0
                              , A1& a1
                              , std::size_t const& it
                              , std::size_t const& in
                              , std::size_t const& out
                              )
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::nt2_transformer_async<Site>(this->gid_, a0, a1, it, in, out);
    }

    template<class Site, class A0, class A1>
    void nt2_transformer_sync( A0& a0
                             , A1& a1
                             , std::size_t const& it
                             , std::size_t const& in
                             , std::size_t const& out
                             )
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::nt2_transformer_sync<Site>(this->gid_, a0, a1, it, in, out);
    }
  };

} }

#include <nt2/core/functions/transform.hpp>
#include <nt2/sdk/functor/meta/call.hpp>
#include <nt2/sdk/hpx/hpx.hpp>

#ifndef BOOST_NO_EXCEPTIONS
#include <boost/exception_ptr.hpp>
#endif


namespace nt2 { namespace ext
{
  //============================================================================
  // Global HPX elementwise operation
  // Generates a SPMD loop nest and forward to internal site for evaluation
  // using the partial transform syntax.
  //============================================================================
  NT2_FUNCTOR_IMPLEMENTATION( nt2::tag::transform_, nt2::tag::hpx_<Site>
                            , (A0)(A1)(Site)
                            , (ast_<A0>)
                              (ast_<A1>)
                            )
  {
    typedef void result_type;
    
    BOOST_FORCEINLINE result_type
    operator()(A0& a0, A1& a1) const
    {
      typename A0::extent_type e = a0.extent();
      std::ptrdiff_t inner = boost::fusion::at_c<0>(e);
      std::ptrdiff_t outer = nt2::numel(boost::fusion::pop_front(e));
      nt2::transform(a0,a1,0,inner,outer);
    }
  };
  
  //============================================================================
  // Partial HPX elementwise operation
  // Generates a SPMD loop nest and forward to internal site for evaluation
  // using the partial transform syntax.
  //============================================================================
  NT2_FUNCTOR_IMPLEMENTATION( nt2::tag::transform_, nt2::tag::hpx_<Site>
                            , (A0)(A1)(Site)(A2)(A3)(A4)
                            , (ast_<A0>)
                              (ast_<A1>)
                              (scalar_< integer_<A2> >)
                              (scalar_< integer_<A3> >)
                              (scalar_< integer_<A4> >)
                            )
  {
    typedef void result_type;

    BOOST_FORCEINLINE result_type
    operator()(A0& a0, A1& a1, A2 it, A3 in, A4 out) const
    {
      stubs::transforms_server::nt2_transformer_async<Site>(hpx::find_here(),a0,a1,it,in,out);
    }
  };
  
} }

#endif
