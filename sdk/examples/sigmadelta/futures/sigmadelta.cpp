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
#include "images.hpp"
#include "sigmadelta_test.hpp"
#include "sigmadelta.hpp"
#include "pgm.hpp"

HPX_REGISTER_PLAIN_ACTION(sigmadelta_action);

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

   // Affichage des images fond 
    for (std::size_t n=0; n<Ni; n++)
    { 	  
      std::ostringstream nom_fichier;	
      nom_fichier<<"./frame_fond/fond."<<std::setfill('0')<<std::setw(3)<<n<<".pgm";      
            
      Pgm Fond(w,h,im->Fond[n].data.begin());
      Fond.write(nom_fichier.str()); 	  
    }
 
  // Affichage des images estimee 
    for (std::size_t n=0; n<Ni; n++)
    {   
      std::ostringstream nom_fichier;	
      nom_fichier<<"./frame_estime/estime."<<std::setfill('0')<<std::setw(3)<<n<<".pgm";      
      
      Pgm Estimee(w,h,im->Estimee[n].data.begin());
      Estimee.write(nom_fichier.str());   
    }
     
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
