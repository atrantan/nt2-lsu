#ifndef PARAM_H
#define PARAM_H

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/serialization/shared_ptr.hpp>

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
//   Matrix<double> A;
  Spmatrix<double> A;
  Matrix<double> V;
  Matrix<double> H;
  std::vector<double> t0;
  std::vector<double> x;
  
  std::vector<double> c;
  std::vector<double> s;  
  std::vector<double> g;
  
    
  Param(std::size_t m_=1, std::size_t N_=1, std::size_t Nblocs_=1):
  m(m_),N(N_),Nblocs(Nblocs_),jbloc(N/Nblocs),
  tol(1e-6),rho(1.0),
  A(N,N),V(m,N),H(m,m+1),
  t0(N), x(N),
  c(m+1), s(m+1), g(m+1,0.0)
  {        
    double eps = 0.05; // set amount of asymmetry
    std::vector<double> exact(N,1.0);
    std::vector<double> b(N);
    std::vector<double> r0(N);
    
    // Initialization x = x0
    x[0]=1.0;
     
    std::size_t pas(100);
    int idx(0);
      
    // Initialization A
    for (std::size_t i = 0; i < N; i++)
    {       
      A.rows.push_back(idx+1);
      
      for (std::size_t j=0; j <i; j++)
      {
	if (i-j<=pas)
	{
	  A.values.push_back(-1.0-(i-j)*eps); idx++;
	  A.indices.push_back(j+1);   // Warning: Fortran indices
	}
      }
      
      A.values.push_back(2.1); idx++;
      A.indices.push_back(i+1);   // Warning: Fortran indices
      
      for (std::size_t j = i+1; j<N; j++)
      {
	if (j-i<=pas)
	{
	  A.values.push_back(-1.0 + (j-i)*eps); idx++;
	  A.indices.push_back(j+1);   // Warning: Fortran indices
	}
      }
    }
    
    A.rows.push_back(A.values.size()+1);
            
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
    
//     Initialization A
//       for (std::size_t i = 0; i < A.height; i++)
//       {
//         	A(i,i) = 2.1;
//          if (i>0) A(i-1, i) = -1+eps;
//          if (i+1<A.height) A(i+1, i)= -1-eps;
//       }
//       
//       // b = A*exact
//       cblas_dgemv(CblasRowMajor,CblasNoTrans,A.height,A.width,
//       1.0,&A(0,0),A.width, &exact[0],1,0.0,&b[0],1);
//       
//       //r0 = b - a*x0
//       cblas_dgemv(CblasRowMajor,CblasNoTrans,A.height,A.width,
//       -1.0,&A(0,0),A.width, &x[0],1,0.0,&r0[0],1);
//       cblas_daxpy(b.size(),1.0,&b[0],1,&r0[0],1);
//       
//       rho = cblas_dnrm2(r0.size(),&r0[0],1);
//       
//       // Initialization line 1 of V
//       for (std::size_t i = 0; i < V.width; i++)
//       V(0,i) = r0[i]/rho;
//             
//       // Normalization of the tolerance
//       tol = tol*cblas_dnrm2(b.size(),&b[0],1);
  }
   
};

typedef boost::shared_ptr<Param> Param_ptr;

#endif // PARAM_H
