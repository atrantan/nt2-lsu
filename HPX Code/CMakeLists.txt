cmake_minimum_required(VERSION 2.8)

project(hpx_gemres)

# bebob libraries
set(bebop_lib_dir
    "/users/parall/trantan/bebop/sparse_matrix_converter"
    "/users/parall/trantan/bebop/bebop_util"
   )

set(bebop_include_dir
    "/users/parall/trantan/bebop/sparse_matrix_converter/include"
    "/users/parall/trantan/bebop/bebop_util/include"
   )

set(bebop_lib_name 
    sparse_matrix_converter
    bebop_util   
   )

# mkl libraries
set(mkl_lib_dir
    "/opt/intel/mkl/lib/intel64"
    "/opt/intel/lib/intel64"
   )

set(mkl_include_dir
    "/opt/intel/mkl/include"
   )

set(mkl_lib_name 
    mkl_intel_lp64 
    mkl_sequential 
    mkl_core  
    pthread 
   )

# link to executable
set(sources    
    main.cpp
    hpx_gmres_test.cpp
    GSArnoldi_forward.cpp
    )

find_package(PkgConfig)
pkg_check_modules(HPX_VARIABLE QUIET hpx_application)

foreach(tmp ${HPX_VARIABLE_CFLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${tmp}")
endforeach()

include_directories("." ${mkl_include_dir} ${HPX_VARIABLE_INCLUDE_DIRS} ${bebop_include_dir})
link_directories(${mkl_lib_dir} ${HPX_VARIABLE_LIBRARY_DIRS} ${bebop_lib_dir})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/users/parall/trantan/hpx-0.9.0/share/cmake-2.8/Modules")
find_package(HPX)
if (HPX_FOUND)
      message(STATUS "HPX was found.")
endif (HPX_FOUND)

add_hpx_executable(hpx_gmres ESSENTIAL SOURCES ${sources} DEPENDENCIES ${mkl_lib_name} ${bebop_lib_name})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -liostreams")

