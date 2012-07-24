//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_HPX_DETAILS_GRANULARITY_HPP_INCLUDED
#define NT2_SDK_HPX_DETAILS_GRANULARITY_HPP_INCLUDED

#include <hpx/hpx_fwd.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>
#include <boost/atomic.hpp>

namespace nt2 { 
  namespace server
  {
    class granularity
      : public hpx::components::managed_component_base<granularity>
    {
    public:
      granularity() : g_(1) {}
      
      std::size_t get() const { return g_.load(); }
      
      void set(std::size_t g){ g_.store(g); }

      HPX_DEFINE_COMPONENT_ACTION(granularity, set, set_action);
      HPX_DEFINE_COMPONENT_CONST_ACTION(granularity, get, get_action);      

    private:
      boost::atomic<std::size_t> g_;
    };
  }
  
  namespace stubs
  {
    struct granularity : hpx::components::stub_base<server::granularity>
    {
      static void set_sync(hpx::naming::id_type const& gid, std::size_t const& g)
      {
        hpx::async<server::granularity::set_action>(gid,g).get();
      }

      static std::size_t get_sync(hpx::naming::id_type const& gid)
      {
        return hpx::async<server::granularity::get_action>(gid).get();
      }

      static void set_async(hpx::naming::id_type const& gid, std::size_t const& g)
      {
        hpx::apply<server::granularity::set_action>(gid,g);
      }

      static hpx::lcos::future<std::size_t> get_async(hpx::naming::id_type const& gid)
      {
        return hpx::apply<server::granularity::get_action>(gid);
      }
    };
  }

  class granularity
    : public hpx::components::client_base<granularity, stubs::granularity>
  {
    typedef hpx::components::client_base<granularity, stubs::granularity>
    base_type;

  public:
    granularity(){}
    granularity(hpx::naming::id_type const& gid)
      : base_type(gid)
    {}

    void set_sync(std::size_t const& g)
    {      
      BOOST_ASSERT(this->gid_);
      this->base_type::set_sync(this->gid_,g); 
    }

    std::size_t get_sync()
    {
      BOOST_ASSERT(this->gid_);
      return this->base_type::get_sync(this->gid_);
    }

    void set_async(std::size_t const& g)
    {
      BOOST_ASSERT(this->gid_);
      this->base_type::set_async(this->gid_,g); 
    }

    hpx::lcos::future<std::size_t> get_async()
    {
      BOOST_ASSERT(this->gid_);
      return this->base_type::get_async(this->gid_);
    }
  };
}

HPX_REGISTER_ACTION_DECLARATION_EX(nt2::server::granularity::get_action, granularity_get_action);
HPX_REGISTER_ACTION_DECLARATION_EX(nt2::server::granularity::set_action, granularity_set_action);

#endif
