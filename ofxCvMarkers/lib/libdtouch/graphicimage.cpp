/***************************************************************************
                      GraphicImage.cpp  -  description
                             -------------------
    begin                : Tue Nov  2 2004
    copyright            : (C) 2004 by Enrico Costanza
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

#include "graphicimage.h"

//using namespace dtouch;

//#include <jpeglib.h>
//#include <unistd.h>
#ifdef _EC_DEBUG
using std::cout;
using std::endl;
using std::cerr;
#endif

void GraphicImageRGB::markThreshold(unsigned char * proc,unsigned char * thres){
	unsigned char * tPtr = thres;
	unsigned char * pPtr = proc;
	unsigned char * imgPtr = _displayBuffer;

	for(int i=_width*_height;i>0;i--){
		if( *pPtr++ > *tPtr++ ){
			*imgPtr++ = 255;
			*imgPtr++ = 255;
			*imgPtr++ = 255;
		}else{
			*imgPtr++ = 0;
			*imgPtr++ = 0;
			*imgPtr++ = 0;
		}
	}

	return;
}

/***
 * This method fills reg
 *
 */
void GraphicImageRGB::fill(dtouch_Region *reg, int ** labelsMap, unsigned char r, unsigned char g, unsigned char b ){
	if( reg==NULL ){
		return;
	}

	// calculate the bounding box
	DTPoint min, max;
	reg->box(min, max, _width, _height );

	// get the label of this region by looking up the first pixel
	DTPoint f = reg->getFirstData();
	int label = *(labelsMap[f.x+f.y*_width]);

	/*
	using namespace std;
	cout << "label: " << label << endl;
	cout << "min: " << min << endl;
	cout << "max: " << max << endl;
	//*/

	unsigned int offset=0;
	unsigned int offsetMUL3=0;
	// scan the bounding box and look at the labels map
	// fill any point that has the label that we want
	for(int y=min.y;y<=max.y;y++){
		offset = min.x + y * _width;
		int ** lPtr = labelsMap;
		lPtr+= offset;
		offsetMUL3 = offset * 3;
		unsigned char * rPtr = _displayBuffer;
		rPtr+= offsetMUL3 + 0;
		unsigned char * gPtr = _displayBuffer;
		gPtr+= offsetMUL3 + 1;
		unsigned char * bPtr = _displayBuffer;
		bPtr+= offsetMUL3 + 2;

		for(int x=min.x;x<=max.x;x++){
			if( **lPtr == label ){
				// fill this pixel
				*rPtr = r;
				*gPtr = g;
				*bPtr = b;
			}
			lPtr++;
			rPtr+=3;
			gPtr+=3;
			bPtr+=3;
		}
	}

	return;
}

/***
 * This method fills reg
 *
 */
void GraphicImageRGB::fill(dtouch_Region *reg, int ** labelsMap){
	if( reg==NULL ){
		return;
	}

	// calculate the bounding box
	DTPoint min, max;
	reg->box(min, max, _width, _height );

	// get the label of this region by looking up the first pixel
	DTPoint f = reg->getFirstData();
	int label = *(labelsMap[f.x+f.y*_width]);

	unsigned int offset=0;
	unsigned int offsetMUL3=0;
	// scan the bounding box and look at the labels map
	// fill any point that has the label that we want
	for(int y=min.y;y<=max.y;y++){
		offset = min.x + y * _width;
		int ** lPtr = labelsMap + offset;
		offsetMUL3 = offset * 3;
		unsigned char * rPtr = _displayBuffer + offsetMUL3 + 0;
		unsigned char * gPtr = _displayBuffer + offsetMUL3 + 1;
		unsigned char * bPtr = _displayBuffer + offsetMUL3 + 2;

		for(int x=min.x;x<=max.x;x++){
			unsigned char v = (((y%2==0)&&(x%2==0))||((y%2==1)&&(x%2==1)))?(0):(255);
			if( **lPtr == label ){
				// fill this pixel
				*rPtr = v;
				*gPtr = v;
				*bPtr = v;
			}
			lPtr++;
			rPtr+=3;
			gPtr+=3;
			bPtr+=3;
		}
	}

	return;
}

