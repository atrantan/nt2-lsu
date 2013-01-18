#include "GSArnoldi_forward.h"

//-------------------------
// GMRES_V0compute Function
//-------------------------
void GMRES_V0compute(Param_ptr const & p, std::size_t i, int blocsize)
{ 
  std::vector<double> & x(p->x);
  std::vector<double> & b(p->b);
  Matrix<double> &      V(p->V);
  Spmatrix<double> &    A(p->A);
  
  char transa('N');
  mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&x[0],&V(0,i));
  
//   double * y(&V(0,0));
//   double * value(&A[A.rows[i]-1]);
//   int *    col_idx(&A.indices[A.rows[i]-1]);
//   
//   for(std::size_t ii=i; ii<i+blocsize; ii++)
//   {
//     double y_i = 0;
//     
//     for(int jj = A.rows[ii];jj<A.rows[ii+1];jj++,col_idx++,value++)
//     {
//       y_i += value[0]*x[col_idx[0]-1];
//     }
//     y[ii]= y_i;
//   }
  
  cblas_daxpy(blocsize,-1.0,&b[i],1,&V(0,i),1);
}

//-------------------
// GS_HkAdd Function
//-------------------
std::size_t GS_HkAdd(Param_ptr const & p, std::size_t k, Hkt_future const & Hk1, Hkt_future const & Hk2)
{
   std::size_t 		id1 = Hk1.get();
   std::size_t 		id2 = Hk2.get();
   Matrix<double> &	Hkt(p->Hkt);
   
   if(id1>id2) std::swap(id1,id2);
   
   cblas_daxpy(k,1.0,&Hkt(id2,0),1,&Hkt(id1,0),1);
   
   return id1;
}

//-----------------------
// GS_Hkcompute Function
//-----------------------
std::size_t GS_Hkcompute(Param_ptr const & p, std::size_t i, int blocsize, std::size_t k, std::size_t id)
{ 
  Spmatrix<double> &    A(p->A);
  Matrix<double> &      V(p->V);
  Matrix<double> &	Hkt(p->Hkt);
    
  // Matrix-Vector Product
  char transa('N');
  mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&V(k-1,0),&V(k,i));
  
//   double * x(&V(k-1,0));
//   double * y(&V(k,0));
//   double * value(&A[A.rows[i]-1]);
//   int * col_idx(&A.indices[A.rows[i]-1]);
//   
//   for(std::size_t ii=i; ii<i+blocsize; ii++)
//   {
//     double y_i = 0;
//     
//     for(int jj = A.rows[ii];jj<A.rows[ii+1];jj++,col_idx++,value++)
//     {
//       y_i += value[0]*x[col_idx[0]-1];
//     }
//     y[ii] = y_i;
//   }
    
  // Loop of vecdot
  for (std::size_t j=0; j<k; j++)
  Hkt(id,j) = cblas_ddot(blocsize, &V(j,i), 1, &V(k,i), 1);
  
  return id;
}

//----------------------
// GS_wcompute Function
//----------------------
void GS_wcompute(Param_ptr const & p, std::size_t i, std::size_t blocsize, std::size_t k)
{ 

  Matrix<double> &      V (p->V);
  Matrix<double> &      H (p->H);
  
//  cblas_dgemv(CblasRowMajor,CblasTrans,k,blocsize,-1.0,&V(0,i),V.width, &H(k-1,0),1,1.0,&V(k,i),1);
  
  // Loop of axpy
  for (std::size_t j=0; j<k; j++)
  cblas_daxpy(blocsize,-H(k-1,j),&V(j,i),1,&V(k,i),1);
  
}

//----------------------
// GMRES_xcompute Function
//----------------------
void GMRES_xcompute(Param_ptr const & p, std::size_t i, std::size_t blocsize, std::size_t k)
{
    
    Matrix<double> &      V (p->V);
    std::vector<double> & x(p->x);
    std::vector<double> & g(p->g);
    
    // Transpose-Vector Product
//    cblas_dgemv(CblasRowMajor,CblasTrans,k-1,blocsize,1.0,&V(0,i),V.width, &g[0],1,1.0,&x[i],1);
    
     for (std::size_t j=0; j<k-1; j++)
     cblas_daxpy(blocsize,g[j],&V(j,i),1,&x[i],1);
    
}
