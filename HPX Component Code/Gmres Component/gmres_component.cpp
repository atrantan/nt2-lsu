#include "gmres_component.hpp"
#include <hpx/include/iostreams.hpp>

extern "C" {
#include <bebop/smc/sparse_matrix.h>
#include <bebop/smc/sparse_matrix_ops.h>
#include <bebop/smc/csr_matrix.h>
}

#include <mkl_cblas.h>
#include <mkl_spblas.h>

namespace server
{
  void gmres::invoke()
  {
    hpx::cout << "Hello HPX World!\n" << hpx::flush;
  }
  
  double gmres::init( hpx::naming::id_type gid_,
                      std::map<std::string,std::size_t> Par,
                      std::vector<int> rows,
                      std::vector<double> values,
                      std::vector<int> indices
                     )
  {
    
    m           =   Par["m"];
    N 		=   Par["N"];	
    Nblocs      =   Par["Nblocs"];         
    Nlocal      =   Par["Nlocal"];
    imin        =   Par["imin"];      
    gid     	=   gid_;
      
    // Resize Matrices and Vectors
    blocsize.resize(Nblocs);
    offset.resize(Nblocs);
      
    A.height    = Nlocal;
    A.width     = N;

    V.height = m+1;
    V.width = Nlocal;
    V.data.resize((m+1)*Nlocal);
      
    b.resize(Nlocal);
    Hk.resize(m);
    Vk.resize(N);
    x.resize(N);
    g.resize(m+1);
      
    Hkt.height = Nblocs;
    Hkt.width  = m;
    Hkt.data.resize(Nblocs*m);     
      
    // Calculate local offsets and blocsizes
    blocsize[0] = Nlocal/Nblocs + (Nlocal%Nblocs>0);
    offset[0]   = 0;
    for(std::size_t i=1; i<Nblocs; i++)
    {
      blocsize[i] = Nlocal/Nblocs + (Nlocal%Nblocs>i);
      offset[i]   = blocsize[i-1] + offset[i-1];
    }

    A.rows       = std::move(rows);
    A.values     = std::move(values);
    A.indices    = std::move(indices);
      
    int row_offset = A.rows[0];
    
    // Pass from zero-based indexing to one-based indexing
    for (auto &val:A.rows)
    val = val - row_offset + 1;
    
    for (auto &val:A.indices)
    val++;
    
    std::vector<double> exact(N,1.0);
    
    // Compute the right-hand vector
    char transa('N');
    mkl_dcsrgemv(&transa,&A.height,&A.values[0],&A.rows[0],&A.indices[0],&exact[0],&b[0]);
    
    // Local square tolerance
    return cblas_ddot(Nlocal,&b[0],1,&b[0],1);
  }

   double gmres::V0Compute(std::size_t i, int blocsize)
   {
        // SPMV
        char transa('N');
        mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&x[0],&V(0,i));

        // AXPY
        cblas_daxpy(blocsize,-1.0,&b[i],1,&V(0,i),1);
       