void GraphicImageRGB::fillRect( int x, int y, int w, int h ){

	unsigned int offset=0;
	unsigned int offsetMUL3=0;
	// scan the bounding box and look at the labels map
	// fill any point that has the label that we want
	for(int y0=y;y0<=y+h;y0++){
		offset = x + y0 * _width;
		offsetMUL3 = offset * 3;
		unsigned char * rPtr = _displayBuffer + offsetMUL3 + 0;
		unsigned char * gPtr = _displayBuffer + offsetMUL3 + 1;
		unsigned char * bPtr = _displayBuffer + offsetMUL3 + 2;

		for(int x0=x;x0<=x+w;x0++){
			unsigned char v = (((y0%2==0)&&(x0%2==0))||((y0%2==1)&&(x0%2==1)))?(0):(255);
			// fill this pixel
			*rPtr = v;
			*gPtr = v;
			*bPtr = v;
			rPtr+=3;
			gPtr+=3;
			bPtr+=3;
		}
	}

	return;
}

void GraphicImageGrayscale::markThreshold(unsigned char * proc, unsigned char * thres){
	unsigned char * tPtr = thres;
	unsigned char * pPtr = proc;
	unsigned char * imgPtr = _displayBuffer;
	// fill any point that has the label that we want
	for(int i=_width*_height;i>0;i--){
		//if( *pPtr++ > *tPtr++ ){ *imgPtr++=255; }
		//else{ *imgPtr++=0; }
		( *pPtr++ > *tPtr++ )?( *imgPtr++=255 ):( *imgPtr++=0 );
	}

	return;
}


/***
 * This method fills reg
 *
 */
void GraphicImageGrayscale::fill(dtouch_Region *reg, int ** labelsMap){
	if( reg==NULL ){
		return;
	}

	// calculate the bounding box
	DTPoint min, max;
	reg->box(min, max, _width, _height );

	// get the label of this region by looking up the first pixel
	DTPoint f = reg->getFirstData();
	int label = *(labelsMap[f.x+f.y*_width]);

	unsigned int offset=0;
	// scan the bounding box and look at the labels map
	// fill any point that has the label that we want
	for(int y=min.y;y<=max.y;y++){
		offset = min.x + y * _width;
		int ** lPtr = labelsMap + offset;
		unsigned char * imgPtr = _displayBuffer + offset;

		for(int x=min.x;x<=max.x;x++){
			unsigned char v = (((y%2)&&(x%2))||((y%2==0)&&(x%2==0)))?(0):(255);
			if( **lPtr == label ){
				// fill this pixel
				*imgPtr = v;
			}
			lPtr++;
			imgPtr++;
		}
	}

	return;
}

void GraphicImageGrayscale::fillRect( int x, int y, int w, int h ){
	unsigned int offset=0;
	// scan the bounding box and look at the labels map
	// fill any point that has the label that we want
	for(int y0=y;y0<=y+h;y0++){
		offset = x + y0 * _width;
		unsigned char * imgPtr = _displayBuffer + offset;

		for(int x0=x;x0<=x+w;x0++){
			unsigned char v = (((y0%2==0)&&(x0%2==0))||((y0%2==1)&&(x0%2==1)))?(0):(255);
			*imgPtr++ = v;
		}
	}

	return;
}


void GraphicImage::markCross( const DTPoint& p ){
	mark(p);
	if( p.x-1 >= 0 ){ mark(DTPoint(p.x-1,p.y)); }
	if( p.x+1 < _width ){ mark(DTPoint(p.x+1,p.y)); }
	if( p.y-1 >= 0 ){ mark(DTPoint(p.x,p.y-1)); }
	if( p.y-1 < _height ){ mark(DTPoint(p.x,p.y+1)); }
}




GraphicImage::GraphicImage(int in_width, int in_height )
	: _width(in_width), _height(in_height) {

  //cout << "GraphicImage constructor called" << endl;

  //_displayBuffer = new unsigned char[3*_width*_height];

  //cout << "GraphicImage constructor about to return" << endl;
}

GraphicImage::~GraphicImage() {
  //delete [] _displayBuffer;
}


void GraphicImageGrayscale::mark( const DTPoint& p ){
	int x = p.x;
	int y = p.y;
	if( x < 0 || x>=_width ){ return; }
	if( y < 0 || y>=_height ){ return; }
	_displayBuffer[x+0+y*_width] = 0;
}

