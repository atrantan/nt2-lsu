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
        
   // Initialize values in hpx_gemres_test ctor
    hpx_gmres_test(std::size_t m, std::size_t Nblocs, std::string Mfilename)
    :p( new Param(m,Nblocs,Mfilename) )
    {}
    
        
   // Compute GMRES Algorithm in hpx_gemres_test functor
    void operator()();
};

#endif // SIGMADELTA_TEST_H
