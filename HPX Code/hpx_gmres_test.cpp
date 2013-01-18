#include "hpx_gmres_test.h"
   
//-------------
// Gmres_Cycle 
//-------------   
void hpx_gmres_test::gmres_cycle(double & rho)
{
  std::vector<double> & c(p->c);
  std::vector<double> & s(p->s);  
  std::vector<double> & g(p->g);
  
  Matrix<double>      & V(p->V);
  Matrix<double>      & H(p->H);
  Matrix<double>      &	Hkt(p->Hkt);
  
//   Spmatrix<double>    & A(p->A);
  
  std::size_t const m(p->m);
  std::size_t const N(p->N);
  
  std::size_t const Nblocs(p->Nblocs); 
  std::size_t const max_it(p->max_it);
  
  double const tol(p->tol);

// Parallel V0 computation
  
  std::vector<r0_future> r0deps;
  r0deps.reserve(Nblocs);

  for(std::size_t i=0; i<Nblocs ; i++)
  r0deps.push_back( hpx::async(&GMRES_V0compute,p,offset[i],blocsize[i]) );
  hpx::lcos::wait(r0deps);  
  
  rho = cblas_dnrm2(N,&V(0,0),1);
  
  double const alpha(-1./rho);
  
  // Initialization line 1 of V
  cblas_dscal(N,alpha,&V(0,0),1);
  
// //Sequential V0 computation
//   char transa('N');
//   mkl_dcsrgemv(&transa,&N,&A[0],&A.rows[0],&A.indices[0],&x[0],&V(0,0));
//   cblas_daxpy(N,-1.0,&b[0],1,&V(0,0),1);
//   
//   rho = cblas_dnrm2(N,&V(0,0),1);
//   
//   double const alpha(-1./rho);  
//   
//   // Initialization line 1 of V
//   cblas_dscal(N,alpha,&V(0,0),1);
  
  
  // Initialization of local iteration
  std::size_t k=1;
  
  // Initialization of vector g and Matrix H
  for(auto &f:g) f=0.; g[0]=rho;
  
//   hpx::lcos::wait(r0deps);
  
  while((rho > tol) && (k <= m) && (it <= max_it))  
  {
    std::vector<Hkt_future> Hkdeps;
    std::vector<w_future> wdeps;
      
    std::vector<double> Hk(k);

    Hkdeps.reserve(2*Nblocs);
    wdeps.reserve(Nblocs);
    
   // Launch asynchronous calculations
    for(std::size_t i=0; i<Nblocs ; i++)
    Hkdeps.push_back( hpx::async(&GS_Hkcompute,p,offset[i],blocsize[i],k, i) );
    
    // Reduction Step
    for(std::size_t i=0; i<Nblocs ; i++)
    { Hkdeps[i].get();
      cblas_daxpy(k,1.0,&Hkt(i,0),1,&Hk[0],1);
    }
    for(std::size_t i=0; i<k ; i++)
    H(k-1,i) = Hk[i];
   
    // Reduction Step	
//     std::vector<Hkt_future>::iterator value(Hkdeps.begin());
//     while (std::distance(value,Hkdeps.end())>=2)
//     {
// 	Hkdeps.push_back( hpx::async(&GS_HkAdd,p,k,*value,*(value+1)) );
// 	value+=2;
//     }
//     Hkdeps.back().get();    
//     for(std::size_t i=0; i<k ; i++)
//     H(k-1,i) = Hkt(0,i);

//     // Sequential SpMV and loop of VecDot
//     char transa('N');
//     mkl_dcsrgemv(&transa,&N,&A[0],&A.rows[0],&A.indices[0],&V(k-1,0),&V(k,0));
//     
//     for (std::size_t j=0; j<k; j++)
//     H(k-1,j) = cblas_ddot(N, &V(j,0), 1, &V(k,0), 1);
     
    // Launch asynchronous calculations
    for(std::size_t i=0; i<Nblocs ; i++)
    wdeps.push_back( hpx::async(&GS_wcompute,p,offset[i],blocsize[i],k) );

    // Norm step
    hpx::lcos::wait(wdeps);
    
//     // Sequential loop of axpy and VecScale
//     for (std::size_t j=0; j<k; j++)
//     cblas_daxpy(N,-H(k-1,j),&V(j,0),1,&V(k,0),1);
     
    H(k-1,k) = cblas_dnrm2(N,&V(k,0),1);
    double const beta(1.0/H(k-1,k));
    cblas_dscal(N,beta,&V(k,0),1);

    if (k>1)
    {
      // Update Hessenberg Matrix
      for(std::size_t i=0; i<k-1; i++)
      cblas_drot(1,&H(k-1,i),1,&H(k-1,i+1),1,c[i],s[i]);
    }
  
    // Prepare next Hessenberg Matrix update
    cblas_drotg(&H(k-1,k-1),&H(k-1,k),&c[k-1],&s[k-1]);
    H(k-1,k)=0.0;

    cblas_drot(1,&g[k-1],1,&g[k],1,c[k-1],s[k-1]);

    // Update the residual norm
    rho = std::abs(g[k]); 
    
//     std::cout<<"Res"<<it<<": "<<rho<<std::endl;
    
    k++;
    it++;
  }
    
  // It's time to compute the solution
  cblas_dtrsv(CblasRowMajor,CblasLower,CblasTrans,CblasNonUnit,k-1,&H(0,0),H.width,&g[0],1);
    
  std::vector<x_future> xdeps;
  xdeps.reserve(Nblocs);
  
  // Launch asynchronous calculations  
  for(std::size_t i=0; i<Nblocs ; i++)  
  xdeps.push_back( hpx::async(&GMRES_xcompute,p,offset[i],blocsize[i],k) );   
  hpx::lcos::wait(xdeps);
  
//// Sequential Gemv
//   cblas_dgemv(CblasRowMajor,CblasTrans,k-1,V.width,1.0,&V(0,0),V.width, &g[0],1,1.0,&x[0],1);
}
   
//-------------------
// GMRES Algorithm 
//-------------------
void hpx_gmres_test::operator()()
{
  std::vector<double> & x(p->x);
  double const          tol(p->tol);
  std::size_t const     max_it(p->max_it);
  double                rho(1);
    
  // Initialization of vector x
  for(auto &f:x) f = 0.; x[0] = 1.;

  // Initialization of global iteration
  it = 1;
  
  while(it<=max_it && rho > tol)  
  {
    // Launch a Gmres cycle and update rho
    gmres_cycle(rho);
  }
    
//   std::cout<<"Nb_iter: "<<it-1<<std::endl;  

}

