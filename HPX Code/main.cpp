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

#include <iostream>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#include "benchmark.hpp"
#include "hpx_gmres_test.h"

using namespace details;

int hpx_main(boost::program_options::variables_map& vm)
{     
    std::size_t m		= vm["m"].as<std::size_t>();
    std::size_t max_it		= vm["max_it"].as<std::size_t>();
    std::size_t Nblocs		= vm["Nblocs"].as<std::size_t>();    
    std::string Mfilename	= vm["Mfilename"].as<std::string>();
    bool   verify	= vm["verify"].as<bool>();
    
    // Context creation    	
    hpx_gmres_test my_test(m,max_it,Nblocs,Mfilename,verify);
    
    if(!verify)
    {
    // Perform benchmark
     details::benchmark_result<cycles_t> dv;
     details::perform_benchmark(my_test,1., dv);
     std::cout <<hpx::get_os_thread_count()<<" "<<dv.median*1e-6<<"e+06"<<std::endl;
    }
    
    else
    {
        my_test();
        my_test();
    }
    
    return hpx::finalize(); // Handles HPX shutdown
}


int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");
    
    desc_commandline.add_options()
    ( "m"
    , boost::program_options::value<std::size_t>()->default_value(5)
    , "number of iterations between restarts")
    ;
    desc_commandline.add_options()
    ( "max_it"
    , boost::program_options::value<std::size_t>()->default_value(4)
    , "maximum number of iterations")
    ;
    desc_commandline.add_options()
    ( "Nblocs"
    , boost::program_options::value<std::size_t>()->default_value(5)
    , "number of blocks for tiling")
    ;
    desc_commandline.add_options()
    ( "Mfilename"
    , boost::program_options::value<std::string>()->default_value("")
    , "filename of Matrix to use")
    ;
    desc_commandline.add_options()
    ( "verify"
     , boost::program_options::value<bool>()->default_value(0)
     , "flag to enable/disable test mode")
    ;


    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
