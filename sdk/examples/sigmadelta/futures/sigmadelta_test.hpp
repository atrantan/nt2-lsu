#ifndef SIGMADELTA_TEST_H
#define SIGMADELTA_TEST_H

#include "sigmadelta.hpp"

class Sigmadelta_test
{
public:  
   std::size_t const Ni;	// Nombre d'images
   std::size_t const h;		// Hauteur
   std::size_t const w; 	// Largeur
   std::size_t const bloc_i;    // Nombre de blocs
   std::size_t const bloc_j;    // Nombre de blocs


   std::vector<sigmadelta_future> promesses;      // Vecteur de futures
   Images *im;	// Images original, fond, estimee
                // et Matrices Moyennes
   
   // Constructeur par defaut
    Sigmadelta_test():
    Ni(181), h(1080), w(1920), bloc_i(180), bloc_j(960)
    {
      im = initialisation(Ni, h, w, bloc_i, bloc_j);      
    };
    
    // Operation
    void operator()() 
    {  
      // Création des futures pour la dernière image
      // qui vont à leur tour créer des futures pour
      // les images précédentes (cf. Factorielle)
    
      for(size_t ii=0; ii<h; ii+=bloc_i)
      for(size_t jj=0; jj<w; jj+=bloc_j)
      promesses.push_back
      (hpx::lcos::async<sigmadelta_action>(hpx::find_here(), Ni-1, ii, jj, im));          
       hpx::lcos::wait(promesses);      
    };
};

#endif // SIGMADELTA_TEST_H
