#ifndef HPX_GMRES_TEST_H
#define HPX_GMRES_TEST_H

#include <string>

#include "matrix.h"
#include "spmatrix.h"
#include "param.h"
#include "GSArnoldi_forward.h"

#include <mkl_cblas.h>
#include <mkl_spblas.h>


class hpx_gmres_test
{
  public: 
      
    Param_ptr p;
    
    std::size_t it;
    
    std::vector<std::size_t> blocsize;
    std::vector<std::size_t> offset;
        
   // Initialize values in hpx_gemres_test ctor
    hpx_gmres_test(std::size_t m, std::size_t max_it, std::size_t Nblocs, std::string Mfilename)
    :p( new Param(m,max_it,Nblocs,Mfilename) ),blocsize(Nblocs),offset(Nblocs)
    {
      std::size_t N = p->N;
      
      blocsize[0] = N/Nblocs + (N%Nblocs>0);
      offset[0] = 0;
    
      for(std::size_t i=1; i<Nblocs; i++)
      {
        blocsize[i] = N/Nblocs + (N%Nblocs>i);
        offset[i] = blocsize[i-1] + offset[i-1];
      }
    }
    
   // GMRES Cycle called several times in operator()
    void gmres_cycle();
   
   // Compute GMRES Algorithm in hpx_gemres_test functor
    void operator()();
};

#endif // SIGMADELTA_TEST_H
