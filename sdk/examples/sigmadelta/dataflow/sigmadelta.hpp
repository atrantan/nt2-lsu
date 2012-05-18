//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_EXAMPLES_SIGMADELTA_DATAFLOW_SIGMADELTA_HPP
#define NT2_SDK_EXAMPLES_SIGMADELTA_DATAFLOW_SIGMADELTA_HPP

#include <hpx/hpx.hpp>
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/components/dataflow/dataflow.hpp>
#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include "pgm.hpp"
#include "images.hpp"
#include "matrix.hpp"
#include "param.hpp"


Images * initialisation(std::size_t h, std::size_t w, std::size_t Ni);

/**********************************************************************/
/*	 Encapsulation des valeurs de retour dans des flots           */
/**********************************************************************/
typedef hpx::lcos::dataflow_base<int>  M_flow;
typedef hpx::lcos::dataflow_base<int>  D_flow;
typedef hpx::lcos::dataflow_base<int>  V_flow;

/**********************************************************************/
/*		 Prototypes des fonctions utilisées                   */
/**********************************************************************/
int Mcompute(Param, Images *, int);
int Dcompute(Param, Images *, int);
int Vcompute(Param, Images *, int, int);

/**********************************************************************/
/*     Encapsulation des fonctions utilisées dans des actions         */
/**********************************************************************/
    typedef hpx::actions::plain_result_action3<
    int,			 // result
    Param,			 // argument
    Images *,     		 // argument
    int,		         // argument
    Mcompute     		 // wrapped function
    > Mcompute_action;
    
    typedef hpx::actions::plain_result_action3<
    int,                        // result
    Param,			// argument
    Images *,      		// argument
    int,		        // argument
    Dcompute     		// wrapped function
    > Dcompute_action;
    
    typedef hpx::actions::plain_result_action4<
    int,                        // result
    Param,			// argument
    Images *,                   // argument
    int,		        // argument
    int,                        // argument
    Vcompute     		// wrapped function
    > Vcompute_action;

/**********************************************************************/
/*     Encapsulation des actions dans des dataflow asynchrones        */
/**********************************************************************/ 
    typedef hpx::lcos::dataflow<Mcompute_action>  M_async;
    typedef hpx::lcos::dataflow<Dcompute_action>  D_async;
    typedef hpx::lcos::dataflow<Vcompute_action>  V_async;

  
/**********************************************************************/
/*			Calcul de M	                              */
/**********************************************************************/
    
int Mcompute(Param p, Images * im, int Mold)
{
  std::size_t const & size_i(p.bloc_i);
  std::size_t const & size_j(p.bloc_j);
  std::size_t const & n(p.n);
  std::size_t const & ii(p.ii);
  std::size_t const & jj(p.jj);
  
/* Traitement de la premiere Image */  
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

/* Sinon traitement de l'Image n*/  

  for(std::size_t i=0; i<size_i; i++) 
  for(std::size_t j=0; j<size_j; j++)
  {       
    boost::uint8_t const & I(im->I[n](i+ii,j+jj));      
    boost::uint8_t & mt (im->M(i+ii,j+jj));
    
    // Calcul de M  
    if (mt < I) 
    mt = mt + 1; 
    else if (mt > I) 
    mt = mt - 1;
    
    im->Fond[n](i+ii,j+jj) = mt;  
  }
  return 0;
}

/**********************************************************************/
/*			Calcul de D	  			      */
/**********************************************************************/

int Dcompute(Param p, Images * im, int M)
{
  std::size_t const & size_i(p.bloc_i);
  std::size_t const & size_j(p.bloc_j);
  std::size_t const & n(p.n);
  std::size_t const & ii(p.ii);
  std::size_t const & jj(p.jj);
  
  Matrix<boost::uint8_t> D(size_i,size_j);
  
  for(std::size_t i=0; i<size_i; i++) 
  for(std::size_t j=0; j<size_j; j++)
  {    
    boost::uint8_t const & I( im->I[n](i+ii,j+jj) );      
    boost::uint8_t const & mt( im->M(i+ii,j+jj) );
   
    // Calcul de D
    if (mt < I)
    im->D(i+ii,j+jj) = I - mt;	      
    else
    im->D(i+ii,j+jj) = mt - I;     
  }
  return 0;
}

/**********************************************************************/
/*			Calcul de V				      */
/**********************************************************************/

int Vcompute(Param p, Images * im, int Vold, int D)
{
  std::size_t const N(3);
  
  std::size_t const & size_i(p.bloc_i);
  std::size_t const & size_j(p.bloc_j);
  std::size_t const & n(p.n);
  std::size_t const & ii(p.ii);
  std::size_t const & jj(p.jj);
 
/* Traitement de la premiere Image */  
  if (n < 1) 
  return 0;

/* Sinon Traitement de l'Image n */
                    
  for(std::size_t i=0; i<size_i; i++) 
  for(std::size_t j=0; j<size_j; j++)
  {    
      boost::uint8_t & vt( im->V(i+ii,j+jj) );
      boost::uint8_t const & d( im->D(i+ii,j+jj) );    
	    
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

/******************************************************************************************/
/*				Initialisation						  */
/******************************************************************************************/

Images * initialisation(std::size_t h, std::size_t w, std::size_t Ni)
{    
      Images* im_ptr = new Images(h, w, Ni);
        
      for (std::size_t n=0; n<Ni; n++)
      { std::ostringstream nom_fichier;	
	nom_fichier<<"./frame/frame."<<std::setfill('0')<<std::setw(3)<<n<<".pgm";
	Pgm mon_image(nom_fichier.str()); 
	im_ptr->I.push_back(Matrix<boost::uint8_t>(h,w,mon_image.pixel));
      }
      
      return im_ptr;
}

#endif

