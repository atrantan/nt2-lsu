#ifndef SIGMADELTA_H
#define SIGMADELTA_H

#include <hpx/hpx.hpp>
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/iostreams.hpp>

#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/ref.hpp>

#include "pgm.hpp"
#include "images.hpp"
#include "matrix.hpp"

/**************************************************************************/
/*		Prototypes des fonctions utilisées			  */
/**************************************************************************/

int sigmadelta(std::size_t n, std::size_t ii, std::size_t jj, Images * im);

Images * initialisation(std::size_t Ni, std::size_t h, std::size_t w, 
                        std::size_t bloc_i, std::size_t bloc_j);

/**************************************************************************/
/*	Encapsulation de la valeur de retour dans une future              */
/**************************************************************************/
typedef hpx::lcos::future< int > sigmadelta_future;

/**************************************************************************/
/*	Encapsulation de la fonction sigmadelta dans une action           */
/**************************************************************************/
typedef hpx::actions::plain_result_action4<
    int,      // result type
    std::size_t,   // argument
    std::size_t,   // argument
    std::size_t,   // argument
    Images *,      // argument
    sigmadelta     // wrapped function
    > sigmadelta_action;

/**************************************************************************/
/*			Fonction sigmadelta            			  */
/**************************************************************************/
int sigmadelta(std::size_t n, std::size_t ii, std::size_t jj, Images * im)
{
  std::size_t const N(3);
  std::size_t const & size_i(im->bloc_i);
  std::size_t const & size_j(im->bloc_j);
  
  /* Traitement de la premiere image */

  if (n < 1)
  {       
    for(std::size_t i=0; i<size_i; i++) 
    for(std::size_t j=0; j<size_j; j++)
      {
	boost::uint8_t const & I(im->I[0](i+ii,j+jj));
	im->M(i+ii,j+jj) = I; 
	im->Fond[0](i+ii,j+jj) = I;
      }
      
    return 0;
  }
  
  /* Sinon */

  // Creation d'une future pour attendre les 
  // résultats des images précédentes

  sigmadelta_future res = 
  hpx::lcos::async<sigmadelta_action>(hpx::find_here(), n-1, ii, jj, im);

  res.get();
  
  /* Traitement de la n-ième image */

  for(std::size_t i=0; i<size_i; i++)  
  for(std::size_t j=0; j<size_j; j++)
	{   	  
	  boost::uint8_t const & I(im->I[n](i+ii,j+jj));
	  boost::uint8_t d;        
	  boost::uint8_t & mt = im->M(i+ii,j+jj);
	  boost::uint8_t & vt = im->V(i+ii,j+jj);
	  
	  // Calcul de M  
	  if (mt < I) 
	  mt = mt + 1; 
	  else if (mt > I) 
	  mt = mt - 1;
	  
	  im->Fond[n](i+ii,j+jj) = mt;  
	  
	  // Calcul de D
	  if (mt < I)
	  d = I - mt;	      
	  else
	  d = mt - I;
		  
	  // Calcul de V  
	  if (d != 0) 
	  { 
	    if (vt < N*d) 
	    vt = vt + 1;		
	    else if (vt > N*d)  	 
	    vt =  vt - 1;
	  }
	  
	  // calcul de E
	  if (d > vt)  
	  im->Estimee[n](i+ii,j+jj) = 0; 
      }

  return 0;
}

/**************************************************************************/
/*			Fonction initialisation       			  */
/**************************************************************************/
Images * initialisation(std::size_t Ni, std::size_t h, std::size_t w, 
                        std::size_t bloc_i, std::size_t bloc_j)
{    
   Images* im_ptr = new Images(h, w, Ni, bloc_i, bloc_j);

   for (std::size_t n=0; n<Ni; n++)
   { std::ostringstream nom_fichier;	
     nom_fichier<<"./frame/frame."<<std::setfill('0')<<std::setw(3)<<n<<".pgm";
     Pgm mon_image(nom_fichier.str()); 
     im_ptr->I.push_back(Matrix<boost::uint8_t>(h,w,mon_image.pixel));
   }
      
      return im_ptr;
}

#endif

