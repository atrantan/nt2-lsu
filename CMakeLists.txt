cmake_minimum_required(VERSION 2.8)
project(TiledLU)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{HPX_ROOT}/share/cmake-2.8/Modules")

#*******************
# plasma
#*******************
set(plasma_link_flags "-L$ENV{PLASMA_ROOT}/lib -lcoreblas -lplasma -lquark -lcoreblas -L$ENV{PLASMA_ROOT}/lib -lcblas -L$ENV{PLASMA_ROOT}/lib -llapacke -L$ENV{PLASMA_ROOT}/lib -ltmg -llapack -L/opt/intel/mkl/lib/intel64 -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -L/opt/intel/lib/intel64 -limf -lpthread -lm -lhwloc -lgfortran")

#*******************
# mkl
#*******************
set(mkl_link_flags "-L/opt/intel/mkl/lib/intel64 -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread")

#*******************
# tbb
#*******************
set(tbb_link_flags "-L$ENV{TBBROOT}/lib -ltbb")

include_directories($ENV{PLASMA_ROOT}/include /opt/intel/mkl/include $ENV{TBBROOT}/include)

#*******************
# hpx
#*******************

find_package(HPX REQUIRED)
if (HPX_FOUND)
message(STATUS "HPX was found.")
endif (HPX_FOUND)

include_directories(${HPX_INCLUDE_DIR})
link_directories(${HPX_LIBRARY_DIR})

option(debug "debug" OFF)

if(debug MATCHES ON)
add_definitions(-g -DDEBUG)
endif(debug MATCHES ON)

add_definitions(-O3 -fomit-frame-pointer -std=c++0x)

add_subdirectory(src)
