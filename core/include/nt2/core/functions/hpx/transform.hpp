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

#include <nt2/core/functions/transform.hpp>
#include <nt2/sdk/functor/meta/call.hpp>
#include <nt2/sdk/hpx/hpx.hpp>
#include <nt2/include/functions/run.hpp>
#include <nt2/sdk/config/cache.hpp>
#include <nt2/include/functions/numel.hpp>
#include <nt2/core/details/components.hpp>
#include <boost/simd/sdk/simd/native.hpp>
#include <boost/fusion/include/at.hpp>

#ifndef BOOST_NO_EXCEPTIONS
#include <boost/exception_ptr.hpp>
#endif

#ifndef BOOST_SIMD_NO_SIMD
//==============================================================================
// HPX + SIMD
//==============================================================================
namespace nt2 { namespace ext
{
  //============================================================================
  // nD element-wise operation
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
      std::size_t granularity = 8;
      const std::size_t outer_sz     = nt2::numel(boost::fusion::pop_front(a0.extent())); 
      const std::size_t outer_step   = outer_sz/granularity;
      const std::size_t outer_sz_bnd = outer_step*granularity;
      details::transform_actions actions(hpx::find_here());
      // Computation for regular chunks
      for(std::size_t j=0; j<outer_sz; j+=outer_step)
      {
        actions.nt2_nD_loopnest_async(a0, a1, j, j+outer_step);
      }
      // Epilogue
      actions.nt2_nD_loopnest_async(a0, a1, outer_sz_bnd, outer_sz);
    }
  };

  //============================================================================
  // 1D element-wise operation
  //============================================================================
  NT2_FUNCTOR_IMPLEMENTATION_TPL( nt2::tag::transform_, nt2::tag::hpx_<Site>
                                , (class A0)
                                  (class A1)(class T1)(class N1)
                                  (class Shape)(class StorageKind)(std::ptrdiff_t Sz)
                                  (class Site)
                                , (ast_<A0>)
                                  ((expr_< table_< unspecified_<A1>
                                                 , nt2::settings( nt2::of_size_<Sz>
                                                                , Shape
                                                                , StorageKind
                                                                )
                                                 >
                                         , T1, N1
                                         >
                                   ))
                                 )
  {
    typedef void result_type;

    typedef typename meta::
    strip< typename meta::scalar_of<A0>::type >::type stype;
    
    BOOST_FORCEINLINE result_type
    operator()(A0& a0, A1& a1) const
    {
      std::size_t granularity = 8;
      std::size_t bound = boost::fusion::at_c<0>(a0.extent());
      const std::size_t aligned_step = bound/granularity;
      const std::size_t aligned_bound = aligned_step*granularity;
      details::transform_actions actions(hpx::find_here());
      // Process all simd chunks
      for(std::size_t i=0; i<aligned_bound; i+=aligned_step)
      {
        actions.nt2_1D_loopnest_async(a0, a1, i, i+aligned_step);
      }
      // Process the scalar epilogue
      for(std::size_t i=aligned_bound; i!=bound; ++i)
        nt2::run(a0, i, nt2::run(a1, i, meta::as_<stype>()));
    }
  };

} }

#else

//==============================================================================
// HPX + no SIMD
//==============================================================================
namespace nt2 { namespace details {

  // template<class A0, class A1, class Target>
  // int nt2_hpx_action_1D_no_simd( A0& a0
  //                              , A1& a1
  //                              , std::size_t const& begin
  //                              , std::size_t const& end
  //                              )
  // {
  //   typedef typename meta::
  //     strip< typename meta::
  //     scalar_of<A0>::type
  //            >::type                                    target_type;

  //   // Process one scalar chunk
  //   for(std::size_t i=begin; i<end; ++i)
  //   {
  //     nt2::run(a0, i, nt2::run(a1, i, meta::as_<target_type>()));
  //   }

  //   return 0;
  // }

} }

namespace nt2 { namespace ext
{
  //============================================================================
  // Element-wise operation
  //============================================================================
  NT2_FUNCTOR_IMPLEMENTATION( nt2::tag::transform_, nt2::tag::hpx_<Site>
                            , (A0)(A1)(Site)
                            , (ast_<A0>)
                              (ast_<A1>)
                            )
  {
    typedef void                                        result_type;

    typedef typename meta::
      strip< typename meta::
             scalar_of<A0>::type
             >::type                                    target_type;

    BOOST_FORCEINLINE result_type
    operator()(A0& a0, A1& a1) const
    {
      std::size_t granularity = 8;
      std::size_t bound = boost::fusion::at_c<0>(a0.extent());
      const std::size_t aligned_step = bound/granularity;
      const std::size_t aligned_bound = aligned_step*granularity;
      // rtype r;

      // for(std::size_t i=0; i<aligned_bound; i+=aligned_step)
      // {
      //   r = nt2_1D_no_simd_async(hpx::find_here(), a0, a1, i, i+aligned_step);
      // }
      // // Process the epilogue
      // for(std::size_t i=aligned_bound; i!=bound; ++i)
      //   nt2::run(a0, i, nt2::run(a1, i, meta::as_<stype>()));
      
    }
  };

} }

#endif

#endif
