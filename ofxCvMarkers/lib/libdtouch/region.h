/***************************************************************************
               region.h  -  class representing image regions
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

#ifndef EC_REGION
#define EC_REGION

#include "listpool.h"
#include "types.h"

class dtouch_Region : private ListPool<DTPoint>
{
public:
	bool _white;
	
	bool _potentialRoot;
	bool _branch;
	
	bool _onBorder;

	int _noLeaves;
	int _noBranches;
	
protected:
	DTPoint _min;
	DTPoint _max;

	bool _labelPtrPoolAllocated;
	Pool<int *> *_labelPtrPool;
	bool _labelPtrListAllocated;
	ListPool<int *> *_labelPtrList;

public:
	friend class listItem;
public:
	bool isOnBorder(){
		return _onBorder;
	}

protected:
	dtouch_Region( Pool<DTPoint> *in_pointPool ) : 
		ListPool<DTPoint>(in_pointPool),
		_white(false),
		_potentialRoot(false),
		_branch(false),
		_onBorder(false), 
		_noLeaves(0),
		_noBranches(0),
		_min(-1,-1),
		_max(-1,-1),
		_labelPtrPoolAllocated(true),
		_labelPtrListAllocated(true)
	{ }

	// this is the one used from the RegionAdjacencyGraph
	dtouch_Region( Pool<DTPoint> *in_pointPool, Pool<int *> *in_labelPtrPool) : 
		ListPool<DTPoint>(in_pointPool),
		_white(false),
		_potentialRoot(false),
		_branch(false),
		_onBorder(false), 
		_noLeaves(0),
		_noBranches(0),
		_min(-1,-1),
		_max(-1,-1),
		_labelPtrPoolAllocated(false),
		_labelPtrPool(in_labelPtrPool),
		_labelPtrListAllocated(true)
	{ }

	dtouch_Region( Pool<DTPoint> *in_pointPool, DynamicPool<int *> *in_labelPtrPool, 
		ListPool<int *> *in_labelPtrList) : 
		ListPool<DTPoint>(in_pointPool),
		_white(false),
		_potentialRoot(false),
		_branch(false),
		_onBorder(false), 
		_noLeaves(0),
		_noBranches(0),
		_min(-1,-1),
		_max(-1,-1),
		_labelPtrPoolAllocated(false),
		_labelPtrPool(in_labelPtrPool),
		_labelPtrListAllocated(false),
		_labelPtrList(in_labelPtrList)
	{ }

	dtouch_Region( Pool<DTPoint> *in_pointPool, ListPool<int *> *in_labelPtrList) : 
		ListPool<DTPoint>(in_pointPool),
		_white(false),
		_potentialRoot(false),
		_branch(false),
		_onBorder(false), 
		_noLeaves(0),
		_noBranches(0),
		_min(-1,-1),
		_max(-1,-1),
		_labelPtrPoolAllocated(false),
		_labelPtrPool(NULL),
		_labelPtrListAllocated(false),
		_labelPtrList(in_labelPtrList)
	{ }

	dtouch_Region():
		_min(-1,-1),
		_max(-1,-1)
	{}

	void constructL(){
		if( _labelPtrPoolAllocated ){
			_labelPtrPool = DynamicPool<int *>::newL(300);
		}
		if( _labelPtrListAllocated ){
			_labelPtrList = new ListPool<int *>( _labelPtrPool );
		}
	}
	

public:
	
	static dtouch_Region * newL( Pool<DTPoint> *in_pointPool ){
		#ifndef __SYMBIAN32__
		dtouch_Region * self = new dtouch_Region(in_pointPool);
		self->constructL();
		#else
		dtouch_Region * self = new (ELeave) dtouch_Region(in_pointPool);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}
	
	static dtouch_Region * newL( Pool<DTPoint> *in_pointPool, ListPool<int *> *in_labelPtrList){
		#ifndef __SYMBIAN32__
		dtouch_Region * self = new dtouch_Region(in_pointPool,in_labelPtrList);
		self->constructL();
		#else
		dtouch_Region * self = new (ELeave) dtouch_Region(in_pointPool,in_labelPtrList);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}
	
	static dtouch_Region * newL( Pool<DTPoint> *in_pointPool, DynamicPool<int *> *in_labelPtrPool, 
			ListPool<int *> *in_labelPtrList){
		#ifndef __SYMBIAN32__
		dtouch_Region * self = new dtouch_Region(in_pointPool, in_labelPtrPool, in_labelPtrList);
		self->constructL();
		#else
		dtouch_Region * self = new (ELeave) dtouch_Region(in_pointPool, in_labelPtrPool, in_labelPtrList);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}
	
	static dtouch_Region * newL( Pool<DTPoint> *in_pointPool, Pool<int *> *in_labelPtrPool ){
		#ifndef __SYMBIAN32__
		dtouch_Region * self = new dtouch_Region(in_pointPool,in_labelPtrPool);
		self->constructL();
		#else
		dtouch_Region * self = new (ELeave) dtouch_Region(in_pointPool,in_labelPtrPool);
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}

	~dtouch_Region()
	{
		_white = _potentialRoot = _branch = _onBorder = false;
		_noLeaves = _noBranches = 0;
		if( _labelPtrPoolAllocated ){
			delete _labelPtrPool;
		}
		if( _labelPtrListAllocated ){
			delete _labelPtrList;
		}else{
			_labelPtrList->empty();
		}
	}

	void empty(){
		if( _first != NULL ){
			_pool->pushList(_first, _last,_size);
			_first = _last = _current = NULL;
			_white = _potentialRoot = _branch = _onBorder = false;
			_noLeaves = _noBranches = 0;
			_size = 0;
			_labelPtrList->empty();
			_min = DTPoint(-1,-1);
			_max = DTPoint(-1,-1);
		}
	}

	void appendL( const int &x, const int &y ){
		if( _first == NULL ){
			_first = _pool->getElementL();
			_first->Data().x = x;
			_first->Data().y = y;
			//_current = _first;
			_last = _first;
		}
		else{
			_last->setNext( _pool->getElementL() );
			_last = _last->getNext();
			_last->Data().x = x;
			_last->Data().y = y;
			
			// not too sure about the following
			//current = last;
		}
		_size++;
		return;
	}

	void append( dtouch_Region *src ){
		if( src->isEmpty() ){
			// do nothing
			return;
		}
		if( _first == NULL ){
			_first = src->_first;
			_current = _first;
			_last = src->_last;

		}
		else{
			_last->setNext( src->_first );
			_last = src->_last;
			// not too sure about the following
			//_current = _last;
		}
		_size += src->_size;

		src->_size = 0;
		src->_first = NULL;
		src->_current = NULL;
		src->_last = NULL;
		return;
	}
	
	//int getSize(){ return _size; }
	using ListPool<DTPoint>::getFirstData;
	using ListPool<DTPoint>::getSize;
	using ListPool<DTPoint>::getData;
	using ListPool<DTPoint>::isEmpty;
	using ListPool<DTPoint>::reset;
	using ListPool<DTPoint>::nullTest;
	using ListPool<DTPoint>::fwd;
	
	bool isLabelPtrListNull( ){ return _labelPtrList->isNull(); }
	void emptyLabelPtrList( ){ _labelPtrList->empty(); }
	void appendToLabelPtrListL( int *data ){ _labelPtrList->appendL(data); }
	void appendToLabelPtrList( ListPool<int *> *data ){ _labelPtrList->append(data); }
	void resetLabelPtrList(){ _labelPtrList->reset(); }
	void fwdLabelPtrList(){ _labelPtrList->fwd(); }
	ListPool<int *> *getLabelPtrList( ){ return _labelPtrList; }

	int * getLabelPtrListData( ){ return _labelPtrList->getData(); }
	

	void box( DTPoint &min, DTPoint &max, int width, int height);
	//List<DTPoint> *getFirstComponent();
	//DTPoint *getCorners();
	DTPoint getCentre();
	DTPoint getCentreRound();
	
	const bool &getWhite() const { return _white; }

	#ifndef __SYMBIAN32__
	PointList * getPointList( int ** labelsMap, int width, int height );
	#endif
};

/*
bool operator==(const dtouch_Region& a, const dtouch_Region& b){
	// TODO: see if b contains all and only the points in a
	return false;
}
*/

#ifndef __SYMBIAN32__
PointList * intersect(dtouch_Region * a, dtouch_Region * b, 
					  int ** labelsMapA, int ** labelsMapB, 
					  int width, int height);

PointList * exclude(dtouch_Region * a, dtouch_Region * b, 
					int ** labelsMapA, int ** labelsMapB, 
					int width, int height);
#endif

#endif