void GraphicImageGrayscale::mark( const int& x, const int& y ){
	int X = x*3;
	if( X < 0 || X>=_width ){ return; }
	if( y < 0 || y>=_height ){ return; }
	_displayBuffer[X+0+y*_width*3] = 0;
	_displayBuffer[X+1+y*_width*3] = 0;
	_displayBuffer[X+2+y*_width*3] = 0;
}

void GraphicImageRGB::mark( const DTPoint& p ){
	int x = 3 * p.x;
	int y = p.y;
	if( x < 0 || x>=_width ){ return; }
	if( y < 0 || y>=_height ){ return; }
	_displayBuffer[x+0+y*_width*3] = 0;
	_displayBuffer[x+1+y*_width*3] = 0;
	_displayBuffer[x+2+y*_width*3] = 0;
}

void GraphicImageRGB::mark( const int& x, const int& y ){
	int X = x*3;
	if( X < 0 || X>=_width ){ return; }
	if( y < 0 || y>=_height ){ return; }
	_displayBuffer[X+0+y*_width*3] = 0;
	_displayBuffer[X+1+y*_width*3] = 0;
	_displayBuffer[X+2+y*_width*3] = 0;
}


#if 0

// utility functions and constants
DTPoint minX( PointList *reg ){
  DTPoint result(0,0);
  int xMin = 800;
	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint p = reg->getData();
    if( p.x < xMin ){
      result = p;
      xMin = p.x;
    }
  }
  return result;
}

DTPoint maxX( PointList *reg ){
  DTPoint result(0,0);
  int xMax = 0;
	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint p = reg->getData();
    if( p.x > xMax ){
      result = p;
      xMax = p.x;
    }
  }
  return result;
}

DTPoint minY( PointList *reg ){
  DTPoint result(0,0);
  int yMin = 600;
	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint p = reg->getData();
    if( p.y < yMin ){
      result = p;
      yMin = p.y;
    }
  }
  return result;
}

DTPoint maxY( PointList *reg ){
  DTPoint result(0,0);
  int yMax = 0;
	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint p = reg->getData();
    if( p.y > yMax ){
      result = p;
      yMax = p.y;
    }
  }
  return result;
}

const char numMap[10][15] = {
  {0,0,0,
   0,1,0,
   0,1,0,
   0,1,0,
   0,0,0,},
  {1,0,1,
   0,0,1,
   1,0,1,
   1,0,1,
   1,0,1,},
  {0,0,0,
   1,1,0,
   0,0,0,
   0,1,1,
   0,0,0,},
  {0,0,0,
   1,1,0,
   0,0,0,
   1,1,0,
   0,0,0,},
  {0,1,0,
   0,1,0,
   0,0,0,
   1,1,0,
   1,1,0,},
  {0,0,0,
   0,1,1,
   0,0,0,
   1,1,0,
   0,0,0,},
  {0,0,0,
   0,1,1,
   0,0,0,
   0,1,0,
   0,0,0,},
  {0,0,0,
   1,1,0,
   1,1,0,
   1,1,0,
   1,1,0,},
  {0,0,0,
   0,1,0,
   0,0,0,
   0,1,0,
   0,0,0,},
  {0,0,0,
   0,1,0,
   0,0,0,
   1,1,0,
   1,1,0,}
};

//********************************************************
//********************************************************
//**                                                    **
//**             Display related methods                **
//**                                                    **
//********************************************************
//********************************************************

void GraphicImage::outline(dtouch_Region *reg, const unsigned char r, const unsigned char g, const unsigned char b ){
	if( reg==NULL ){
		return;
	}

	DTPoint min, max;
	reg->box( min, max, _width, _height );
	// cout << "min: " << min << "\tmax: " << max << endl;
	for(int x=min.x;x<max.x;x++){
		int y=min.y;
		int X = 3*x;
		_displayBuffer[X+0+y*_width*3] = r;
		_displayBuffer[X+1+y*_width*3] = g;
		_displayBuffer[X+2+y*_width*3] = b;
		y=max.y;
		_displayBuffer[X+0+y*_width*3] = r;
		_displayBuffer[X+1+y*_width*3] = g;
		_displayBuffer[X+2+y*_width*3] = b;
	}
	for(int y=min.y;y<max.y;y++){
		int x=min.x * 3;
		_displayBuffer[x+0+y*_width*3] = r;
		_displayBuffer[x+1+y*_width*3] = g;
		_displayBuffer[x+2+y*_width*3] = b;
		x=max.x * 3;
		_displayBuffer[x+0+y*_width*3] = r;
		_displayBuffer[x+1+y*_width*3] = g;
		_displayBuffer[x+2+y*_width*3] = b;
	}
}

