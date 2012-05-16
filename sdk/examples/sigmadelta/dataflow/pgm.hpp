///////////////////////////////////////////////////////////
//   header inclus dans tous les fichiers sources//
///////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <cassert>
#include <math.h>
#include <vector>

#ifndef pgm_H
#define pgm_H

////////////////////////////////////
//Redefinition des types d'entiers//
////////////////////////////////////
typedef uint8_t BYTE;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef long LONG;
typedef short SHORT;
typedef char CHAR;

//////////////////////////////////////////////////////////////////
//              Definition de la classe Pgm                     //
//	Represente un fichier image au format Pgm 		//
//////////////////////////////////////////////////////////////////
class Pgm
{
	public:
	SHORT width;		//largeur
	SHORT height;		//hauteur
	LONG length;		//nombre d'octets de 'pixel' 
	ULONG level;		//niveau de R,G,B maximal (255)
	std::vector<BYTE> pixel;//tableau des pixels

	Pgm(ULONG,ULONG,std::vector<BYTE>::iterator);
	Pgm(std::string);
	
	void write(std::string);

	//affecte la couleur 'color' au pixel de coordonnees (I,J)
	void setpixel(SHORT I,SHORT J,BYTE color) 	
	{						  
	  assert((I>=0)&&(I<height)&&(J>=0)&&(J<width));

	  LONG ind=(I*width+J);
	  pixel[ind]=255&color;
	  return;
	};

	//retourne la couleur 'color' du pixel de coordonnees (I,J)
	BYTE getpixel(SHORT I,SHORT J) 	
	{	
	  BYTE couleur;
	  BYTE B;

	  assert((I>=0)&&(I<height)&&(J>=0)&&(J<width));
		  
	  
	  LONG ind=(I*width+J);
	  
	  B = pixel[ind];		

	  couleur = 0;
	  couleur = couleur|B;

	  return couleur;
	};

};

//////////////////////////////////////////////////////////////////
//Pgm(w,h): constructeur affectant la memoire pour une image w*h//
//////////////////////////////////////////////////////////////////
Pgm::Pgm(ULONG w,ULONG h, std::vector<BYTE>::iterator it_tab) 
: width(w), height(h), 
  length(width*height), 
  level(255), pixel(it_tab,it_tab+length)
{}

Pgm::Pgm(std::string fname) 
: width(0), height(0), 
  length(width*height), 
  level(255), pixel(length, 0)
{
	std::ifstream in;
	int i = 0;

	in.open(fname.c_str(),std::ios::in);
	   
	if (!in.is_open()) 
	{ std::cout << "Can't read image: " << fname << std::endl;
	  exit(1);
	}

	while(in.good())
	{	
	  if (in.peek() == '#' || in.peek() == '\n') 
	  in.ignore(256, '\n'); 

	  else
	  { 	
	    if (i == 0)
	    {std::string s; in >> s;}      // Ignore P5

	    else if (i == 1)
	    {
	      in >> width;               // Lecture width 
	      in >> height;              // Lecture height
	      length = width * height;   // Taille de l'image
	      pixel.resize(length);
	    }

	    else if (i == 2)
	    in >> level;  		     // Lecture level

	    else if (i == 3)
	    in.read ((char *)(&(pixel[0])), length); //Lecture image

	    ++i;
	  }
	 }

  in.close();
}

/////////////////////////////////////////////////////////////////////
//write(nom): ecrit les donnees dans le fichier 'nom' (Pgm binaire)//
/////////////////////////////////////////////////////////////////////
void Pgm::write(std::string nom)
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
	out.write((char *)(&(pixel[0])),length);
	out.close();

	return;
}

#endif
