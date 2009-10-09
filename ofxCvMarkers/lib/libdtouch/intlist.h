/***************************************************************************
          intlist.h  -
                             -------------------
    begin                : Thu Mar 1 2007
    copyright            : (C) 2007 by Enrico Costanza @ EPFL
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


#ifndef EC_INTLIST
#define EC_INTLIST

#include "listpool.h"
#include "point.h"
#include  "fiducialdata.h"

#include <assert.h>

//typedef ListPool<DTPoint> dtouch_Region;

/*
typedef struct{
	int type;
	DTPoint centre;
	int angle;
	double angled;
	int rootRegion;
	DTPoint v1;
	DTPoint v2;
	DTPoint branchCentre[6];

	int width;
	int height;

	DTPoint corner[4];
} FiducialData;
//*/

//#include <deque>

//define new types based on the List<T> template
typedef List<int> IntList;

int * IntList2ArrayL( IntList * l );
IntList * array2IntList( int * data );
void sortIntList( IntList * l );
void sortIntList( IntList * l, IntList * follower );

bool compareArrays( int * a, int * b );
bool arrayContains( int * a, int t );

bool compareIntList( IntList * a, IntList * b );
int intcmp(const void *x, const void *y);

#endif

