#include "hpx_gmres_test.h"
   
//-------------
// Gmres_Cycle 
//-------------   
double hpx_gmres_test::gmres_cycle()
{
  std::vector<double> & c(p->c);
  std::vector<double> & s(p->s);  
  std::vector<double> & g(p->g);
    
  std::vector<double> & x(p->x);
  std::vector<double> & r0(p->r0);
  
  Matrix<double>      & V(p->V);
  Matrix<double>      & H(p->H);
  
  std::size_t const m(p->m);
  std::size_t const N(p->N);
  std::size_t const Nblocs(p->Nblocs); 
  std::size_t const max_it(p->max_it);
  
  double const tol(p->tol);
  double rho;
  
  std::vector<r0_future> r0deps;
  
  // Launch asynchronous calculations
  for(std::size_t i=0; i<Nblocs ; i++)
  r0deps.push_back( hpx::async(&GMRES_r0compute,p,offset[i],blocsize[i]) );
   
  hpx::lcos::wait(r0deps);
  rho = cblas_dnrm2(N,&r0[0],1);
    
  // Initialization line 1 of V
  for (std::size_t j = 0; j < N; j++)
  V(0,j) = -r0[j]/rho;
  
  // Initialization of local iteration
  std::size_t k = 1;
  
  // Initialization of vector g
  for(auto &f:g) f = 0.; g[0]=rho;
  
  while((rho > tol) && (k <= m) && (it <= max_it))  
  {
    std::vector<Hkt_future> Hkt;
    std::vector<w_future> wdeps;
    std::vector<double> Hk(k);
    
    Hkt.reserve(2*Nblocs);
    wdeps.reserve(Nblocs);
    
    // Launch asynchronous calculations
    for(std::size_t i=0; i<Nblocs ; i++)
    Hkt.push_back( hpx::async(&GS_Hkcompute,p,offset[i],blocsize[i],k) );
    
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
    
    // Copy Hk vector
    for(std::size_t i=0; i<k; i++)
    H(k-1,i) = Hk[i];
    
    // Launch asynchronous calculations
    for(std::size_t i=0; i<Nblocs ; i++)
    wdeps.push_back( hpx::async(&GS_wcompute,p,offset[i],blocsize[i],k) );

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
    
    std::cout<<"Res"<<it<<": "<<rho<<std::endl;
    
    k++;
    it++;
  }
  
  // It's time to compute the solution
  cblas_dtrsv(CblasRowMajor,CblasLower,CblasTrans,CblasNonUnit,k-1,&H(0,0),H.width,&g[0],1);
  cblas_dgemv(CblasRowMajor,CblasTrans,k-1,V.width,1.0,&V(0,0),V.width, &g[0],1,1.0,&x[0],1);
  
  return rho;
}
   
//-------------------
// GMRES Algorithm 
//-------------------
void hpx_gmres_test::operator()()
{
  std::vector<double> & x(p->x);
  double const 		tol(p->tol);
  std::size_t const 	max_it(p->max_it);
  double 		rho = 1;
    
  // Initialization of vector x
  for(auto &f:x) f = 0.; x[0] = 1.;

  // Initialization of global iteration
  it = 1;
  
  while(it<=max_it && rho > tol)  
  {
    // Launch a Gmres cycle and returns last residual norm
    rho = gmres_cycle();
  }

}

