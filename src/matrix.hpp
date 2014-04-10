#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>


template <class T>
class Matrix
{
  public:

  std::size_t height;
  std::size_t width;
  std::vector<T> data;

  Matrix(std::size_t h=0, std::size_t w=0, T const & c=T())
  :height(h),width(w),data(w*h,c)
  {}

  Matrix(size_t h, size_t w, std::vector<T> &d)
  :height(h),width(w),data(d)
  {}

	~Matrix() {}

  T& operator()(std::size_t i, std::size_t j)
  { return data[i+j*height]; }

  T const & operator()(std::size_t i, std::size_t j)const
  { return data[i+j*height]; }

};

#endif // MATRIX_H