void GraphicImage::outlineStrong(dtouch_Region *reg, const unsigned char r, const unsigned char g, const unsigned char b ){
	if( reg==NULL ){
		return;
	}

	DTPoint min, max;
	reg->box(min, max, _width, _height );
	// cout << "min: " << min << "\tmax: " << max << endl;
	for(int x=min.x;x<max.x;x++){
		int y=min.y;
		int X = 3*x;
		_displayBuffer[X+0+y*_width*3] = r;
		_displayBuffer[X+1+y*_width*3] = g;
		_displayBuffer[X+2+y*_width*3] = b;
		y=max.y;
		_displayBuffer[X+0+y*_width*3] = r;
		_displayBuffer[X+1+y*_width*3] = g;
		_displayBuffer[X+2+y*_width*3] = b;
		// double lines
		y=min.y+1;
		_displayBuffer[X+0+y*_width*3] = r;
		_displayBuffer[X+1+y*_width*3] = g;
		_displayBuffer[X+2+y*_width*3] = b;
		y=max.y-1;
		_displayBuffer[X+0+y*_width*3] = r;
		_displayBuffer[X+1+y*_width*3] = g;
		_displayBuffer[X+2+y*_width*3] = b;
	}
	for(int y=min.y;y<max.y;y++){
		int x=min.x * 3;
		_displayBuffer[x+0+y*_width*3] = r;
		_displayBuffer[x+1+y*_width*3] = g;
		_displayBuffer[x+2+y*_width*3] = b;
		x=max.x * 3;
		_displayBuffer[x+0+y*_width*3] = r;
		_displayBuffer[x+1+y*_width*3] = g;
		_displayBuffer[x+2+y*_width*3] = b;
		// double lines
		x= (min.x+1) * 3;
		_displayBuffer[x+0+y*_width*3] = r;
		_displayBuffer[x+1+y*_width*3] = g;
		_displayBuffer[x+2+y*_width*3] = b;
		x= (max.x-1) * 3;
		_displayBuffer[x+0+y*_width*3] = r;
		_displayBuffer[x+1+y*_width*3] = g;
		_displayBuffer[x+2+y*_width*3] = b;
	}
}


void GraphicImage::boxPlus(dtouch_Region *reg, DTPoint &min, DTPoint &max,
		DTPoint &p1, DTPoint &p2 , DTPoint &p3 , DTPoint &p4  ){
	int xMin = _width;
	int xMax = 0;
	int yMin = _height;
	int yMax = 0;

	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint tmp = reg->getData();
		if(tmp.x > xMax){
			xMax=tmp.x;
			p1 = tmp;
		}
		if(tmp.y > yMax){
			yMax=tmp.y;
			p2 = tmp;
		}
		if(tmp.x < xMin){
			xMin=tmp.x;
			p3 = tmp;
		}
		if(tmp.y < yMin){
			yMin=tmp.y;
			p4 = tmp;
		}
	}

	min.x = xMin;
	min.y = yMin;
	max.x = xMax;
	max.y = yMax;
	return;
}

/*
 void GraphicImage::plotNumber(DTPoint p, int number, unsigned char r, unsigned char g, unsigned char b ){
	int nDigits;
	bool negative = ( number < 0 );
	number = abs( number );
	if( number==0 ){
		nDigits = 1;
	}else{
		//nDigits = (int) round( log10((double)number) ) + 1;
		nDigits = (int) ceil( log10((double)number) ) + 1;
	}
	#ifdef _WIN32
	plotDigit( p+DTPoint(0,0), number%10, r, g, b);
	for(int i=1;i<nDigits;i++){
	#else
	plotDigit( p+DTPoint((nDigits-1)*4,0), number%10, r, g, b);
	for(int i=nDigits-2;i>=0;i--){
	#endif
		number /= 10;
		int digit = number%10;
		plotDigit( p+DTPoint(i*4,0), digit, r, g, b);
	}
	if( negative ){
		if( (p.x > 6) && (p.y >=0) && (p.x < _width) && (p.y<(_height-3)) ){
			#ifdef _WIN32
			mark( p+DTPoint( nDigits*4+1, 3 ), r, g, b );
			mark( p+DTPoint( nDigits*4+2, 3 ), r, g, b );
			mark( p+DTPoint( nDigits*4+3, 3 ), r, g, b );
			#else
			mark( p+DTPoint( -4, 3 ), r, g, b );
			mark( p+DTPoint( -5, 3 ), r, g, b );
			mark( p+DTPoint( -6, 3 ), r, g, b );
			#endif
		}
	}

	return;
}
*/

