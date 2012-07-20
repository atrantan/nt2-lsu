//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

#include <hpx/hpx.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

#include <nt2/core/functions/hpx/transform.hpp>

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::
managed_component<nt2::server::transforms_server> transforms_server_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(transforms_server_type, transforms_server);
