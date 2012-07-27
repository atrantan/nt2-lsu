//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

#include <hpx/hpx.hpp>
#include <hpx/runtime/components/component_factory.hpp>

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>

#include <nt2/sdk/hpx/details/granularity.hpp>

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<
  nt2::server::granularity
  > granularity_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(granularity_type, granularity);

HPX_REGISTER_ACTION_EX(nt2::server::granularity::get_action, granularity_get_action);
HPX_REGISTER_ACTION_EX(nt2::server::granularity::set_action, granularity_set_action);

