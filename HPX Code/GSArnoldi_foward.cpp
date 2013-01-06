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
std::vector<double> GS_Hkcompute(Param_ptr const & p, std::size_t i, std::size_t k)
{ 
  int jbloc(p->jbloc);
  std::size_t const  N(p->N);
    
  Spmatrix<double> & A (p->A);
//   Matrix<double> & A (p->A);
  Matrix<double> & V (p->V);  
  std::vector<double> & t0(p->t0);
    
  std::vector<double> Hkt(k);   
  
  if(i+jbloc>N) jbloc = N-i;
  
  // Matrix-Vector Product
  
//   cblas_dgemv(CblasRowMajor,CblasNoTrans,jbloc,N,1.0,&A(i,0),A.width, &V(k-1,0),1,0.0,&t0[i],1);
  
  char transa('N');
  mkl_dcsrgemv(&transa,&jbloc,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&V(k-1,0),&t0[i]);
    
  // Matrix-Vector Product
  cblas_dgemv(CblasRowMajor,CblasNoTrans,k,jbloc,1.0,&V(0,i),N,&t0[i],1,0.0,&Hkt[0],1);
  
  return Hkt;
}

//----------------------
// GS_wcompute Function
//----------------------
void GS_wcompute(Param_ptr const & p, std::size_t i, std::size_t k)
{ 
  int jbloc(p->jbloc);
  std::size_t const N(p->N);
    
  Matrix<double> & V (p->V);
  Matrix<double> & H (p->H);
  std::vector<double> & t0(p->t0);

  if(i+jbloc>N) jbloc = N-i;
  
  // Transpose-Vector Product
  cblas_dgemv(CblasRowMajor,CblasTrans,k,jbloc,1.0,&V(0,i),V.width, &H(k-1,0),1,0.0,&V(k,i),1);
    
  // Vector Addition
  cblas_daxpy(jbloc,-1.0,&t0[i],1,&V(k,i),1);
  
}
