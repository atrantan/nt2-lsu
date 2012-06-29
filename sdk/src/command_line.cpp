//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#include <boost/simd/sdk/config/os.hpp>

#include <vector>
#include <string>

#ifdef BOOST_SIMD_OS_LINUX

#elif BOOST_SIMD_OS_MAC_OS

#elif BOOST_SIMD_OS_WINDOWS

#endif

namespace nt2 { namespace details 
{
  bool get_args(std::vector<std::string>& args)
  {
#ifdef BOOST_SIMD_OS_LINUX

#elif BOOST_SIMD_OS_MAC_OS

#elif BOOST_SIMD_OS_WINDOWS

#else 
    return false;
#endif
  }

} }
