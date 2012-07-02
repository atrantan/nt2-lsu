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
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#elif BOOST_SIMD_OS_MAC_OS

#elif BOOST_SIMD_OS_WINDOWS

#endif

namespace nt2 { namespace details 
{
  bool get_args(std::vector<std::string>& args)
  {
#ifdef BOOST_SIMD_OS_LINUX
    std::string       elem;
    std::stringstream procfile_str;

    procfile_str << "/proc/" << getpid() << "/cmdline";
    std::string name = procfile_str.str();

    std::ifstream proc;
    proc.open(name.c_str(), std::ios::binary);
    
    if(!proc.is_open())
      return false;    
    
    while(!proc.eof())
    {
      char c = proc.get();
      if (c == '\0')
      {
        args.push_back(elem);
        elem.clear();
      }
      else
      {
        elem += c;
      }
    }

    return true;
#elif BOOST_SIMD_OS_MAC_OS
    // TODO
#elif BOOST_SIMD_OS_WINDOWS
    // TODO
#else 
    return false;
#endif
  }

} }
