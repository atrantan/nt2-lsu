//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================
#ifndef NT2_SDK_EXAMPLES_SIGMADELTA_FUTURES_PGM_HPP
#define NT2_SDK_EXAMPLES_SIGMADELTA_FUTURES_PGM_HPP

#include <fstream>
#include <iostream>
#include <cassert>
#include <math.h>
#include <nt2/table.hpp>
#include <nt2/include/functions/of_size.hpp>

//////////////////////////////////////////////////////////////////
//              Definition de la classe Pgm                     //
//	Represente un fichier image au format Pgm 		//
//////////////////////////////////////////////////////////////////
namespace nt2 { namespace test 
{
  template<class T>
  class Pgm
  {
  public:
    short width;		//largeur
    short height;		//hauteur
    long length;		//nombre d'octets de 'pixel' 
    unsigned long level;		//niveau de R,G,B maximal (255)
    nt2::table<T> pixel;//tableau des pixels
    
    Pgm(std::string fname) 
      : width(0), height(0), 
        length(width*height), 
        level(255)
    {
      std::ifstream in;
      int i = 0;
      
      in.open(fname.c_str(),std::ios::in);
      
      if (!in.is_open()) 
      { 
        std::cout << "Can't read image: " << fname << std::endl;
        exit(1);
      }

      while(in.good())
      {	
        if (in.peek() == '#' || in.peek() == '\n') 
	  in.ignore(256, '\n'); 
        
        else
	{ 	
          if (i == 0){ std::string s; in >> s; }      // Ignore P5
          else if (i == 1)
	  {
            in >> width;               // Lecture width 
            in >> height;              // Lecture height
            length = width*height;
            pixel.resize(nt2::of_size(width,height));
          }

          else if (i == 2) in >> level;  		     // Lecture level
          else if (i == 3) in.read(((char*)pixel.raw()), length); //Lecture image
          ++i;
        }
      }
      
      in.close();
    }
    
    //affecte la couleur 'color' au pixel de coordonnees (I,J)
    void setpixel(short I,short J,T color) 	
    {						  
      assert((I>=0)&&(I<height)&&(J>=0)&&(J<width));
      pixel(I,J)=255&color;
    };
    
    //retourne la couleur 'color' du pixel de coordonnees (I,J)
    T getpixel(short I,short J) 	
    {	
      T couleur;
      T B;
      
      assert((I>=0)&&(I<height)&&(J>=0)&&(J<width));
      B = pixel(I,J);		
      couleur = 0;
      couleur = couleur|B;
      
      return couleur;
    };

    void write(std::string nom)
    {
      std::ofstream out(nom.c_str(),std::ios::binary);
      
      //tester si le fichier est ouvert
      out.seekp(0,std::ios::end);
      if (out.tellp()<0)
      {
        out.close();
        throw("erreur d'ouverture de fichier");
      }
      
      //ecriture de l'en-tête
      out<<"P5\n"<<width<<" "<<height<<"\n"<<level<<"\n";
      
      //ecriture des valeurs des pixels
      out.write((char *)(pixel.raw()),length);
      out.close();
    }
    
  };
} }

#endif
