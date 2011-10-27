/***************************************************************************
                          regionadjacencygraph.h  -  description
                             -------------------
    begin                : Wed Nov 26 2003
    copyright            : (C) 2003 by Enrico Costanza
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

#ifndef EC_REGIONADJACENCYGRAPH
#define EC_REGIONADJACENCYGRAPH

#ifndef _EC_GRAPHSIZE
	#ifdef __SYMBIAN32__
		#define _EC_GRAPHSIZE 1000
		//#define _EC_GRAPHSIZE 500
	#else
		#define _EC_GRAPHSIZE 4000
	#endif //__SYMBIAN32__
#endif // _EC_GRAPHSIZE

#include "region.h"

#include "types.h"
#include "stack.h"
#include "listhash.h"

#ifdef _EC_DEBUG
#include "graphicimage.h"
#endif // _EC_DEBUG

//#define DT_LOG(x) //(RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L(x) ))

class RAGBuilder;

void box(dtouch_Region *reg, DTPoint &min, DTPoint &max, const int& width, const int& height);

class RegionAdjacencyGraph{
protected:
	int _width;
	int _height;
	int _size;
	int _used;

	ListHash<int> **_edges;

	dtouch_Region **_region;
	Pool<DTPoint> *_regionsPool;

	int **_labelsMap;
	int *_labels;
	int _usedLabelPtrs;
	PDStack<int> * _availableLabels;

	bool _leavesMarked;
	bool _branchesMarked;

	bool *_stored;

	DynamicPool<int *> *_labelsPtrPool;

	void expandL();

	#ifdef _EC_DEBUG
	//debug use only!
	GraphicImage * _display;
	#endif

	RegionAdjacencyGraph() {}

	RegionAdjacencyGraph( const int &in_width, const int &in_height, const int &initialSize );

	void constructL();

public:
	//static RegionAdjacencyGraph * newL( const int &in_width, const int &in_height, const int &initialSize=_EC_GRAPHSIZE )
	static RegionAdjacencyGraph * newL( const int &in_width, const int &in_height, const int &initialSize=_EC_GRAPHSIZE )
	{
		#ifndef __SYMBIAN32__
		RegionAdjacencyGraph * self = new RegionAdjacencyGraph( in_width,  in_height, initialSize );
		self->constructL();
		#else
		RegionAdjacencyGraph * self = new (ELeave) RegionAdjacencyGraph( in_width,  in_height, initialSize );
		CleanupStack::PushL(self);
		self->constructL();
		CleanupStack::Pop(self);
		#endif
		return self;
	}

	#ifdef _EC_DEBUG
	setDisplay( GraphicImage * in_display ) { _display = in_display; }
	#endif

	~RegionAdjacencyGraph();

	void reset(){
		dtouch_Region ** regionPtr = _region;
		ListHash<int> ** edgePtr = _edges;
		for(int i=0;i<_size;i++){
			(*edgePtr++)->empty();
			(*regionPtr++)->empty();
		}
		memset(_labels,0,_width*_height*sizeof(int));

		_used = 0;
		_usedLabelPtrs = 0;
		_leavesMarked = false;
		_branchesMarked = false;
		_background = -1;

		_availableLabels->reset();

		#ifndef __SYMBIAN32__
		for(int i=0;i<_size;i++){_distance[i]=0;}
		#endif // def __SYMBIAN32__
	}


	void linkL( const int &node1, const int &node2 ){ _edges[node1]->appendL(node2); _edges[node2]->appendL(node1); }

	bool check( const int &node1, const int &node2 ) const {
		if(_edges[node1]->getSize() > _edges[node2]->getSize()){
			return _edges[node2]->check(node1);
		}else{
			return _edges[node1]->check(node2);
		}
	}

	int mergeL( const int &node1, const int &node2 );

	const int &getSize() const { return _size; }
	ListHash<int> **getEdges() { return _edges; }
	dtouch_Region **getRegions() { return _region; }
	dtouch_Region *getRegion( const int &i ) { return _region[i]; }

	const int &getNoRegions() const { return _used; }
	const int &getDegree( const int &i ) const { return _edges[i]->getSize(); }
	//const int &getWhite( const int &i ) const { return _region[i]->_white; }
	int getWhite( const int &i ) const { return _region[i]->getWhite(); }
	const int &getNoLeaves( const int &i ) const { return _region[i]->_noLeaves; }

	ListHash<int> *getEdges(const int &i){ return _edges[i]; }

	int * getNewLabelL(){
		if( _availableLabels->isEmpty() ){
			if( _used >= _size - 1 ){
				// expand graph
				expandL();
			}
			_labels[_usedLabelPtrs] = _used++;
			return _labels + _usedLabelPtrs++;
		}
		_labels[_usedLabelPtrs] = _availableLabels->pop();
		return _labels + _usedLabelPtrs++;
	}

	const int &getWidth() const { return _width; }
	const int &getHeight() const { return _height; }
	bool * &getStored() { return _stored; }

	IntList * findFiducialRootDownL( const int *connections, int tolerance );

	//void markLeavesAndBranches();

	void markLeaves( bool whiteBackground = true );
	void markAllLeaves( );
	void markAllBranches( );

	void markAllLeaves( int tolerance );
	void markAllBranches( int tolerance );
	FiducialDataList * findFiducialsL( int tolerance );

	int* findBranchesL( const int root, const int *connections );
	int* findBranchesGenericL( const int root, const int *connections );
	FiducialDataList * findFiducialsL();

	void calculateInfo( FiducialData& data );

	void decodeFiducialLinearL( int root, int *fiducialSequence, FiducialData &fd );
	//void decodeFiducial4L( int root, int *fiducialSequence, FiducialData &fd );
	void decodeFiducial5L( int root, int *fiducialSequence, FiducialData &fd );
	void decodeFiducial6L( int root, int *fiducialSequence, FiducialData &fd );
	//DTPoint decodeFiducialGenericL( int root, int *fiducialSequence, int &type, int &angle );

	FiducialDataList * findFiducialsL( int *fiducialSequence, //FiducialData **fiducialData,
											bool usingLinearDecoding=true );

	friend class RAGBuilder;

#ifndef __SYMBIAN32__
protected:
	int * _distance;

	// method used to measure the distance
	// of nodes from a root
	void visit(int node, int distance, int parent);
public:
	void measureDistanceFromRoot( int root, int background );
	int getDistance( int index ){
		return _distance[index];
	}
	IntList * getSequenceL(int root, int background);
	IntList * getSequenceL(int root, int background, IntList * map);
#endif // __SYMBIAN32__

public:
	#ifndef __SYMBIAN32__
	FiducialData analyseFiducialL( bool whiteBackground, IntList * badRegions );
	bool compareFiducialL( bool whiteBackground,	FiducialData target,
							IntList * badRegions );

	char * getDotString( bool complete = false );
	char * getStatsString();
	#endif
protected:
	int _background;
public:
	int getBackgroundNode(){ return _background; }

	int ** getLabelsMap( ){ return _labelsMap; }

};


#endif

