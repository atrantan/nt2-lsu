#ifndef PARAM_H
#define PARAM_H

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
