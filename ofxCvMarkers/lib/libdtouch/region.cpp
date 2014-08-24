/***************************************************************************
             region.cpp  -  class representing image regions
                             -------------------
    begin                : Wed Dec 01 2004
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

#include "region.h"

using namespace std;

//using namespace dtouch;
#define min(a,b) ((a<b)?(a):(b))
#define max(a,b) ((a>b)?(a):(b))


void dtouch_Region::box( DTPoint &min, DTPoint &max, int width, int height){
	if( _min.x!=-1 ){
		min = _min;
		max = _max;
		return;
	}
	dtouch_Region * reg = this;
	_min.x = width;
	_max.x = 0;
	_min.y = height;
	_max.y = 0;

	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint tmp = reg->getData();
		(tmp.x > _max.x) ? (_max.x=tmp.x) : (0);
		(tmp.y > _max.y) ? (_max.y=tmp.y) : (0);
		(tmp.x < _min.x) ? (_min.x=tmp.x) : (0);
		(tmp.y < _min.y) ? (_min.y=tmp.y) : (0);
	}

	min = _min;
	max = _max;

	return;
}

DTPoint dtouch_Region::getCentre( ){
	DTPoint result(0,0);
	for(this->reset();!(this->isNull());this->fwd()){
		result += this->getData();
	}

	result /= this->getSize();
	return result;
}

DTPoint dtouch_Region::getCentreRound( ){
	DTPoint result(0,0);
	for(this->reset();!(this->isNull());this->fwd()){
		result += this->getData();
	}

	double x = (double) result.x;
	double y = (double) result.y;
	//result.x = (int) round( x / (double) reg->getSize() );
	result.x = (int) ceil( x / (double) this->getSize() );
	//result.y = (int) round( y / (double) reg->getSize() );
	result.y = (int) ceil( y / (double) this->getSize() );
	return result;
}

#ifndef __SYMBIAN32__
PointList * intersect(dtouch_Region * a, dtouch_Region * b, int ** labelsMapA, int ** labelsMapB, int width, int height){
	PointList * result = PointList::newL();

	DTPoint min_, max_, minA, maxA, minB, maxB;
	a->box(minA, maxA, width, height);
	b->box(minB, maxB, width, height);
	min_ = DTPoint( min(minA.x,minB.x), min(minA.y,minB.y) );
	max_ = DTPoint( max(maxA.x,maxB.x), max(maxA.y,maxB.y) );

	// get the label of region a by looking up the first pixel
	DTPoint f;
	int labelA = -1;
	if( !(a->isEmpty()) ){
		f = a->getFirstData();
		labelA = *(labelsMapA[f.x+f.y*width]);
	}

	int labelB = -1;
	// get the label of region b by looking up the first pixel
	if( !(b->isEmpty()) ){
		f = b->getFirstData();
		labelB = *(labelsMapB[f.x+f.y*width]);
	}

	unsigned int offset=0;
	// scan the bounding box and look at the labels map
	for(int y=min_.y;y<=max_.y;y++){
		offset = min_.x + y * width;
		int ** lPtrA = labelsMapA + offset;
		int ** lPtrB = labelsMapB + offset;

		for(int x=min_.x;x<=max_.x;x++){
			//cout << "lPtrA: " << (**lPtrA) << ", lPtrB: " << (**lPtrB);
			//cout << " labelA: " << labelA << ", labelB: " << labelB << endl;
			if( (**lPtrB == labelB) && (**lPtrA == labelA) ){
				// this pixel is part of the region
				// add it to results
				result->appendL( DTPoint(x,y) );
			}
			lPtrA++;
			lPtrB++;
		}
	}

	return result;
}

PointList * exclude(dtouch_Region * a, dtouch_Region * b, int ** labelsMapA, int ** labelsMapB, int width, int height){
	PointList * result = PointList::newL();

	DTPoint min_, max_, minA, maxA, minB, maxB;
	a->box(minA, maxA, width, height);
	a->box(minB, maxB, width, height);
	min_ = DTPoint( max(minA.x,minB.x), max(minA.y,minB.y) );
	max_ = DTPoint( min(maxA.x,maxB.x), min(maxA.y,maxB.y) );

	// get the label of region a by looking up the first pixel
	DTPoint f;
	int labelA = -1;
	if( !(a->isEmpty()) ){
		f = a->getFirstData();
		labelA = *(labelsMapA[f.x+f.y*width]);
	}

	// get the label of region b by looking up the first pixel
	int labelB = -1;
	if( !(b->isEmpty()) ){
		f = b->getFirstData();
		labelB = *(labelsMapA[f.x+f.y*width]);
	}

	unsigned int offset=0;
	// scan the bounding box and look at the labels map
	for(int y=min_.y;y<=max_.y;y++){
		offset = min_.x + y * width;
		int ** lPtrA = labelsMapA + offset;
		int ** lPtrB = labelsMapB + offset;

		for(int x=min_.x;x<=max_.x;x++){
			//cout << "lPtrA: " << (**lPtrA) << ", lPtrB: " << (**lPtrB);
			//cout << " labelA: " << labelA << ", labelB: " << labelB << endl;
			if( /*(**lPtrA != **lPtrB) &&*/ ((labelA == **lPtrA)||(labelB == **lPtrB)) ){
				// this pixel is part of the region
				// add it to results
				result->appendL( DTPoint(x,y) );
			}
			lPtrA++;
			lPtrB++;
		}
	}

	return result;
}

PointList * dtouch_Region::getPointList(int ** labelsMap, int width, int height){
	PointList * result = PointList::newL();

	// TODO: check region size!
	// convert the runlenght representation to a list of points
	//cout << "dtouch_Region::getPointList" << endl;
	//cout << "region size: " << getSize() << endl;
	//cout << "first point: " << getFirstData() << endl;
	dtouch_Region * reg = this;
	// calculate the bounding box
	DTPoint min, max;
	reg->box(min, max, width, height);

	// get the label of this region by looking up the first pixel
	DTPoint f = reg->getFirstData();
	//cout << "region size: " << reg->getSize() << endl;
	//cout << "first point: " << f << endl;
	int label = *(labelsMap[f.x+f.y*width]);

	unsigned int offset=0;
	// scan the bounding box and look at the labels map
	for(int y=min.y;y<=max.y;y++){
		offset = min.x + y * width;
		int ** lPtr = labelsMap + offset;

		for(int x=min.x;x<=max.x;x++){
			if( **lPtr == label ){
				// this pixel is part of the region
				// add it to results
				result->appendL( DTPoint(x,y) );
			}
			lPtr++;
		}
	}

	return result;
}

#endif //def __SYMBIAN32__
