//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#define NT2_UNIT_MODULE "nt2 HPX main - test_main"

#include <nt2/table.hpp>
#include <nt2/core/utility/of_size.hpp>
#include <nt2/include/functions/repnum.hpp>
#include <nt2/sdk/unit/perform_benchmark.hpp>
#include <boost/swap.hpp>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iostream>

//#define NT2_JACOBI_LOGGING

using namespace nt2;

namespace nt2{ namespace example 
{
  //============================================================================
  // Simd version
  //===========================================================================
  template<class T>
  struct diffusion_simd
  {
    typedef T s_t;
    
    diffusion_simd(std::size_t const& s, std::size_t const& iter)
      : size_(s), iterations_(iter)
    {
      new_.resize(of_size(size_,size_));
      old_.resize(of_size(size_,size_));
      
      // Initialization of the first matrix
      for(std::size_t i = 1; i<=size_; ++i)
        for(std::size_t j = 1; j<=size_; ++j)
          old_(i,j) = s_t(1); 
      
#ifdef NT2_JACOBI_LOGGING
      std::cout << old_ << std::endl;
#endif
    }
 
    BOOST_FORCEINLINE
    void operator()()
    {
      // Computation of the equation
      for(std::size_t i = 0; i<iterations_; ++i)
      {   
        new_(_(begin_+1,end_-1),_(begin_+1,end_-1)) 
          = ( old_(_(begin_,end_-2),_(begin_+1,end_-1))
            + old_(_(begin_+2,end_),_(begin_+1,end_-1))
            + old_(_(begin_+1,end_-1),_(begin_,end_-2))
            + old_(_(begin_+1,end_-1),_(begin_+2,end_))
            )/s_t(4);
        
#ifdef NT2_JACOBI_LOGGING
        std::cout << new_ << std::endl;
#endif
        
        boost::swap(new_,old_);
      }
    }
   
    std::size_t size_;
    std::size_t iterations_;
    table<s_t,_2D> new_;
    table<s_t,_2D> old_;
  };

  //============================================================================
  // Scalar version
  //===========================================================================
  template<class T>
  struct diffusion_scalar
  {
    typedef T s_t;
    
    diffusion_scalar(std::size_t const& s, std::size_t const& iter)
      : size_(s), iterations_(iter)
    {
      new_.resize(of_size(size_,size_));
      old_.resize(of_size(size_,size_));
      
      // Initialization of the first matrix
      for(std::size_t i = 1; i<=size_; ++i)
        for(std::size_t j = 1; j<=size_; ++j)
          old_(i,j) = s_t(1); 
      
#ifdef NT2_JACOBI_LOGGING
      std::cout << old_ << std::endl;
#endif
    }
 
    BOOST_FORCEINLINE
    void operator()()
    {
      // Computation of the equation
      for(std::size_t i = 0; i<iterations_; ++i)
      {
        for(std::size_t i = 2; i<size_; ++i)
          for(std::size_t j = 2; j<size_; ++j)
          {  
            new_(i,j) 
              = ( old_(i+1,j)
                + old_(i-1,j)
                + old_(i,j+1)
                + old_(i,j-1)
                )/s_t(4);
          }
#ifdef NT2_JACOBI_LOGGING
        std::cout << new_ << std::endl;
#endif
        
        boost::swap(new_,old_);
      }
    }
   
    std::size_t size_;
    std::size_t iterations_;
    table<s_t,_2D> new_;
    table<s_t,_2D> old_;
  };

} }

//==============================
// Main
//==============================
int main(int argc, char* argv[])
{
  std::size_t size = atoi(argv[1]);
  std::size_t iterations = atoi(argv[2]);

  typedef float s_t;

  nt2::unit::benchmark_result<nt2::details::cycles_t> dv;
  nt2::unit::perform_benchmark(nt2::example::diffusion_simd<s_t>(size, iterations), 5., dv);
  std::cout << "Diffusion Equation Simd :\n\t";
  std::cout << dv.median/(double)(size*size*iterations) << "\n";

  nt2::unit::perform_benchmark(nt2::example::diffusion_scalar<s_t>(size, iterations), 5., dv);
  std::cout << "Diffusion Equation Scalar :\n\t";
  std::cout << dv.median/(double)(size*size*iterations) << "\n";

  return 0;
}

