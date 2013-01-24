#ifndef GSARNOLDI_FORWARD_H
#define GSARNOLDI_FORWARD_H

#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/lcos/future_wait.hpp>

#include "param.h"
#include "matrix.h"
#include "spmatrix.h"
#include "benchmark.hpp"

typedef hpx::lcos::future<std::size_t> Hkt_future;
typedef hpx::lcos::future<double> w_future;
typedef hpx::lcos::future<double> r0_future;
typedef hpx::lcos::future<void> x_future;

double GMRES_V0compute(Param_ptr const & p, std::size_t i, int blocsize);
std::size_t GS_HkAdd(Param_ptr const & p, std::size_t k, Hkt_future const & Hk1, Hkt_future const & Hk2);
std::size_t GS_Hkcompute(Param_ptr const & p, std::size_t i, int blocsize, std::size_t k, std::size_t id);
double GS_wcompute(Param_ptr const & p, std::size_t i, std::size_t blocsize, std::size_t k);
void GMRES_xcompute(Param_ptr const & p, std::size_t i, std::size_t blocsize, std::size_t k);


#endif // GSARNOLDI_FORWARD_H
