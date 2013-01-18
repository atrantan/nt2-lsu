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
    
    // Context creation    	
    hpx_gmres_test my_test(m,max_it,Nblocs,Mfilename);      

    // Perform benchmark
     details::benchmark_result<cycles_t> dv;
     details::perform_benchmark(my_test,1., dv);
     std::cout <<hpx::get_os_thread_count()<<" "<<dv.median*1e-6<<"e+06"<<std::endl;    
    
//    Matrix<double> &V (my_test.p->V);
//    Matrix<double> &H (my_test.p->H);
//    std::vector<double> &x (my_test.p->x);
//
//     for(std::size_t it=0; it<2; it++)
//     {     
//    my_test();
//    my_test();
//
//    std::cout << "Matrice H\n";
//    for (std::size_t i=0;i<m+1;i++)
//    {
//     for (std::size_t j=0;j<m;j++)
//     std::cout<<H(j,i)<<" ";
//     std::cout<<std::endl;
//    }
//    
//    std::cout<<std::endl;
//    
//    std::cout << "Matrice V\n";
//    for (std::size_t i=0;i<V.width;i++)
//    {
//     for (std::size_t j=0;j<m;j++)
//     std::cout<<V(j,i)<<" ";
//     std::cout<<std::endl;
//    }
//   std::cout<<std::endl;  
//   
//    std::cout << "Solution x\n";
//    for (std::size_t i=0;i<x.size();i++)
//    {
//     std::cout<<x[i];
//     std::cout<<std::endl;
//    }
//
//   std::cout<<std::endl; 
//   }
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
    , "m for the GSArnoldi function")
    ;
    desc_commandline.add_options()
    ( "max_it"
    , boost::program_options::value<std::size_t>()->default_value(4)
    , "max_it for the Gmres algorithm")
    ;
    desc_commandline.add_options()
    ( "Nblocs"
    , boost::program_options::value<std::size_t>()->default_value(5)
    , "Nblocs for the GSArnoldi function")
    ;
    desc_commandline.add_options()
    ( "Mfilename"
    , boost::program_options::value<std::string>()->default_value("")
    , "Mfilename for Gmres algorithm")
    ;


    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
