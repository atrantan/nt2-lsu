cmake_minimum_required(VERSION 2.8)

project(Petsc_gmres)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++0x -Wall -Wextra")

# petsc libraries
set(petsc_include_dir
    "/users/parall/trantan/petsc/include"
    "/users/parall/trantan/boost_old/include"
   )

set(petsc_lib_dir
    "/users/parall/trantan/petsc/lib"
   )


set(petsc_lib_name
    petsc
    lapack
    blas
    superlu_dist_3.1
    parmetis 
    metis
    X11    
   )
   
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

include_directories("." ${petsc_include_dir} ${bebop_include_dir})
link_directories(${petsc_lib_dir} ${bebop_lib_dir})


set(sources
    main.cpp
    )

add_executable(petsc_gmres ${sources})
target_link_libraries(petsc_gmres ${petsc_lib_name} ${bebop_lib_name})
