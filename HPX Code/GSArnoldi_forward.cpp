#include "GSArnoldi_forward.h"

//-------------------------
// GMRES_V0compute Function
//-------------------------
double GMRES_V0compute(Param_ptr const & p, std::size_t i, int blocsize)
{ 
  std::vector<double> & x(p->x);
  std::vector<double> & b(p->b);
  Matrix<double> &      V(p->V);
  Spmatrix<double> &    A(p->A);
  
  // SPMV
  char transa('N');
  mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&x[0],&V(0,i));
  
//   double * y(&V(0,i));
//   int *    row(&A.rows[i]);
//   double * value(&A[A.rows[i]-1]);
//   int *    col_idx(&A.indices[A.rows[i]-1]);
//   
//   for(int ii=0; ii<blocsize; ii++,row++)
//   {
//     double y_i = 0;
//     
//     for(int jj = row[0];jj<row[1];jj++,col_idx++,value++)
//     {
//       y_i += value[0]*x[col_idx[0]-1];
//     }
//     y[ii]= y_i;
//   }
    
  // AXPY
   cblas_daxpy(blocsize,-1.0,&b[i],1,&V(0,i),1);
   
   return cblas_ddot(blocsize, &V(0,i), 1, &V(0,i), 1);
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

  // SPMV
  char transa('N');
  mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&V(k-1,0),&V(k,i));
  
//   double * x(&V(k-1,0));
//   double * y(&V(k,i));
//   int *    row(&A.rows[i]);
//   double * value(&A[A.rows[i]-1]);
//   int *    col_idx(&A.indices[A.rows[i]-1]);
//   
//   for(int ii=0; ii<blocsize; ii++,row++)
//   {
//     double y_i = 0;
//     
//     for(int jj = row[0];jj<row[1];jj++,col_idx++,value++)
//     {
//       y_i += value[0]*x[col_idx[0]-1];
//     }
//     y[ii] = y_i;
//   }
    
  // Loop of VecDot
  for (std::size_t j=0; j<k; j++)
  Hkt(id,j) = cblas_ddot(blocsize, &V(j,i), 1, &V(k,i), 1);
  
  return id;
}

//----------------------
// GS_wcompute Function
//----------------------
double GS_wcompute(Param_ptr const & p, std::size_t i, std::size_t blocsize, std::size_t k)
{ 

  Matrix<double> &      V (p->V);
  Matrix<double> &      H (p->H);
  
//  cblas_dgemv(CblasRowMajor,CblasTrans,k,blocsize,-1.0,&V(0,i),V.width, &H(k-1,0),1,1.0,&V(k,i),1);
  
  // Loop of AXPY
  for (std::size_t j=0; j<k; j++)
  cblas_daxpy(blocsize,-H(k-1,j),&V(j,i),1,&V(k,i),1);
  
  return cblas_ddot(blocsize, &V(k,i), 1, &V(k,i), 1);
  
}

//----------------------
// GMRES_xcompute Function
//----------------------
void GMRES_xcompute(Param_ptr const & p, std::size_t i, std::size_t blocsize, std::size_t k)
{
    
    Matrix<double> &      V (p->V);
    std::vector<double> & x(p->x);
    std::vector<double> & g(p->g);

    // Loop of AXPY
    for (std::size_t j=0; j<k-1; j++)
    cblas_daxpy(blocsize,g[j],&V(j,i),1,&x[i],1);
    
}
