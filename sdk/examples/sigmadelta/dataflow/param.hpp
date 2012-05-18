//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_EXAMPLES_SIGMADELTA_DATAFLOW_PARAM_HPP
#define NT2_SDK_EXAMPLES_SIGMADELTA_DATAFLOW_PARAM_HPP

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/serialization/split_member.hpp>

class Param
{
public:
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
    
  template<class Archive>
  void save(Archive & ar, const unsigned int version) const
  {
    ar & n;
    ar & ii;
    ar & jj;
    ar & bloc_i;
    ar & bloc_j;
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    ar & n;
    ar & ii;
    ar & jj;
    ar & bloc_i;
    ar & bloc_j;
  } 
  
  std::size_t n;  		 // Numero de l'image
  std::size_t ii;  		 // Indice ligne du bloc
  std::size_t jj;   		 // Indice colonne du bloc
  std::size_t bloc_i;	         // Hauteur du bloc
  std::size_t bloc_j;		 // Largeur du bloc
  
  Param(std::size_t n_=1, std::size_t ii_=1, std::size_t jj_=1,
        std::size_t bi=1, std::size_t bj=1)
       : n(n_), ii(ii_), jj(jj_), bloc_i(bi), bloc_j(bj)
  {}
};

#endif // PARAM_H
