#ifndef MATRIX_H
#define MATRIX_H

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/serialization/split_member.hpp>

template <class T>
class Matrix
{
  public:
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  template<class Archive>
  void save(Archive & ar, const unsigned int version) const
  {
    ar & height;
    ar & width;
    ar & data;
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    ar & height;
    ar & width;
    ar & data;
  }
  
  std::size_t height;
  std::size_t width;
  std::vector<T> data;
  
  // Constructeur par défaut     
  Matrix(std::size_t h=0, std::size_t w=0, T const & c=T())
  :height(h),width(w),data(w*h,c)
  {}
  
  // Constructeur avec un vecteur paramètre
  Matrix(size_t h, size_t w, std::vector<T> &d)
  :height(h),width(w),data(d)
  {}
  
  // Acces en lecture/ecriture
  T& operator()(std::size_t i, std::size_t j)
  {
    return data[i*width+j];
  }
  
  // Acces en lecture seule
  T const & operator()(std::size_t i, std::size_t j)const
  {
    return data[i*width+j];
  }  
  
};

#endif // MATRIX_H
