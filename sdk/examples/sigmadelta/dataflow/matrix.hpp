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
  
  std::size_t height; //Hauteur de la matrice
  std::size_t width;  //Largeur de la matrice
  std::vector<T> data; //Vecteur de données
 
   // Constructeur par défaut sans paramètres 
   Matrix()
   :height(0),width(0)
   {}
   
   // Constructeur à 2 paramètres
   Matrix(std::size_t h, std::size_t w)
   :height(h),width(w)
   {
     data.resize(h*w);
   }
    
   // Constructeur à 3 paramètres 
   Matrix(std::size_t h, std::size_t w, T const & c)
   :height(h),width(w),data(w*h,c)
   {}
  
  // Constructeur avec un vecteur paramètre
  Matrix(size_t h, size_t w, std::vector<T> &d)
  :height(h),width(w),data(d)
  {}
  
  // Accès en lecture/ecriture
  T& operator()(std::size_t i, std::size_t j)
  {
    return data[i*width+j];
  }
  
  // Accès en lecture seule
  T const & operator()(std::size_t i, std::size_t j)const
  {
    return data[i*width+j];
  }  
  
};

#endif // MATRIX_H
