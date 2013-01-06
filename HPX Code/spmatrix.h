#ifndef SPMATRIX_H
#define SPMATRIX_H

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/move/move.hpp>
#include <boost/format.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/iterator/iterator_concepts.hpp>

template <class T>
class Spmatrix
{
  public:
  
BOOST_COPYABLE_AND_MOVABLE(Spmatrix)    
    
friend class boost::serialization::access;

template <typename Archive>
void serialize(Archive & ar, unsigned)
  { ar& values;
    ar& indices;
    ar& rows;
    ar& height;
    ar& width;
  }
    
  std::vector<T> values;
  std::vector<int> indices;
  std::vector<int> rows;  
  int height;
  int width;
  
  T& operator [](int j)
  {
    return values[j];
  }
  
  T const & operator [](int j) const
  {
    return values[j];
  }
      
  Spmatrix(int h=0, int w=0)
  :height(h),width(w)
  {} 
  
  Spmatrix(const Spmatrix& p) // Copy constructor (as usual)
  : height(p.height),width(p.width),
    values(p.values),rows(p.rows),indices(p.indices)
  {}

  Spmatrix& operator=(BOOST_COPY_ASSIGN_REF(Spmatrix) p) // Copy assignment
   {
      if (this != &p){
      height = p.height;
      width = p.width;
      values = p.values;
      rows = p.rows;
      indices = p.indices;
      }
      return *this;
   }

   //Move semantics...
   Spmatrix(BOOST_RV_REF(Spmatrix) p) //Move constructor
   {
      height = p.height;
      width = p.width;
      values = p.values;
      rows = p.rows;
      indices = p.indices;
   }

   Spmatrix& operator=(BOOST_RV_REF(Spmatrix) p) //Move assignment
   {
      if (this != &p){	
      height = p.height;
      width = p.width;
      values = p.values;
      rows = p.rows;
      indices = p.indices;
      }
      return *this;
   } 
};

#endif // MATRIX_H