        return cblas_ddot(blocsize, &V(0,i), 1, &V(0,i), 1);
    }
    
    std::pair<std::vector<double>,double> gmres::V0Reduce( std::vector<double> x_ )
    {
        x = std::move(x_);
        
        double dotV0= 0.;
        
        std::vector<V0t_future> V0deps;
        V0deps.reserve(Nblocs);
        
        for(std::size_t i=0; i<Nblocs ; i++)
        V0deps.push_back( hpx::async<server::gmres::V0Compute_action>(gid,offset[i],blocsize[i]) );
        
        for (auto&f:V0deps)
        dotV0 +=f.get();
        
        return std::make_pair( std::vector<double>(&V(0,0),&V(1,0)),
                               dotV0
                              );
    }
  
  
  void gmres::GS_HkCompute(std::size_t i, int blocsize, std::size_t k, std::size_t ii)
  {
      // SPMV
      char transa('N');
      mkl_dcsrgemv(&transa,&blocsize,&A[A.rows[i]-1],&A.rows[i],&A.indices[A.rows[i]-1],&Vk[0],&V(k,i));
      
      // Loop of VecDot
      for (std::size_t j=0; j<k; j++)
      Hkt(ii,j) = cblas_ddot(blocsize, &V(j,i), 1, &V(k,i), 1);
  }
  
  std::vector<double> gmres::GS_HkReduce(double alpha, std::vector<double> Vk_, std::size_t k)
  {
    Vk = std::move(Vk_);
     
    std::vector<double> Hk_(k);
    
    std::vector< hpx::lcos::future<void> > Hkdeps;      
    Hkdeps.reserve(Nblocs);
       
    cblas_dscal(Nlocal,alpha,&V(k-1,0),1);
    
   // Launch asynchronous computation of Hk vector
    for(std::size_t i=0; i<Nblocs ; i++)
    Hkdeps.push_back( hpx::async<server::gmres::GS_HkCompute_action>(gid,offset[i],blocsize[i],k,i) );
    
    // Reduction Step for Hk vector
    for(std::size_t i=0; i<Nblocs ; i++)
    { Hkdeps[i].get();
      cblas_daxpy(k,1.0,&Hkt(i,0),1,&Hk_[0],1);
    }
    
    return Hk_;
  }
  
  double gmres::GS_wCompute(std::size_t i, std::size_t blocsize, std::size_t k)
  {
      // Loop of AXPY
      for (std::size_t j=0; j<k; j++)
      cblas_daxpy(blocsize,-Hk[j],&V(j,i),1,&V(k,i),1);
      
      return cblas_ddot(blocsize, &V(k,i), 1, &V(k,i), 1);
  }
    
  std::pair<std::vector<double>,double> gmres::GS_wReduce(std::vector<double> Hk_, std::size_t k)
  {
    Hk = std::move(Hk_);
      
    double dotw= 0.;
    
    std::vector<wt_future> wdeps;
    wdeps.reserve(Nblocs);
     
    // Launch asynchronous computation of Vk vector
    for(std::size_t i=0; i<Nblocs ; i++)
    wdeps.push_back( hpx::async<server::gmres::GS_wCompute_action>(gid,offset[i],blocsize[i],k) );
    
    for (auto&f:wdeps)
    dotw +=f.get();
    
    return std::make_pair( std::vector<double>(&V(k,0),&V(k+1,0)),
                           dotw
                          );
  }
    
    void gmres::xCompute(std::size_t i, std::size_t blocsize, std::size_t k)
    {
        // Loop of AXPY
        for (std::size_t j=0; j<k; j++)
        cblas_daxpy(blocsize,g[j],&V(j,i),1,&x[imin+i],1);
    }
    
    std::vector<double> gmres::xReduce(std::vector<double> g_, std::size_t k)
    {
        g = std::move(g_);
        
        std::vector<xt_future> xdeps;
        xdeps.reserve(Nblocs);
        
        for(std::size_t i=0; i<Nblocs ; i++)
        xdeps.push_back( hpx::async<server::gmres::xCompute_action>(gid,offset[i],blocsize[i],k) );
        
        hpx::lcos::wait(xdeps);
        
        return std::vector<double>(&x[imin],&x[imin+Nlocal]);
    }
}

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<server::gmres> gmres_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(gmres_type, gmres);

HPX_REGISTER_ACTION(server::gmres::invoke_action,       gmres_invoke_action);
HPX_REGISTER_ACTION(server::gmres::init_action,         gmres_init_action);
HPX_REGISTER_ACTION(server::gmres::V0Compute_action,	gmres_V0Compute_action);
HPX_REGISTER_ACTION(server::gmres::V0Reduce_action,     gmres_V0Reduce_action);
HPX_REGISTER_ACTION(server::gmres::GS_HkCompute_action,	gmres_GS_HkCompute_action);
HPX_REGISTER_ACTION(server::gmres::GS_HkReduce_action,	gmres_GS_HkReduce_action);
HPX_REGISTER_ACTION(server::gmres::GS_wCompute_action,	gmres_GS_wCompute_action);
HPX_REGISTER_ACTION(server::gmres::GS_wReduce_action,	gmres_GS_wReduce_action);
HPX_REGISTER_ACTION(server::gmres::xCompute_action,     gmres_xCompute_action);
HPX_REGISTER_ACTION(server::gmres::xReduce_action,      gmres_xReduce_action);