void GraphicImage::plotDigit(DTPoint p, int digit, unsigned char r, unsigned char g, unsigned char b ){
	if( digit > 9 || digit < 0 ){
		return;
	}

	int x = p.x;
	int y = p.y;
	for(int dy=0;dy<5;dy++){
		for(int dx=0;dx<3;dx++){
		//cout << numMap[digit][dx+dy*3];
			#ifdef _WIN32
			int xImg = x+3-dx;
			int yImg = y+5-dy;
			#else
			int xImg = x+dx;
			int yImg = y+dy;
			#endif
			if( (xImg < _width) && (yImg < _height) && (xImg >= 0) && (yImg >=0) ){
				if( numMap[digit][dx+dy*3]==0 ){
					mark(DTPoint(xImg,yImg),r,g,b);
				}else{
					mark(DTPoint(xImg,yImg),255-r,255-g,255-b);
				}
			}
		}
		//cout << endl;
	}
}


/***
 * This method fills reg
 *
 */
void GraphicImage::fill(dtouch_Region *reg, int ** labelsMap, unsigned char r, unsigned char g, unsigned char b ){
	if( reg==NULL ){
		return;
	}

	// calculate the bounding box
	DTPoint min, max;
	reg->box(min, max, _width, _height );

	// get the label of this region by looking up the first pixel
	DTPoint f = reg->getFirstData();
	int label = *(labelsMap[f.x+f.y*_width]);

	unsigned int offset=0;
	unsigned int offsetMUL3=0;
	// scan the bounding box and look at the labels map
	// fill any point that has the label that we want
	for(int y=min.y;y<=max.y;y++){
		offset = min.x + y * _width;
		int ** lPtr = labelsMap + offset;
		offsetMUL3 = offset * 3;
		unsigned char * rPtr = _displayBuffer + offsetMUL3 + 0;
		unsigned char * gPtr = _displayBuffer + offsetMUL3 + 1;
		unsigned char * bPtr = _displayBuffer + offsetMUL3 + 2;

		for(int x=min.x;x<=max.x;x++){
			if( **lPtr == label ){
				// fill this pixel
				*rPtr = r;
				*gPtr = g;
				*bPtr = b;
			}
			lPtr++;
			rPtr+=3;
			gPtr+=3;
			bPtr+=3;
		}
	}

	return;
}

void GraphicImage::mark(PointList *reg, unsigned char r, unsigned char g, unsigned char b ){
	if( reg==NULL ){
		return;
	}
	for(reg->reset();!reg->nullTest();reg->fwd()){
		DTPoint p = reg->getData();
		int x = p.x;
		int y = p.y;
		_displayBuffer[x+0+y*_width] = 0;
	}
}

void GraphicImage::plotSegment(DTPoint p1, DTPoint p2, unsigned char r, unsigned char g, unsigned char b ){
	int deltaX = p2.x - p1.x;
	int deltaY = p2.y - p1.y;
	if( abs(deltaX) > abs(deltaY) ){
		// plot as y = f(x)
		int dx = (( deltaX<0 )?(-1):(1));
		for(int x=p1.x;(abs(x - p2.x)>1);x+=dx){
			int y = p1.y + ( deltaY * (x-p1.x) / deltaX );
			if( x>=0 && x<_width && y>=0 && y<_height ){
				int ximg = 3*x;
    			_displayBuffer[ximg+0+y*_width*3] = r;
    			_displayBuffer[ximg+1+y*_width*3] = g;
				_displayBuffer[ximg+2+y*_width*3] = b;
			}
		}
	}else{
		// plot as x = f(y)
		int dy = (( deltaY<0 )?(-1):(1));
		for(int y=p1.y;(abs(y - p2.y)>1);y+=dy){
			int x = p1.x + ( deltaX * (y-p1.y) / deltaY );
			if( x>=0 && x<_width && y>=0 && y<_height ){
				int ximg = 3*x;
    			_displayBuffer[ximg+0+y*_width*3] = r;
    			_displayBuffer[ximg+1+y*_width*3] = g;
				_displayBuffer[ximg+2+y*_width*3] = b;
				}
		}
	}
}

