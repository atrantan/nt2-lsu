#include "GSArnoldi_forward.h"

//-------------------
// GS_HkAdd Function
//-------------------
std::vector<double> GS_HkAdd(std::size_t k, Hkt_future const & Hk1, Hkt_future const & Hk2)
{
    std::vector<double> res(Hk1.get());
    cblas_daxpy(k,1.0,&Hk2.get()[0],1,&res[0],1);
    
    return res;      
}

//-----------------------
// GS_Hkcompute Function
//-----------------------
std::vector<double> GS_Hkcompute(Param_ptr p, std::size_t i, int blocsize, std::size_t k)
{ 
  std::size_t const  N(p->N);
    
  Spmatrix<double> & A (p->A);
  Matrix<double> & V (p->V);  
  std::vector<double> & t0(p->t0);
    
  std::vector<double> Hkt(k);   
    
  // Matrix-Vector Product

  char transa('N');
  mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&V(k-1,0),&t0[i]);
    
  // Matrix-Vector Product
  cblas_dgemv(CblasRowMajor,CblasNoTrans,k,blocsize,1.0,&V(0,i),N,&t0[i],1,0.0,&Hkt[0],1);
  
  return Hkt;
}

//----------------------
// GS_wcompute Function
//----------------------
void GS_wcompute(Param_ptr p, std::size_t i, std::size_t blocsize, std::size_t k)
{ 

  Matrix<double> & V (p->V);
  Matrix<double> & H (p->H);
  std::vector<double> & t0(p->t0);
  
  // Transpose-Vector Product
  cblas_dgemv(CblasRowMajor,CblasTrans,k,blocsize,1.0,&V(0,i),V.width, &H(k-1,0),1,0.0,&V(k,i),1);
    
  // Vector Addition
  cblas_daxpy(blocsize,-1.0,&t0[i],1,&V(k,i),1);
  
}
