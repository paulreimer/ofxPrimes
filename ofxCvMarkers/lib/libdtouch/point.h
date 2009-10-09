/***************************************************************************
                          point.h  -  simple point convenience class
                             -------------------
    begin                : Thu Oct 17 2002
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
#ifndef EC_POINT
#define EC_POINT

//#include <iostream>
#ifndef __SYMBIAN32__
#include <cstdlib>
#include <iostream>
#else
#include <stdlib.h>
#endif

#include <math.h>

class DTPoint{
public:
	long x;
	long y;

	DTPoint() { /*x=0; y=0;*/ }
	DTPoint(int a, int b) : x(a), y(b) { }

	void operator=(const DTPoint& a) { x=a.x; y=a.y; return; }
	void operator+=(const DTPoint& a) { x+=a.x; y+=a.y; return; }
	void operator-=(const DTPoint& a) { x-=a.x; y-=a.y; return; }
	void operator|=(const DTPoint& a) { x=abs(a.x); y=abs(a.y); return; }

	void operator+=(const int& a) { x+=a; y+=a; return; }
	void operator-=(const int& a) { x-=a; y-=a; return; }
	void operator*=(const int& a) { x*=a; y*=a; return; }
	void operator/=(const int& a) { x/=a; y/=a; return; }

	DTPoint operator[](const DTPoint& a){
		DTPoint r;
		r.x=abs(a.x);
		r.y=abs(a.y);
		return r;
	}

	// Manhattan distance or city block distance
	int distanceTo( const DTPoint &other )
	{
		return abs(x - other.x) + abs(y - other.y);
	}


	int dist( const DTPoint &a ){
		//return (int)round(sqrt( (x-a.x)*(x-a.x) + (y-a.y)*(y-a.y) ));
		return (int)ceil(sqrt( (double) ((x-a.x)*(x-a.x) + (y-a.y)*(y-a.y)) ));
	}

	bool is4Neighbour( DTPoint p ){
		return ( abs( p.x - x ) + abs( p.y - y ) == 1 );
	}

	bool is8Neighbour( DTPoint p ){
		return ( (abs( p.x - x ) <= 1) && (abs( p.y - y ) <= 1) );
	}

};

inline DTPoint operator+(const DTPoint& a, const DTPoint& b){
  int x=a.x + b.x;
  int y=a.y + b.y;
  return DTPoint(x,y);
}

inline DTPoint operator-(const DTPoint& a, const DTPoint& b){
  int x=a.x - b.x;
  int y=a.y - b.y;
  return DTPoint(x,y);
}

inline bool operator==(const DTPoint& a, const DTPoint& b){
	return (a.x==b.x && a.y==b.y);
}

#ifndef __SYMBIAN32__
inline std::ostream &operator<<(std::ostream &strm, const DTPoint &object)
{
  strm << "(" << object.x << "," << object.y << ")";
  return strm ;
}
#endif

/*
class dPoint{
public:
	double x;
	double y;

	dPoint() {}
	dPoint(double a, double b) : x(a), y(b) { }

  void operator=(const dPoint& a) { x=a.x; y=a.y; return; }
  void operator+=(const dPoint& a) { x+=a.x; y+=a.y; return; }
  void operator-=(const dPoint& a) { x-=a.x; y-=a.y; return; }
  void operator|=(const dPoint& a) { x=fabs(a.x); y=fabs(a.y); return; }
  double operator*(const dPoint& a) { return a.x*x + a.y*y; }

  void operator+=(const double& a) { x+=a; y+=a; return; }
  void operator-=(const double& a) { x-=a; y-=a; return; }
  void operator*=(const double& a) { x*=a; y*=a; return; }
  void operator/=(const double& a) { x/=a; y/=a; return; }

  dPoint operator[](const dPoint& a){
    dPoint r;
    r.x=fabs(a.x);
    r.y=fabs(a.y);
    return r;
  }
};

dPoint operator+(const dPoint& a, const dPoint& b){
  double x=a.x + b.x;
  double y=a.y + b.y;
  return dPoint(x,y);
}

dPoint operator-(const dPoint& a, const dPoint& b){
  double x=a.x - b.x;
  double y=a.y - b.y;
  return dPoint(x,y);
}

dPoint operator*(const double& a, const dPoint& b){
  return dPoint(a*b.x,a*b.y);
}


bool operator==(const dPoint& a, const dPoint& b){
	return (a.x==b.x && a.y==b.y);
}


#ifndef __SYMBIAN32__
std::ostream &operator<<(std::ostream &strm, const dPoint &object){
  strm << "(" << object.x << "," << object.y << ")";
  return strm;
}
#endif

*/



#endif

