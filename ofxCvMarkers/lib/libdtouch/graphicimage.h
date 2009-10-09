/***************************************************************************
                   graphicimage.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Enrico Costanza
    email                : e.costanza@ieee.org
 ***************************************************************************/


/***************************************************************************
 *                                                                         *
 *	This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330                           *
 *  Boston, MA  02111-1307  USA                                            *
 *                                                                         *
 ***************************************************************************/


/*  Changes

	Code optimization by Jorge M Santiago

*/


#ifndef EC_GRAPHICIMAGE
#define EC_GRAPHICIMAGE

//#include <iostream>
#include "region.h"

#include "types.h"
//using std::cout;
//using std::endl;

class GraphicImage{

public:
	//typedef enum { RGB32, YUV422P } ColourFormat;

// constructors & destructor
protected:
	GraphicImage( ) { }
public:
	GraphicImage( int in_width, int in_height );
	virtual ~GraphicImage();

  
// image buffer
protected:
	unsigned char *_displayBuffer;

	int _width;
	int _height;

public:
	int getWidth() { return _width; }
	int getHeight() { return _height; }
	virtual int getDept() = 0;

	unsigned char * getImageBuffer() { return _displayBuffer; } 

	void setImageBuffer( unsigned char **inBuffer ) { _displayBuffer = *inBuffer; }

/*	
protected:
	void box(dtouch_Region *reg, DTPoint &min, DTPoint &max);
	void boxPlus( dtouch_Region *reg, DTPoint &min, DTPoint &max,
                DTPoint &p1, DTPoint &p2 , DTPoint &p3 , DTPoint &p4  );
//*/

// methods related to display:
public:
	virtual void fill(dtouch_Region *reg, int ** labelsMap, unsigned char r, unsigned char g, unsigned char b ) = 0;
	virtual void fill(dtouch_Region *reg, int ** labelsMap ) = 0;

	virtual void markThreshold( unsigned char * proc, unsigned char * thresh ) = 0;
	virtual void fillRect( int x, int y, int w, int h ) = 0;
	
	virtual void mark( const int& x, const int& y ) = 0;
	virtual void mark( const DTPoint& p) = 0;
	virtual void markCross( const DTPoint& p);
	/*
	void outline(dtouch_Region *reg,
                unsigned char r, unsigned char g, unsigned char b );
	void outlineStrong(dtouch_Region *reg,
                unsigned char r, unsigned char g, unsigned char b );
	void mark(PointList *reg, unsigned char r, unsigned char g, unsigned char b );
	void fill(dtouch_Region *reg, int ** labelsMap, unsigned char r, unsigned char g, unsigned char b );
	
	
	void mark(int x, int y, unsigned char r, unsigned char g, unsigned char b );
	

	void plotSegment( DTPoint p1, DTPoint p2,
                  unsigned char r, unsigned char g, unsigned char b );

	void plotLineNorm( int rho, int angle );

	void plotLine( double m, double q );
	// the following is an overloaded version of the one above added for "retrocompatibility"
	// it is just an alias for plotSegment
	void plotLine( DTPoint p1, DTPoint p2,
                  unsigned char r, unsigned char g, unsigned char b ){
		plotSegment( p1, p2, r, g, b );                  
	}

	void plotDigit(DTPoint p, int digit, unsigned char r, unsigned char g, unsigned char b );
	void plotNumber(DTPoint p, int number, unsigned char r, unsigned char g, unsigned char b );

	void fill( int x, int y ){
		unsigned char *ptr = _displayBuffer+3*x+y*3*_width;
		*ptr++ = 0;
		*ptr++ = 0;
		*ptr++ = 0;
	}
	  
	void mapTextureToQuadrilateral( unsigned char *texture, int textureWidth,
                                  int textureHeight, unsigned char *img,
                                  DTPoint p1, DTPoint p2, DTPoint p3, DTPoint p4 );

	void displayTextureOnQuadrilateral( unsigned char *texture, int textureWidth,
                                      int textureHeight,
                                      DTPoint p1, DTPoint p2, DTPoint p3, DTPoint p4 ){
		mapTextureToQuadrilateral( texture,textureWidth,textureHeight,
                                _displayBuffer,p1,p2,p3,p4);
	}
	//*/

};

class GraphicImageRGB : public GraphicImage{
protected:
	GraphicImageRGB() {}
public:
	GraphicImageRGB(int in_width, int in_height ) : GraphicImage( in_width, in_height ) { }
	virtual ~GraphicImageRGB() {}

	virtual int getDept() { return 3; }

	virtual void markThreshold( unsigned char * proc, unsigned char * thresh );
	virtual void fill(dtouch_Region *reg, int ** labelsMap, unsigned char r, unsigned char g, unsigned char b ) ;
	virtual void fill(dtouch_Region *reg, int ** labelsMap );
	virtual void fillRect( int x, int y, int w, int h );

	virtual void mark( const int& x, const int& y );
	virtual void mark( const DTPoint& p);
};

class GraphicImageGrayscale : public GraphicImage{
protected:
	GraphicImageGrayscale() {}
public:
	GraphicImageGrayscale(int in_width, int in_height ) : GraphicImage( in_width, in_height ) { }
	virtual ~GraphicImageGrayscale() {}

	virtual int getDept() { return 1; }

	virtual void markThreshold( unsigned char * proc, unsigned char * thresh );
	virtual void fill(dtouch_Region *reg, int ** labelsMap, unsigned char r, unsigned char g, unsigned char b ) {fill(reg,labelsMap);}
	virtual void fill(dtouch_Region *reg, int ** labelsMap );
	virtual void fillRect( int x, int y, int w, int h );

	virtual void mark( const int& x, const int& y );
	virtual void mark( const DTPoint& p);
};

#endif
