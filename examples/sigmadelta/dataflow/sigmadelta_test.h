#ifndef SIGMADELTA_TEST_H
#define SIGMADELTA_TEST_H

#include "sigmadelta.h"
#include <boost/foreach.hpp>


class Sigmadelta_test
{
   public: 
   std::size_t const Ni;	// Nombre d'images
   std::size_t const h;		// Hauteur
   std::size_t const w; 	// Largeur
   std::size_t const bi;    	// hauteur du bloc
   std::size_t const bj;    	// largeur du bloc
   std::size_t const Nbi,Nbj;   // Nombre de blocs
   std::size_t src, dst;        // Indices pour swapper 
   
   Images *im;		        // Images original, fond, estimee
    
   // Constructeur par defaut
    Sigmadelta_test():
    Ni(181), h(1080), w(1920), bi(180), bj(960), Nbi(h/bi),Nbj(w/bj),src(0),dst(1)
    {      
      im = initialisation(h, w, Ni);
      
    };
    
    // Operation
    void operator()() 
    {  
     // Création de 2 Matrices de flots pour chaque etape (M, D, V)
      std::vector< Matrix<M_flow> > m(2, Matrix<M_flow>(Nbi,Nbj));
      std::vector< Matrix<D_flow> > d(2, Matrix<D_flow>(Nbi,Nbj));
      std::vector< Matrix<V_flow> > v(2, Matrix<V_flow>(Nbi,Nbj));          
      
     // Affectation de dataflows(1 par bloc) aux Matrices sources
      for(size_t ii=0, i=0; ii<h; ii+=bi, i++)
      for(size_t jj=0, j=0; jj<w; jj+=bj, j++)	
      {	      
	Param const p(0,ii,jj,bi,bj);	    
	m[src](i,j)= M_async(hpx::find_here(),p,im,0);
	d[src](i,j)= D_async(hpx::find_here(),p,im,m[src](i,j));
	v[src](i,j)= V_async(hpx::find_here(),p,im,0,d[src](i,j));
      }      
    
    // Creation de l'arbre des dépendances    
    for(size_t n=1; n<Ni; n++)
    {
	// Affectation de dataflows(1 par bloc)
        // Matrices sources --> Matrices destinataires
	for(size_t ii=0, i=0; ii<h; ii+=bi, i++)
	for(size_t jj=0, j=0; jj<w; jj+=bj, j++)	
	{	      
	  Param const p(n,ii,jj,bi,bj);	    
	  m[dst](i,j)= M_async(hpx::find_here(),p,im,m[src](i,j));
	  d[dst](i,j)= D_async(hpx::find_here(),p,im,m[dst](i,j));
	  v[dst](i,j)= V_async(hpx::find_here(),p,im,v[src](i,j),d[dst](i,j));
	}
        // Matrices destinataires deviennent Matrices sources
	boost::swap(src,dst);
    }
      
     /* Barriere de synchronisation */	
       BOOST_FOREACH(V_flow & f, v[src].data)
       f.get_future().get();
	  
    };
};

#endif // SIGMADELTA_TEST_H