void GraphicImage::plotLine( double mDouble, double qDouble ){
  if( mDouble < 1.0 ){
  	#ifdef _EC_DEBUG
    cout << "y = f(x) - m: " << mDouble << " q: " << qDouble << endl;
    #endif
    //plot as y = f(x) = mx + q
    int m = (int) ( 100.0 * mDouble );
    int q = (int) ( 100.0 * qDouble );
    //cout << "y = f(x) - m: " << m << ", q: " << q << endl;
    for(int x=0;x<_width;x++){
      int y = (m*x + q) /100;
      if( (y>=0) && (y<_height) ){
        mark( x, y, 0, 0, 0 );
      }
    }
  }else{
  	#ifdef _EC_DEBUG
    cout << "x = f(y) - m: " << mDouble << " q: " << qDouble << endl;
    #endif
    //plot as x = f(y) = my + q
    int m = (int) ( 100.0 / mDouble );
    int q = (int) ( 100.0 * qDouble / mDouble);
    //cout << "x = f(y) - m: " << m << ", q: " << q << endl;
    for(int y=0;y<_height;y++){
      int x = (m*y + q) / 100;
      if( (x>=0) && (x<_width) ){
        mark( x, y, 0, 0, 0 );
      }
    }
  }
  return;
}


void GraphicImage::plotLineNorm( int rho, int angle ){
  //cout << "angle: " << angle << ", rho: " << rho << endl;
  if( abs(angle) > 45 ){
    //plot as y = f(x) = mx + q
    int m = (int) (- 100.0 / tan( angle*acos(0.0)/90.0 ));
    int q = (int) (rho * 100.0 / sin( angle*acos(0.0)/90.0 ));
    //cout << "y = f(x) - m: " << m << ", q: " << q << endl;
    for(int x=0;x<_width;x++){
      int y = (m*x + q) /100;
      if( (y>=0) && (y<_height) ){
        mark( x, y, 0, 0, 0 );
      }
    }
  }else{
    //plot as x = f(y) = my + q
    int m = (int) (- tan( angle*acos(0.0)/90.0 ) * 100.0);
    int q = (int) (rho * 100.0 / cos( angle*acos(0.0)/90.0 ));
    //cout << "x = f(y) - m: " << m << ", q: " << q << endl;
    for(int y=0;y<_height;y++){
      int x = (m*y + q) / 100;
      if( (x>=0) && (x<_width) ){
        mark( x, y, 0, 0, 0 );
      }
    }
  }

}

// try to hack a method to map a (rectangular) photo
// to a generic quadrilateral
void GraphicImage::mapTextureToQuadrilateral( unsigned char *texture, int textureWidth, int textureHeight,
    unsigned char *img, DTPoint p1, DTPoint p2, DTPoint p3, DTPoint p4 ){

  unsigned char *ip = texture;
	for(int v=0;v<textureHeight;v++) {
		//unsigned char *rp = result + Y*photoW;
		for(int u=0;u<textureWidth;u++){
			// in the following, fixed point arithmetic
			// should be used rather that floating point

			double U = (double)u/textureWidth;
			double V = (double)v/textureHeight;
			double xtop = (double) p1.x + U*(p2.x-p1.x);
			double ytop = (double) p1.y + U*(p2.y-p1.y);
			double xbottom = (double) p4.x + U*(p3.x-p4.x);
			double ybottom = (double) p4.y + U*(p3.y-p4.y);
			double x = xtop + V*(xbottom-xtop);
			double y = ytop + V*(ybottom-ytop);

      // set pixel (x,y) in the image to texture(u,v)
      // ..except that pixel (x,y) does not exist because x,y are double
      // so I would need to interpolate int(x) and int(y)
      // just "for the sake of it" map without interpolation
      unsigned char r = *ip++;
      unsigned char g = *ip++;
      unsigned char b = *ip++;
      mark( DTPoint((int)x,(int)y), r, g, b);
			//*rp++ = interpolate(img, x, y);
		}
	}

	return;
}

#endif

/*
void GraphicImage::saveScreen(){
  //use libjpg or libpng to do the same thing
}
//*/


