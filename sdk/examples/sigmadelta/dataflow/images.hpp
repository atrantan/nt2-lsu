//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_EXAMPLES_SIGMADELTA_DATAFLOW_IMAGES_HPP
#define NT2_SDK_EXAMPLES_SIGMADELTA_DATAFLOW_IMAGES_HPP

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
    ar & M;
    ar & D;
    ar & V;
    ar & h;
    ar & w;
    ar & Ni;
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    ar & I;
    ar & Fond;
    ar & Estimee;
    ar & M;
    ar & D;
    ar & V;
    ar & h;
    ar & w;
    ar & Ni;
  } 
    
  std::vector< Matrix<boost::uint8_t> > I, Fond, Estimee; //Vecteurs d'Images 
  Matrix<boost::uint8_t> M, D, V; //Matrices Moyennes temporaires
  std::size_t h, w, Ni; //Dimension des images
  
  Images(std::size_t h_=1, std::size_t w_=1, std::size_t Ni_=1)
  :Fond(Ni_, Matrix<boost::uint8_t>(h_,w_)), 
   Estimee(Ni_, Matrix<boost::uint8_t>(h_,w_,255)),
   M(h_,w_),D(h_,w_),V(h_,w_,1),
   h(h_), w(w_), Ni(Ni_)
  {};
  
};

#endif // IMAGES_H
