add_executable(lu_plasma lu_plasma.cpp)
add_executable(lu_mkl kernels.c lu_mkl.cpp)
add_executable(lu_tbb kernels.c lu_tbb.cpp)

add_hpx_executable(lu_hpx       ESSENTIAL
                                SOURCES kernels.c lu_hpx.cpp
                                DEPENDENCIES ${mkl_link_flags}
                                )

add_hpx_executable(lu_dataflow   ESSENTIAL
                                 SOURCES kernels.c lu_dataflow.cpp
                                 DEPENDENCIES ${mkl_link_flags}
                                )

target_link_libraries (lu_plasma ${plasma_link_flags})
target_link_libraries (lu_mkl ${mkl_link_flags})
target_link_libraries (lu_tbb ${tbb_link_flags} ${mkl_link_flags})
