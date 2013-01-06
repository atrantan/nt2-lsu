#include "hpx_gmres_test.h"
     
// Compute GMRES Algorithm in hpx_gemres_test functor
void hpx_gmres_test::operator()()
{
  std::vector<double> & c(p->c);
  std::vector<double> & s(p->s);  
  std::vector<double> & g(p->g);
  
  double tol(p->tol);
  
  std::vector<double> & x(p->x);
  Matrix<double>      & V(p->V);
  Matrix<double>      & H(p->H);
  
  std::size_t const m(p->m);
  std::size_t const N(p->N);
  std::size_t const jbloc(p->jbloc); 
    
  // Initialization of vector x
  for(auto &f:p->x) f = 0.; p->x[0] = 1.;

  // Initialization k
  std::size_t k = 1;

  double rho(p->rho);

  // Initialization of vector g
  for(auto &f:g) f = 0.; g[0]=rho;
  
  while((rho > tol) && (k < m))  
  {
    std::vector<Hkt_future> Hkt;
    std::vector<w_future> wdeps;
    std::vector<double> Hk(k);
    
    Hkt.reserve(10*p->Nblocs);
    wdeps.reserve(10*p->Nblocs);
    
    // Launch asynchronous calculations
    for(std::size_t i=0; i<N ; i+=jbloc)
    Hkt.push_back( hpx::async(&GS_Hkcompute,p,i,k) );
    
    // Reduction Step	
//         std::vector<Hkt_future>::iterator value(Hkt.begin());
//       
//         while (std::distance(value,Hkt.end())>=2)
//         {
//             Hkt.push_back( hpx::async(&GS_HkAdd,k,*value,*(value+1)) );
//             value+=2;
//         }
//         Hk = Hkt.back().get();
    
    // Reduction Step
    for(auto &Hkt_:Hkt)
    cblas_daxpy(k,1.0,&(Hkt_.get()[0]),1,&Hk[0],1);
    
    // Launch asynchronous calculations
    for(std::size_t i=0; i<N; i+=jbloc)
    wdeps.push_back( hpx::async(&GS_wcompute,p,i,k) );
  
    // Copy Hk vector
    for(std::size_t i=0; i<k; i++)
    H(k-1,i) = Hk[i];

    // Norm step
    hpx::lcos::wait(wdeps);
    H(k-1,k) = cblas_dnrm2(N,&V(k,0),1);
  
    double const alpha(-1.0/H(k-1,k));

    cblas_dscal(N,alpha,&V(k,0),1);

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

    k++;
    
  }

// It's time to compute the solution
cblas_dtrsv(CblasRowMajor,CblasLower,CblasTrans,CblasNonUnit,k-1,&H(0,0),H.width,&g[0],1);
cblas_dgemv(CblasRowMajor,CblasTrans,k-1,V.width,1.0,&V(0,0),V.width, &g[0],1,1.0,&x[0],1);
}

