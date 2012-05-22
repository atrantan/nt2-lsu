//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_EXAMPLES_SIGMADELTA_FUTURES_SIGMADELTA_TEST_HPP
#define NT2_SDK_EXAMPLES_SIGMADELTA_FUTURES_SIGMADELTA_TEST_HPP

#include <hpx/hpx.hpp>
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/iostreams.hpp>
#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/ref.hpp>
#include <boost/serialization/split_member.hpp>
#include <nt2/table.hpp>
#include <nt2/include/functions/of_size.hpp>
#include <nt2/include/functions/repnum.hpp>
#include <vector>
#include <algorithm>

namespace nt2{ namespace test 
{
  
  
  template<class T>
  class Images
  {
  public: 

    typedef T scalar_type;
    typedef container::table<scalar_type> table_;

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
    
    Images(std::size_t h_=1, std::size_t w_=1, std::size_t Ni_=1, std::size_t bi=1, std::size_t bj=1)
      : Fond(Ni_, table_(nt2::of_size(h_,w_))), 
        h(h_), w(w_), Ni(Ni_), bloc_i(bi), bloc_j(bj),
        M(h_,w_),V(h_,w_,1)
    {
      std::fill(Estimee.begin(), Estimee.end(), nt2::repnum(255, nt2::of_size(h_, w_))); 
    };
    
    std::vector<table_> I, Fond, Estimee;   // Image vector
    std::size_t h, w, Ni, bloc_i, bloc_j;   
    table_ M, V;	                    // Temporary images
  };

  template<class T>
  int sigmadelta(std::size_t n, std::size_t ii, std::size_t jj, Images<T>* im);

  // Sigma-Delta action constructor
  template<class T>
  struct get_sigmadelta_action
  {
    typedef hpx::actions::plain_result_action4<
      int,                                 // result type
      std::size_t,                         // argument
      std::size_t,                         // argument
      std::size_t,                         // argument
      Images<T>*,                          // argument
      sigmadelta                           // wrapped function
      > type;
  }; 

  // Sigma-Delta future constructor
  template<class T>
  struct get_sigmadelta_future
  {
    // Return type embedded in a future
    typedef hpx::lcos::future<T> type;
  };

  // Sigma-Delta Function
  template<class T>
  int sigmadelta(std::size_t n, std::size_t ii, std::size_t jj, Images<T>* im)
  {
    std::size_t const N(3);
    std::size_t const & size_i(im->bloc_i);
    std::size_t const & size_j(im->bloc_j);
    
    // First frame processing
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
    
    // This future waits for the results of the previous images.
    typename get_sigmadelta_future<int>::type  res = 
      hpx::lcos::async<typename get_sigmadelta_action<T>::type>(hpx::find_here(), n-1, ii, jj, im);
    
    res.get();

    // Nth image processing
    for(std::size_t i=0; i<size_i; i++)  
      for(std::size_t j=0; j<size_j; j++)
      {   	  
	boost::uint8_t const & I(im->I[n](i+ii,j+jj));
	boost::uint8_t d;        
	boost::uint8_t & mt = im->M(i+ii,j+jj);
	boost::uint8_t & vt = im->V(i+ii,j+jj);
	  
	if (mt < I) 
	  mt = mt + 1; 
	else if (mt > I) 
	  mt = mt - 1;
	  
	im->Fond[n](i+ii,j+jj) = mt;  
	  
	if (mt < I)
	  d = I - mt;	      
	else
	  d = mt - I;
	
	if (d != 0) 
	{ 
	  if (vt < N*d) 
	    vt = vt + 1;		
	  else if (vt > N*d)  	 
	    vt =  vt - 1;
	}
	
	if (d > vt)  
	  im->Estimee[n](i+ii,j+jj) = 0; 
      }
    
    return 0;
  }

  template<class T> 
  struct sigmadelta_test
  {
    typedef T scalar_type;
    typedef Images<scalar_type> image_vector_;
    typedef std::vector<get_sigmadelta_future<int>::type> promesses_;
    typedef typename get_sigmadelta_action<scalar_type>::type sigmadelta_action;
    

    sigmadelta_test():
      Ni(181), h(1080), w(1920), bloc_i(180), bloc_j(960)
    {
      im = initialisation(Ni, h, w, bloc_i, bloc_j);      
    }

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

    image_vector_*           im;          
    promesses_        promesses;
    std::size_t const Ni;	// image number
    std::size_t const h;	// height
    std::size_t const w; 	// width
    std::size_t const bloc_i;   // block parameters
    std::size_t const bloc_j;

  protected :
    image_vector_* initialisation(std::size_t Ni, std::size_t h, std::size_t w, 
                            std::size_t bloc_i, std::size_t bloc_j)
    {    
      image_vector_* im_ptr = new image_vector_(h, w, Ni, bloc_i, bloc_j);
      
      for (std::size_t n=0; n<Ni; n++)
      { 
	std::ostringstream nom_fichier;	
	nom_fichier<<"./frame/frame."<<std::setfill('0')<<std::setw(3)<<n<<".pgm";
	// Pgm mon_image(nom_fichier.str()); 
	// im_ptr->I.push_back(table_(h,w,mon_image.pixel));
      }
      
      return im_ptr;
    }
  };

} }

#endif 
