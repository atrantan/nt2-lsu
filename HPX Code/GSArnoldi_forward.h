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

typedef hpx::lcos::future< std::vector<double> > Hkt_future;
typedef hpx::lcos::future<void> w_future;

std::vector<double> GS_HkAdd(std::size_t k, Hkt_future const & Hk1, Hkt_future const & Hk2);
std::vector<double> GS_Hkcompute(Param_ptr p, std::size_t i, int blocsize, std::size_t k);
void GS_wcompute(Param_ptr p, std::size_t i, std::size_t blocsize, std::size_t k);


#endif // GSARNOLDI_FORWARD_H
