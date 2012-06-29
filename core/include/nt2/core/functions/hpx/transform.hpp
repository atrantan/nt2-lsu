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
#include <nt2/sdk/hpx/hpx.hpp>
#include <nt2/include/functions/run.hpp>
#include <nt2/sdk/config/cache.hpp>
#include <nt2/include/functions/numel.hpp>
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
    typedef void                                        result_type;
    
    typedef typename meta::
      strip< typename meta::
             scalar_of<A0>::type
             >::type                                    stype;
    
    typedef boost::simd::native<stype, BOOST_SIMD_DEFAULT_EXTENSION>
    target_type;
  
    BOOST_FORCEINLINE result_type
    operator()(A0& a0, A1& a1) const
    {

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
    typedef void                                        result_type;
    
    typedef typename meta::
      strip< typename meta::
             scalar_of<A0>::type
             >::type                                    stype;
    
    typedef boost::simd::native<stype, BOOST_SIMD_DEFAULT_EXTENSION>
    target_type;
    
    BOOST_FORCEINLINE result_type
    operator()(A0& a0, A1& a1) const
    {
 
    }
  };

} }

#else

//==============================================================================
// HPX + no SIMD
//==============================================================================
namespace nt2 { namespace ext
{
  //============================================================================
  // Element-wise operation
  //============================================================================
  NT2_FUNCTOR_IMPLEMENTATION( nt2::tag::transform_, nt2::tag::openmp_<Site>
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
 
    }
  };

} }

#endif

#endif
