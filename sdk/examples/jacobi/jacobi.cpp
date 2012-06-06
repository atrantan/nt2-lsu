//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

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

namespace nt2{ namespace log 
{
  template<class T, class S, class Dummy>
  void print_table( table<T,S,Dummy> const& t
                  , std::size_t      const& h
                  , std::size_t      const& w);
} }

namespace nt2{ namespace example 
{
  template<class T>
  struct diffusion
  {
    typedef T s_t;
    
    diffusion(std::size_t const& s, std::size_t const& iter)
      : size_(s), iterations_(iter)
    {
      new_.resize(of_size(size_,size_));
      old_.resize(of_size(size_,size_));
      
      // Initialization of the first matrix
      for(std::size_t i = 1; i<=size_; ++i)
        for(std::size_t j = 1; j<=size_; ++j)
          old_(i,j) = s_t(i+j); 
      
#ifdef NT2_JACOBI_LOGGING
      nt2::log::print_table(old_, size_, size_);
#endif
    }
 
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
        nt2::log::print_table(new_, size_, size_);
#endif
        
        boost::swap(new_,old_);
      }
    }
   
    std::size_t size_;
    std::size_t iterations_;
    table<s_t> new_;
    table<s_t> old_;
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
  nt2::unit::perform_benchmark(nt2::example::diffusion<s_t>(size, iterations), 5., dv);
  std::cout << "Diffusion Equation :\n\t";
  std::cout << std::scientific << dv.median/(double)(size*size*iterations) << "\n";

  return 0;
}

// Logging facility
namespace nt2{ namespace log 
{
  template<class T, class S, class Dummy>
  void print_table( table<T,S,Dummy> const& t
                  , std::size_t      const& h
                  , std::size_t      const& w)
  {
    std::cout << "==================================\n";
    for(std::size_t i = 1; i<=h; ++i)
    {
      for(std::size_t j = 1; j<=w; ++j)
      {
        std::cout << t(i,j) << " ";
      }
      std::cout << "\n";
    }
    std::cout << "==================================\n";
  }
} }
