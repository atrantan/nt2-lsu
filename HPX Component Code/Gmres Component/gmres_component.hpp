#if !defined(GMRES_COMPONENT_HPP)
#define GMRES_COMPONENT_HPP

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

#include "matrix.h"
#include "spmatrix.h"

typedef hpx::lcos::future<double>  				    V0t_future;
typedef hpx::lcos::future< std::pair<std::vector<double>,double> >  V0_future;
typedef hpx::lcos::future< std::vector<double> >                    Hk_future;
typedef hpx::lcos::future<double>  				    wt_future;
typedef hpx::lcos::future< std::pair<std::vector<double>,double> >  w_future;
typedef hpx::lcos::future< void >                    xt_future;
typedef hpx::lcos::future< std::vector<double> >                    x_future;

namespace server
{

  struct HPX_COMPONENT_EXPORT gmres
  : hpx::components::managed_component_base<gmres>
  {
      // Methods
      
      void invoke();
      
      double init(hpx::naming::id_type,
                  std::map<std::string,std::size_t>,
                  std::vector<int>,
                  std::vector<double>,
                  std::vector<int>
                  );
      
      double V0Compute(std::size_t, int);
      
      std::pair<std::vector<double>,double> V0Reduce(std::vector<double>);
      
      void GS_HkCompute(std::size_t, int, std::size_t, std::size_t);
      
      std::vector<double> GS_HkReduce(double, std::vector<double>, std::size_t);
      
      double GS_wCompute(std::size_t, std::size_t, std::size_t);
      
      std::pair<std::vector<double>,double> GS_wReduce(std::vector<double>, std::size_t);
      
      void xCompute(std::size_t, std::size_t, std::size_t);
      
      std::vector<double> xReduce(std::vector<double>, std::size_t);


      // Actions
      
      HPX_DEFINE_COMPONENT_ACTION(gmres, invoke,        invoke_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, init,          init_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, V0Compute,     V0Compute_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, V0Reduce,      V0Reduce_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, GS_HkCompute,  GS_HkCompute_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, GS_HkReduce,   GS_HkReduce_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, GS_wCompute,   GS_wCompute_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, GS_wReduce,    GS_wReduce_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, xCompute,      xCompute_action);
      HPX_DEFINE_COMPONENT_ACTION(gmres, xReduce,       xReduce_action);  
      
      // Members
      
      std::size_t m;
      std::size_t N;
      std::size_t Nlocal;
      std::size_t imin;
      std::size_t Nblocs;
  
      Spmatrix<double> 	A;
      Matrix<double> 	V;
      
      std::vector<double> b, Hk, Vk, x, g;
      
      hpx::naming::id_type gid;
      
      std::vector<std::size_t> blocsize; // Array of size Nblocs with successive blocksizes
      std::vector<std::size_t> offset;   // Array of size Nblocs with successive offsets
      
      Matrix<double> Hkt;
  };
}

namespace stubs
{
  
  struct gmres : hpx::components::stub_base<server::gmres>
  {
      static hpx::lcos::future<void> invoke(hpx::naming::id_type const& gid_)
      {
          return hpx::async<server::gmres::invoke_action>(gid_);
      }
      
      static double init( hpx::naming::id_type const & gid_,
                          std::map<std::string,std::size_t> const & par_,
                          std::vector<int> const & rows,
                          std::vector<double> const & values,
                          std::vector<int> const & indices
                        )
      {
          return hpx::async<server::gmres::init_action>(gid_,gid_,par_,rows,values,indices).get();
      }
      
      static V0_future V0Reduce(hpx::naming::id_type const& gid_, std::vector<double> const & x_)
      {
          return hpx::async<server::gmres::V0Reduce_action>(gid_,x_);
      }
      
      static Hk_future GS_HkReduce(hpx::naming::id_type const& gid_,
                                    double alpha,
                                    std::vector<double> const & Vk_,
                                    std::size_t k_
                                   )
      {
          return hpx::async<server::gmres::GS_HkReduce_action>(gid_,alpha,Vk_,k_);
      }
      
      static w_future GS_wReduce(hpx::naming::id_type const& gid_,
                                 std::vector<double> const & Hk_,
                                 std::size_t k_
                                 )
      {
          return hpx::async<server::gmres::GS_wReduce_action>(gid_,Hk_,k_);
      }
      
      static x_future xReduce(hpx::naming::id_type const& gid_,
                              std::vector<double> const & g_,
                              std::size_t k_
                              )
      {
          return hpx::async<server::gmres::xReduce_action>(gid_,g_,k_);
      }
      
  };

}

struct gmres
  : hpx::components::client_base<gmres, stubs::gmres>
{
    typedef hpx::components::client_base<gmres, stubs::gmres>
    base_type;

    hpx::lcos::future<void> invoke()
    {
      return this->base_type::invoke(this->get_gid());
    }
    
    double init(std::map<std::string,std::size_t> const & Par,
                std::vector<int> const & rows,
                std::vector<double> const & values,
                std::vector<int> const & indices
                )
    {
      return this->base_type::init(this->get_gid(),Par,rows,values,indices);
    }
    
    V0_future V0Reduce(std::vector<double> const & x_)
    {
        return this->base_type::V0Reduce(this->get_gid(),x_);
    }
    
    Hk_future GS_HkReduce(double alpha, std::vector<double> const & Vk_, std::size_t k)
    {
      return this->base_type::GS_HkReduce(this->get_gid(),alpha,Vk_,k);
    }
        
    w_future GS_wReduce(std::vector<double> const & Hk_, std::size_t k)
    {
      return this->base_type::GS_wReduce(this->get_gid(),Hk_,k);
    }
    
    x_future xReduce(std::vector<double> const & g_, std::size_t k)
    {
        return this->base_type::xReduce(this->get_gid(),g_,k);
    }
};

HPX_REGISTER_ACTION_DECLARATION(server::gmres::invoke_action,       gmres_invoke_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::init_action,         gmres_init_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::V0Compute_action,    gmres_V0Compute_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::V0Reduce_action,     gmres_V0Reduce_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::GS_HkCompute_action, gmres_GS_HkCompute_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::GS_HkReduce_action,  gmres_GS_HkReduce_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::GS_wCompute_action,  gmres_GS_wCompute_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::GS_wReduce_action,   gmres_GS_wReduce_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::xCompute_action,     gmres_xCompute_action);
HPX_REGISTER_ACTION_DECLARATION(server::gmres::xReduce_action,      gmres_xReduce_action);


#endif 
