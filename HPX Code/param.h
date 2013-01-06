#ifndef PARAM_H
#define PARAM_H

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/serialization/shared_ptr.hpp>

extern "C" {
#include <bebop/smc/sparse_matrix.h>
#include <bebop/smc/sparse_matrix_ops.h>
#include <bebop/smc/csr_matrix.h>
}

#include <iostream>

#include <hpx/include/iostreams.hpp>

#include <mkl_cblas.h>
#include <mkl_spblas.h>

#include "matrix.h"
#include "spmatrix.h"
#include "benchmark.hpp"

using namespace details;

class Param
{
public:
  
friend class boost::serialization::access;

template <typename Archive>
void serialize(Archive & ar, unsigned)
  { 
    ar & m;
    ar & N;
    ar & Nblocs;
    ar & jbloc;
    ar & tol;
    ar & rho;
    ar & A;
    ar & V;
    ar & H;
    ar & t0;
    ar & x;
    ar & c;
    ar & s;
    ar & g;
  }   
  
  std::size_t m;
  std::size_t N;
  std::size_t Nblocs;
  int jbloc;
  double tol;
  double rho;
  Spmatrix<double> A;
  Matrix<double> V;
  Matrix<double> H;
  std::vector<double> t0;
  std::vector<double> x;
  
  std::vector<double> c;
  std::vector<double> s;  
  std::vector<double> g;
  
    
  Param(std::size_t m_=1, std::size_t Nblocs_=1, std::string Mfilename=""):
  m(m_),Nblocs(Nblocs_),tol(1e-6),rho(1.0),
  H(m,m+1), c(m+1), s(m+1), g(m+1,0.0)
  {
          
    // Initialization A
    struct sparse_matrix_t* At;
    struct csr_matrix_t * ptr;
    
    At = load_sparse_matrix(MATRIX_MARKET,Mfilename.c_str());
    if (At == NULL) 
    {
      std::cout<<"Unfound file\n";
      exit(EXIT_FAILURE);
    }
    sparse_matrix_convert(At,CSR);
    
    ptr = (struct csr_matrix_t*) (At->repr);
            
    N = ptr->n; 
    jbloc = N/Nblocs;
    
    std::size_t nnz = ptr->nnz;
    A.height = N; 
    A.width = N;
    
    A.rows.resize(N+1);
    A.values.resize(nnz);
    A.indices.resize(nnz);
    
    memcpy(&A.rows[0],ptr->rowptr,(N+1)*sizeof(int));
    memcpy(&A.indices[0],ptr->colidx,(nnz)*sizeof(int));
    memcpy(&A.values[0],ptr->values,(nnz)*sizeof(double));
        
    // Pass from zero-based indexing to one-based indexing
    for (auto &val:A.rows)
    val++;
    
    for (auto &val:A.indices)
    val++;
    
    // Initialization V
    V.height = m; 
    V.width = N; 
    V.data.resize(m*N);
    
    // Initialization t0 and x
    t0.resize(N); 
    x.resize(N);
    
    // Initialization x = x0
    x[0]=1.0;
    
    std::vector<double> exact(N,1.0);
    std::vector<double> b(N);
    std::vector<double> r0(N);
    
    // Compute the right-hand vector        
    char transa('N');
    mkl_dcsrgemv(&transa,&A.height,&A.values[0],&A.rows[0],&A.indices[0],&exact[0],&b[0]);
    
    mkl_dcsrgemv(&transa,&A.height,&A.values[0],&A.rows[0],&A.indices[0],&x[0],&r0[0]);
    cblas_daxpy(N,-1.0,&b[0],1,&r0[0],1);
    
    rho = cblas_dnrm2(N,&r0[0],1);
    
    // Initialization line 1 of V
    for (std::size_t j = 0; j < N; j++)
    V(0,j) = -r0[j]/rho;
	  
    // Normalization of the tolerance
    tol = tol*cblas_dnrm2(N,&b[0],1);
  }
   
};

typedef boost::shared_ptr<Param> Param_ptr;

#endif // PARAM_H
