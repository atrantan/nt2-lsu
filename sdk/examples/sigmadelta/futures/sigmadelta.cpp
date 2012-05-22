////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2012 Antoine TRAN TAN
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx.hpp>
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/util/high_resolution_timer.hpp>
#include <nt2/sdk/unit/perform_benchmark.hpp>
#include <iostream>
#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include "sigmadelta_test.hpp"

typedef nt2::test::get_sigmadelta_action<boost::uint8_t>::type sigmadelta_action;

HPX_REGISTER_PLAIN_ACTION(sigmadelta_action);

int hpx_main(boost::program_options::variables_map& vm)
{   
  nt2::test::sigmadelta_test<boost::uint8_t> my_test;    
  hpx::util::high_resolution_timer t;
  std::cout << "Sigma-Delta Algorithm\n";
  nt2::unit::benchmark_result<nt2::details::cycles_t> dv;
  nt2::unit::perform_benchmark(my_test,10., dv);
  std::cout << "sigmadelta : " << dv.median/(double)(my_test.h*my_test.w*my_test.Ni)<< " cpe\n";
  char const* fmt = "Elapsed time: %1% [s]\n";
  std::cout << (boost::format(fmt) % t.elapsed());
  return hpx::finalize(); // Handles HPX shutdown
}


int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
