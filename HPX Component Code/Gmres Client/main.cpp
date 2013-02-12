#include "gmres_test.h"
#include "benchmark.hpp"

#include <hpx/hpx_init.hpp>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

using namespace details;

int hpx_main(boost::program_options::variables_map& vm)
{
    
    std::size_t m		= vm["m"].as<std::size_t>();
    std::size_t max_it		= vm["max_it"].as<std::size_t>();
    std::size_t Nblocs		= vm["Nblocs"].as<std::size_t>();    
    std::string Mfilename	= vm["Mfilename"].as<std::string>();
    std::size_t Locs 		= vm["Locs"].as<std::size_t>();
    
     gmres_test test (m,max_it,Nblocs,Mfilename,Locs);

    // Perform benchmark
//      details::benchmark_result<cycles_t> dv;
//      details::perform_benchmark(test,1., dv);
//      std::cout <<hpx::get_os_thread_count()<<" "<<dv.median/(1e6)<< "e+06\n";
    
   test();
   test();
 
    std::cout<<std::endl;
    
    return hpx::finalize(); // Initiate shutdown of the runtime system.
}

int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");
    
    desc_commandline.add_options()
    ( "m"
    , boost::program_options::value<std::size_t>()->default_value(30)
    , "number of iterations between restarts")
    ;
    desc_commandline.add_options()
    ( "max_it"
    , boost::program_options::value<std::size_t>()->default_value(1000)
    , "maximum number of iterations")
    ;
    desc_commandline.add_options()
    ( "Nblocs"
    , boost::program_options::value<std::size_t>()->default_value(5)
    , "number of blocks for tiling")
    ;
    desc_commandline.add_options()
    ( "Mfilename"
    , boost::program_options::value<std::string>()->default_value("../add20.mtx")
    , "filename of Matrix to use")
    ;
    desc_commandline.add_options()
    ( "Locs"
    , boost::program_options::value<std::size_t>()->default_value(3)
    , "Locs for the GSArnoldi function")
    ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
//]
