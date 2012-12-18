#include <algorithm>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <sys/time.h>


namespace details
{  /** Type cycles_t **/
    typedef boost::uint64_t cycles_t ;
    
   /** Fonction read_cycles **/ 
    inline cycles_t read_cycles()
    {
      boost::uint32_t hi = 0, lo = 0;
      __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
      cycles_t that = static_cast<cycles_t>(lo)
                    | ( static_cast<cycles_t>(hi)<<32 );
      return that;
    }       
    
   /** Fonction now **/ 
    inline double now()
    {
      struct timeval tp;
      gettimeofday(&tp,NULL);
      return double(tp.tv_sec) + double(tp.tv_usec)*1e-6;
    }
    
   /** Structure benchmark_result **/ 
    template <typename T>
    struct benchmark_result
    {
    benchmark_result(): median(0.){}
    std::vector<T> data;
    T median;
    };
    
   /** Fonction median **/ 
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
       
   /** Fonction perform_benchmark **/    
  template<class Test, typename T> inline
  void perform_benchmark(Test& test, double duration, benchmark_result<T>& res )
  {
    std::vector<T> cycles;
    double t(0.),vt(0.);
    T vc;
    
      do
        {
          vt = details::now();
          {
            vc = details::read_cycles();
            test();
            vc = details::read_cycles() - vc;
          }
          vt = details::now() - vt;
          t += vt;
          cycles.push_back(vc);
        } while(t < duration);
      res.data = cycles;
      median(cycles.begin(),cycles.end(), res.median);
  }
        
}