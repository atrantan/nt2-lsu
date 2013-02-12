//
//  gmres_test.cpp
//  
//
//  Created by antoine on 11/11/12.
//
//

#include "gmres_test.h"

//-------------
// Gmres_Cycle 
//-------------   
void gmres_test::gmres_cycle()
{ 
    
  // Initialization of local iteration
  std::size_t k=1;
    
  std::vector<V0_future> V0deps;
  V0deps.reserve(num_locs);
   
  double dotV0 = 0.;
  
  // Gather not nprmalized V0
  for(auto & p_ :p)
  V0deps.push_back( p_.V0Reduce(x) );
    
  for(std::size_t i=0; i<num_locs; i++)
  {
      std::size_t imin (offset[i]);
      std::pair<std::vector<double>,double> res = V0deps[i].get();
      
      std::copy( res.first.begin(), res.first.end(), Vk.begin()+imin );
      dotV0 += res.second;
  }
  rho = sqrt(dotV0);
  
  // Constant for Vk normalization
  double alpha = -1./rho;
  
  // Initialization of vector g
  for(auto &f:g) f=0.; g[0]=rho;
   
  while((rho > tol) && (k <= m) && (it <= max_it))  
  {
    std::vector<Hk_future> Hkdeps;
    std::vector<w_future> wdeps;
      
    std::vector<double> Hk(k);

    Hkdeps.reserve(num_locs);
    wdeps.reserve(num_locs);
            
    // Vk scale
    cblas_dscal(N,alpha,&Vk[0],1);
      
   // Send Vk and Receive Hk values
    for(auto & p_ :p)
    Hkdeps.push_back( p_.GS_HkReduce(alpha,Vk,k) );
        
   // Reduce Hk values
    for(auto & Hk_ :Hkdeps)
    cblas_daxpy(k,1.0,&(Hk_.get()[0]),1,&Hk[0],1);
    
    // Store Hk in H matrix
    for(std::size_t i=0; i<k ; i++)
    H(k-1,i) = Hk[i];
      
    // Send Hk and Receive Not-Normalized Vk+1
    for(auto & p_ :p)
    wdeps.push_back( p_.GS_wReduce(Hk,k) );
    
    double dotw = 0.;
    
    // Gather Not-Normalized Vk+1
    for(std::size_t i=0; i<num_locs; i++)
    {
        std::size_t imin (offset[i]);
        std::pair<std::vector<double>,double> res = wdeps[i].get();
          
        std::copy( res.first.begin(), res.first.end(), Vk.begin()+imin );
        dotw += res.second;
    }
      
    H(k-1,k) = sqrt(dotw);    
    alpha = 1.0/H(k-1,k);
    
    // Sequential Givens rotations
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
    
    //std::cout<<"Res"<<it<<": "<<rho<<std::endl;
    
    k++;
    it++;
  }
  
  // It's time to compute the solution
  cblas_dtrsv(CblasRowMajor,CblasLower,CblasTrans,CblasNonUnit,k-1,&H(0,0),H.width,&g[0],1);
  
  std::vector<x_future> xdeps;
  xdeps.reserve(num_locs);
    
  // Send Hk and Receive Not-Normalized Vk+1
  for(auto & p_ :p)
  xdeps.push_back( p_.xReduce(g,k-1) );
    
  for(std::size_t i=0; i<num_locs; i++)
  {
     std::size_t imin (offset[i]);
     std::vector<double> res = xdeps[i].get();
        
     std::copy( res.begin(), res.end(), x.begin()+imin );
   }
  
}
   
//-------------------
// GMRES Algorithm 
//-------------------
void gmres_test::operator()()
{  
  rho = 1.;
    
  // Initialization of vector x
  for(auto &f:x) f = 0.; x[0] = 1.;

  // Initialization of global iteration
  it = 1;
  
   while(it<=max_it && rho > tol)  
   {
    // Launch a Gmres cycle and update rho
    gmres_cycle();
   }
//   std::cout<<"Nb_iter: "<<it-1<<std::endl;
}