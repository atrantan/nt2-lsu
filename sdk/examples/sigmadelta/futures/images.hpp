#ifndef IMAGES_H
#define IMAGES_H

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/serialization/split_member.hpp>

#include "matrix.hpp"

class Images
{
  public: 
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
    
  template<class Archive>
  void save(Archive & ar, const unsigned int version) const
  {
    ar & I;
    ar & Fond;
    ar & Estimee;
    ar & h;
    ar & w;
    ar & Ni;
    ar & bloc_i;
    ar & bloc_j;
    ar & M;
    ar & V;
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    ar & I;
    ar & Fond;
    ar & Estimee;
    ar & h;
    ar & w;
    ar & Ni;
    ar & bloc_i;
    ar & bloc_j;
    ar & M;
    ar & V;
  } 
    
  std::vector< Matrix<boost::uint8_t> > I, Fond, Estimee; // Vecteurs d'Images
  std::size_t h, w, Ni, bloc_i, bloc_j;		          // Dimensions des Images et des Blocs
  Matrix<boost::uint8_t> M, V;				  // Matrices Moyennes temporaires 
  
  Images(std::size_t h_=1, std::size_t w_=1, std::size_t Ni_=1, std::size_t bi=1, std::size_t bj=1)
  :Fond(Ni_, Matrix<boost::uint8_t>(h_,w_)), 
   Estimee(Ni_, Matrix<boost::uint8_t>(h_,w_,255)), 
   h(h_), w(w_), Ni(Ni_), bloc_i(bi), bloc_j(bj),
   M(h_,w_),V(h_,w_,1)
  {};
  
};

#endif // IMAGES_H
