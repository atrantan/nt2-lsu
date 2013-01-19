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
    ar & max_it;
    ar & tol;
    ar & A;
    ar & V;
    ar & H;
    ar & x;
    ar & b;
    ar & c;
    ar & s;
    ar & g;
    ar & Hkt;
  }   
  
  std::size_t m;
  std::size_t N;
  std::size_t Nblocs;
  std::size_t max_it;
  
  double tol;
  Spmatrix<double> A;
  Matrix<double> V;
  Matrix<double> H;
   
  std::vector<double> x;
  std::vector<double> b;
  
  std::vector<double> c;
  std::vector<double> s;  
  std::vector<double> g;
  
  Matrix<double> Hkt;
    
  Param(std::size_t m_=1, std::size_t max_it_=1, std::size_t Nblocs_=1, std::string Mfilename=""):
  m(m_),Nblocs(Nblocs_),max_it(max_it_),tol(1e-6),
  H(m,m+1), c(m+1), s(m+1), g(m+1,0.0),Hkt(Nblocs,m)
  {
          
     // Initialization A
     struct sparse_matrix_t* At;
     struct csr_matrix_t * ptr;
     
     At = load_sparse_matrix(MATRIX_MARKET,Mfilename.c_str());
     if (At == NULL) 
     {
       std::cout<<"File not found\n";
       exit(EXIT_FAILURE);
     }
     sparse_matrix_convert(At,CSR);
     
     ptr = (struct csr_matrix_t*) (At->repr);
             
     N = ptr->n;
     
     std::size_t nnz = ptr->nnz;
     A.height = N; 
     A.width = N;
     
     A.rows.resize(N+1);
     A.values.resize(nnz);
     A.indices.resize(nnz);
     
     memcpy(&A.rows[0],ptr->rowptr,(N+1)*sizeof(int));
     memcpy(&A.indices[0],ptr->colidx,(nnz)*sizeof(int));
     memcpy(&A.values[0],ptr->values,(nnz)*sizeof(double));
     
     destroy_sparse_matrix(At);
         
     // Pass from zero-based indexing to one-based indexing
     for (auto &val:A.rows)
     val++;
     
     for (auto &val:A.indices)
     val++;

    // Initialization V
    V.height = m+1; 
    V.width = N; 
    V.data.resize((m+1)*N);
    
    // Initialization x and b
    x.resize(N);
    b.resize(N);
    
    // Initialization x = x0
    x[0]=1.0;
    
    std::vector<double> exact(N,1.0);
    
    // Compute the right-hand vector        
    char transa('N');
    mkl_dcsrgemv(&transa,&A.height,&A.values[0],&A.rows[0],&A.indices[0],&exact[0],&b[0]);
    
    // Normalization of the tolerance
    tol = tol*cblas_dnrm2(N,&b[0],1);    
  }
   
};

typedef boost::shared_ptr<Param> Param_ptr;

#endif // PARAM_H
