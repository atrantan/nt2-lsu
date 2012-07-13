//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#define NT2_UNIT_MODULE "nt2 HPX module - jacobi small bench"

#include <nt2/core/container/table/table.hpp>
#include <nt2/include/functions/of_size.hpp>
#include <nt2/toolbox/operator/operator.hpp>
#include <nt2/include/functions/function.hpp>
#include <nt2/include/functions/cos.hpp>
#include <nt2/include/functions/fast_cos.hpp>
#include <nt2/include/functions/sin.hpp>
#include <nt2/include/functions/fast_sin.hpp>
#include <nt2/include/functions/sqrt.hpp>
#include <nt2/include/functions/repnum.hpp>

#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
  std::size_t size = atoi(argv[1]);
  nt2::container::table<float, nt2::_2D> new_(nt2::of_size(size,size));
  nt2::container::table<float, nt2::_2D> old_(nt2::of_size(size,size));

  old_ = nt2::repnum(float(16.0), nt2::of_size(size,size));

  new_ = nt2::sqrt(old_);

  return 0;
}
