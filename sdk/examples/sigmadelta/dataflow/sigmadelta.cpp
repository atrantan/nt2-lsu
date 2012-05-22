//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

#define HPX_ACTION_ARGUMENT_LIMIT 10
#define HPX_FUNCTION_LIMIT 13
#define HPX_COMPONENT_CREATE_ARGUMENT_LIMIT 10 

#include <hpx/hpx.hpp>
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/util/high_resolution_timer.hpp>
#include <iostream>
#include <exception>
#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include "images.hpp"
#include <nt2/sdk/unit/perform_benchmark.hpp>
#include "sigmadelta_test.hpp"
#include "sigmadelta.hpp"
#include "pgm.hpp"


/**********************************************************************************/
/*		  Prise en compte des action par le runtime                       */
/**********************************************************************************/
HPX_REGISTER_PLAIN_ACTION(Mcompute_action);
HPX_REGISTER_PLAIN_ACTION(Dcompute_action);
HPX_REGISTER_PLAIN_ACTION(Vcompute_action);

int hpx_main(boost::program_options::variables_map& vm)
{   
   Sigmadelta_test my_test;    
    
   // Recopie des parametres initiaux
    std::size_t const h(my_test.h), w(my_test.w), Ni(my_test.Ni);
    Images* im(my_test.im);
     
   // Demarrage chronomètre
    hpx::util::high_resolution_timer t;
    
   // Lancement du Benchmark 
    std::cout << "Algorithme sigmadelta\n";
    nt2::unit::benchmark_result<nt2::details::cycles_t> dv;
    nt2::unit::perform_benchmark(my_test,10., dv);
    std::cout << "sigmadelta : " << dv.median/(double)(h*w*Ni)<< " cpe\n";

   // Fin du chronomètre
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
