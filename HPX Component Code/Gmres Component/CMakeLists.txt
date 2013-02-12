cmake_minimum_required(VERSION 2.8)

project(hpx_gemres)

# bebob libraries
set(bebop_lib_dir
    "~/bebop/sparse_matrix_converter"
    "~/bebop/bebop_util"
   )

set(bebop_include_dir
    "~/bebop/sparse_matrix_converter/include"
    "~/bebop/bebop_util/include"
   )

set(mkl_lib_dir
    "/opt/intel/mkl/lib/intel64"
    "/opt/intel/lib/intel64"
   )

set(mkl_include_dir
    "/opt/intel/mkl/include"
   )

set(dependencies
    sparse_matrix_converter
    bebop_util   
    mkl_intel_lp64 
    mkl_sequential 
    mkl_core  
    pthread
    iostreams
   )

set(sources
    gmres_component.cpp
    )

find_package(PkgConfig)
pkg_check_modules(HPX_VARIABLE QUIET hpx_component)

foreach(tmp ${HPX_VARIABLE_CFLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${tmp}")
endforeach()

include_directories("." ${mkl_include_dir} ${HPX_VARIABLE_INCLUDE_DIRS} ${bebop_include_dir})
link_directories(${mkl_lib_dir} ${HPX_VARIABLE_LIBRARY_DIRS} ${bebop_lib_dir})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/hpx-0.9.5/share/cmake-2.8/Modules")
find_package(HPX)
if (HPX_FOUND)
      message(STATUS "HPX was found.")
endif (HPX_FOUND)

add_hpx_component(gmres ESSENTIAL SOURCES ${sources} DEPENDENCIES ${dependencies})
