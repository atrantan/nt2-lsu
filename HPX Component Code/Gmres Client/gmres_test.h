#ifndef GEMRES_TEST_H
#define GEMRES_TEST_H

#include <iterator>

#include "gmres_component.hpp"
#include "matrix.h"

extern "C" {
#include <bebop/smc/sparse_matrix.h>
#include <bebop/smc/sparse_matrix_ops.h>
#include <bebop/smc/csr_matrix.h>
}

#include <mkl_cblas.h>
#include <mkl_spblas.h>


struct gmres_test
{    
  // Initialize values in hpx_gmres_test ctor
    gmres_test(std::size_t m_=1, std::size_t max_it_=1, std::size_t Nblocs_=1, 
           std::string Mfilename="", std::size_t Locs_=1):
    m(m_),Nblocs(Nblocs_),max_it(max_it_),num_locs(Locs_),
    tol(1e-6),rho(1.0),localities(hpx::find_all_localities()), 
    c(m+1),s(m+1),g(m+1,0.),blocsize(num_locs),offset(num_locs),
    H(m,m+1)
    {
        
    p.reserve(num_locs);
       
    // Initialization of remote contexts
    for (std::size_t i=0; i<num_locs; i++)
    {
      p.push_back(gmres());
      p.back().create( hpx::find_here() );
    }
         
    struct sparse_matrix_t* At = nullptr;
    struct csr_matrix_t * ptr = nullptr;

    At = load_sparse_matrix(MATRIX_MARKET,Mfilename.c_str());
    if (At == NULL)
    {
        std::cout<<"File not found\n";
        exit(EXIT_FAILURE);
    }
    sparse_matrix_convert(At,CSR);

    ptr = (struct csr_matrix_t*) (At->repr);

    // Write dimension N
    N = ptr->n;
       
    std::map<std::string,std::size_t> Par = { {"m",m}, {"N",N}, {"Nblocs",Nblocs} };
    
    Vk.resize(N);
    x.resize(N);
        
    // Initialization x = x0
    x[0]=1.0;
      
    double dotr0 = 0.;
        
    blocsize[0] = N/num_locs + (N%num_locs>0);
    offset[0]   = 0;
    for(std::size_t i=1; i<num_locs; i++)
    {
        offset[i]    = blocsize[i-1] + offset[i-1];
        blocsize[i]  = N/num_locs + (N%num_locs>i);
    }
        
    int * rows       = ptr->rowptr;
    double * values  = (double *)(ptr->values);
    int * indices    = ptr->colidx;
        
    std::size_t Nlocal, imin;
        
    for (std::size_t i=0; i<num_locs; i++)
    {
        Nlocal = blocsize[i];
        imin   = offset[i];
        
        Par["Nlocal"] = Nlocal;
        Par["imin"]   = imin;
		
        dotr0+= p[i].init(Par,
                          std::vector<int>(&rows[imin],&rows[imin+Nlocal+1]),
                          std::vector<double>(&values[ rows[imin] ],&values[ rows[imin+Nlocal] ]),
                          std::vector<int>(&indices[ rows[imin] ],&indices[ rows[imin+Nlocal] ])                          
                          );
	
    }      
    dotr0 = sqrt(dotr0);
   
     // Normalization of the tolerance
    tol = tol*dotr0;
    
    destroy_sparse_matrix(At);
    }

   // GMRES Cycle called several times in operator()
    void gmres_cycle();
   
   // Compute GMRES Algorithm in hpx_gemres_test functor
    void operator()();

 // Members
    std::vector<gmres> p;    
    std::size_t m, N, Nblocs, max_it, num_locs, it;
    double tol,rho;    
    std::vector<hpx::naming::id_type> localities;    
    std::vector<double> c,s,g;
    std::vector<std::size_t> blocsize,offset;
    
    Matrix<double> H;
    std::vector<double> Vk;
    std::vector<double> x;
};

#endif

