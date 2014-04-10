
#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>
#include <iostream>
#include <algorithm> 

#define FMULS_GETRF(__m, __n) (0.5 * (double)(__n) * \
((double)(__n) * ((double)(__m) - (1./3.) * (__n) - 1. ) \
+ (double)(__m)) + (2. / 3.) * (__n))

#define FADDS_GETRF(__m, __n) (0.5 * (double)(__n) * ((double)(__n) * \
((double)(__m) - (1./3.) * (__n)) - (double)(__m)) + (1. / 6.) * (__n))

#define FLOPS_DGETRF(__m, __n) (FMULS_GETRF((__m), (__n)) \
+  FADDS_GETRF((__m), (__n)) )



unsigned long _timer_h_G_totaltime = 0;
unsigned long _timer_h_G_starttime = 0;
unsigned long _timer_h_G_endtime = 0;

void start_timer() {
  struct timeval time;
  gettimeofday(&time, NULL);
  _timer_h_G_starttime = time.tv_usec + time.tv_sec * 1000000;
};

void stop_timer() {
  struct timeval time;
  gettimeofday(&time, NULL);
  _timer_h_G_endtime = time.tv_usec + time.tv_sec * 1000000;
  _timer_h_G_totaltime = _timer_h_G_endtime - _timer_h_G_starttime;
};

void print_timer() {
  std::cout << "total time:" << ((double)_timer_h_G_totaltime/1000.0) \
    <<  "ms" << std::endl;
}

template<class RandAccessIter, typename T> inline
void median(RandAccessIter begin, RandAccessIter end, T& res)
{
  std::size_t size = end - begin;
  std::size_t middleIdx = size/2;
  RandAccessIter target = begin + middleIdx;
  std::nth_element(begin, target, end);

  if(size % 2) res = *target;
  else
  {
T a = *target;
RandAccessIter targetNeighbor= target-1;
std::nth_element(begin, targetNeighbor, end);
res = (a+*targetNeighbor)/T(2);
  }
}


template<class Test> inline
void perform_benchmark(Test& test, std::size_t maxiter)
{
std::vector<double> time;
double median_time;
double perfo;
std::size_t size = test.N;

  for(std::size_t i=0; i<maxiter; i++)
    {
        test.reset();
        start_timer();
        test();
        stop_timer();
        time.push_back(((double)_timer_h_G_totaltime/1000.0));
    }
  median(time.begin(),time.end(), median_time);

  perfo = FLOPS_DGETRF(size,size)/(median_time*1.e6);

  std::cout << "performances:" << \
  perfo <<  "GFlops" << \
  std::endl;
}

#endif
