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
#include <nt2/sdk/hpx/details/granularity.hpp>

#include <boost/assert.hpp>

#include <vector>
#include <string>

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<
  nt2::server::granularity
  > granularity_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(granularity_type, granularity);

HPX_REGISTER_ACTION_EX(nt2::server::granularity::get_action, granularity_get_action);
HPX_REGISTER_ACTION_EX(nt2::server::granularity::set_action, granularity_set_action);

int hpx_main(boost::program_options::variables_map& vm)
{
  std::vector<std::string> args;
  bool condition = nt2::details::get_args(args);
  BOOST_ASSERT_MSG( condition == true
                  , "Enable to parse command line arguments on this system."
                  );
  int argc = args.size();
  char **argv= (char**)malloc(argc*sizeof(char*));

  std::vector<std::string>::iterator it = args.begin();
  for(std::size_t i=0; i < args.size(); i++)
  {
    std::string tmp = *it++;
    tmp+='\0';
    argv[i] = (char*)malloc((tmp.size())*sizeof(char));
    tmp.copy(argv[i], tmp.size(), 0);
  }

  {
    nt2::granularity g_cmd;
    g_cmd.create(hpx::find_here());
    g_cmd.set_sync(vm["nt2-granularity"].as<std::size_t>());
    nt2::user_main(argc,argv);
  }
  return hpx::finalize();
}

int main(int argc, char *argv[])
{
  boost::program_options::options_description cmdline("Usage: " HPX_APPLICATION_STRING " [options]");
  
  cmdline.add_options()
    ("nt2-granularity", boost::program_options::value<std::size_t>()->default_value(1), "The number of hpx action");
    
  return hpx::init(cmdline, argc, argv);
}
