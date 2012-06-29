//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include <nt2/sdk/hpx/user_main_fwd.hpp>
#include <nt2/sdk/hpx/details/command_line.hpp>

#include <boost/assert.hpp>

#include <vector>
#include <string>

int hpx_main()
{
  std::vector<std::string> args;

  BOOST_ASSERT_MSG( nt2::details::get_args(args)
                  , "Enable to parse command line arguments on this system."
                  );
  int argc = args.size();
  // TODO: 
  char **argv=0;

  int r = nt2::user_main(argc,argv);
  return hpx::finalize();
}

int main(int argc, char **argv)
{
  return hpx::init(argc, argv);
}
