/***************************************************************************
	binarytree.cpp
	
	A template binary tree. The template implicitely rely on the existence
	of a function 
	int cmp( const T& a, const T& b )
	similar to strcmp.
	The tree is based on the C code published on
	http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/tree.html

                             -------------------
    begin                : Wed Nov 15 2007
    copyright            : (C) 2007 by Enrico Costanza @ EPFL LDM
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

#include "binarytree.h"

int cmp( const int &a, const int &b ){
	return a-b;
}

int cmp( const int * a, const int * b ){
	if( *a != *b ){ return *a - *b; }
	int len = *a;
	/* Move s1 and s2 to the first differing characters 
	   in each string, or the ends of the strings if they
	   are identical.  */
	// TODO: shall i start from 1?
	for(int i=0;(i < len && *a == *b);i++) {
		a++;
		b++;
	}
	
	return *a - *b;
}
