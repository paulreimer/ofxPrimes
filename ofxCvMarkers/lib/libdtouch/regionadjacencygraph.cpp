/***************************************************************************
                         regionadjacencygraph.cpp  -  description
                             -------------------
    begin                : Wed Nov 26 2003
    copyright            : (C) 2003 by Enrico Costanza
    email                : e.costanza@ieee.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "regionadjacencygraph.h"

#include "listhash.h"

//debug use only!
#include "graphicimage.h"

#ifndef __SYMBIAN32__
#include <iostream>
#include <sstream>
#endif //__SYMBIAN32__

#ifndef __SYMBIAN32__
using std::cout;
using std::endl;
using std::cerr;
#endif

#ifdef __SYMBIAN32__
#define INT_MAX 16384
#include <flogger.h>
#endif //__SYMBIAN32__

int collinear( const DTPoint &a, const DTPoint &b, const DTPoint &c ){
  return abs(a.x * b.y + b.x * c.y + c.x * a.y - c.x * b.y - b.x * a.y - a.x * c.y);
}

RegionAdjacencyGraph::RegionAdjacencyGraph( const int &in_width, const int &in_height, const int &initSize )	:
    _width (in_width),
    _height (in_height),
    _size( initSize ),
	_used(0),
	_usedLabelPtrs(0),
	_leavesMarked(false),
	_branchesMarked(false),
	_background(-1)
{
}

void RegionAdjacencyGraph::constructL(){
	// WARNING:
	// using a DynamicPool can cause a decrease in performance, but it saves memory
	#ifndef __SYMBIAN32__
	_regionsPool = StaticPool<DTPoint>::newL(_width*_height);
	_edges = new ListHash<int>*[_size];
	_stored = new bool[_width*_height];

	_region = new dtouch_Region*[_size];
	_labelsPtrPool = DynamicPool<int *>::newL(_size);
	dtouch_Region ** l_region = _region;
	ListHash<int>** l_edges = _edges;
	for(int i=0;i<_size;i++){
		*l_edges++ = ListHash<int>::newL(_size);
		//_region[i] = new dtouch_Region(_regionsPool, _labelsPtrPool);
		*l_region++ = dtouch_Region::newL(_regionsPool, _labelsPtrPool);
	}

	_labelsMap = new int*[_width*_height];
	_labels = new int[_width*_height];

	_availableLabels = PDStack<int>::newL();
	#else
	_regionsPool = DynamicPool<DTPoint>::newL(_width*_height / 5);

	_edges = new (ELeave) ListHash<int>*[_size];
	_stored = new (ELeave) bool[_width*_height];

	_region = new (ELeave) dtouch_Region*[_size];
	_labelsPtrPool = DynamicPool<int *>::newL(_size);
	dtouch_Region ** l_region = _region;
	ListHash<int>** l_edges = _edges;
	for(int i=0;i<_size;i++){
		*l_edges++ = ListHash<int>::newL(_size);
		//_region[i] = new dtouch_Region(_regionsPool, _labelsPtrPool);
		*l_region++ = dtouch_Region::newL(_regionsPool, _labelsPtrPool);
	}

	_labelsMap = new (ELeave) int*[_width*_height];
	_labels = new (ELeave) int[_width*_height];

	_availableLabels = PDStack<int>::newL();
	#endif //__SYMBIAN32__


	#ifndef __SYMBIAN32__
	_distance = new int[_size];
	#endif // __SYMBIAN32__
	//reset();
}


void RegionAdjacencyGraph::expandL(void){
	#ifdef _EC_DEBUG
	std::cout << "**********************************************" << std::endl;
	std::cout << "**********************************************" << std::endl;
	std::cout << "**********************************************" << std::endl;
	#endif
	#ifdef __SYMBIAN32__
	//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("RegionAdjacencyGraph::expand called") );
	#endif // __SYMBIAN32__

	int oldSize = _size;
	_size *= 2;

	#ifndef __SYMBIAN32__
	ListHash<int> **tmpEdges = new ListHash<int>*[_size];
	for(int i=0;i<oldSize;i++){
		tmpEdges[i] = _edges[i];
		_edges[i]->expandL();
	}
	delete [] _edges;
	_edges = tmpEdges;

	dtouch_Region **tmpRegion = new dtouch_Region*[_size];

	for(int i=0;i<oldSize;i++){
		tmpRegion[i] = _region[i];
	}
	delete [] _region;
	_region = tmpRegion;

	for(int i=oldSize;i<_size;i++){
		_edges[i] = ListHash<int>::newL(_size);
		_region[i] = dtouch_Region::newL(_regionsPool);
	}

	#else
	ListHash<int> **tmpEdges = new (ELeave) ListHash<int>*[_size];
	CleanupStack::PushL(tmpEdges);
	for(int i=0;i<oldSize;i++){
		tmpEdges[i] = _edges[i];
		_edges[i]->expandL();
	}
	delete [] _edges;
	CleanupStack::Pop(tmpEdges);
	_edges = tmpEdges;

	dtouch_Region **tmpRegion = new (ELeave) dtouch_Region*[_size];

	for(int i=0;i<oldSize;i++){
		tmpRegion[i] = _region[i];
	}
	delete [] _region;
	_region = tmpRegion;

	for(int i=oldSize;i<_size;i++){
		_edges[i] = ListHash<int>::newL(_size);
		_region[i] = dtouch_Region::newL(_regionsPool);
	}

	#endif

	#ifndef __SYMBIAN32__
	int * tmpDistance = new int[_size];
	memcpy(tmpDistance,_distance,oldSize);
	delete [] _distance;
	_distance = tmpDistance;
	for(int i=0;i<_size;i++){_distance[i]=0;}
	#endif // __SYMBIAN32__
}

RegionAdjacencyGraph::~RegionAdjacencyGraph(){
	for(int i=0;i<_size;i++){
		delete _edges[i];
		delete _region[i];
	}

	delete _regionsPool;
	delete _labelsPtrPool;

	delete [] _edges;
	delete [] _region;
	delete [] _labelsMap;
	delete [] _labels;
	delete [] _stored;

	#ifndef __SYMBIAN32__
	delete [] _distance;
	#endif // __SYMBIAN32__

	delete _availableLabels;
	_availableLabels = NULL;
}

int RegionAdjacencyGraph::mergeL( const int &node1, const int &node2 ){
	int src, dest;
	int result;

	if( _region[node1]->isEmpty() || _region[node2]->isEmpty()  ){
		return 0;
	}

	// set the source to be the node with smaller degree
	if( _edges[node1]->getSize() > _edges[node2]->getSize() ){
		src = node2;
		dest = node1;
		result = 1;
	}else{
		src = node1;
		dest = node2;
		result = 2;
	}

	// merge the edges of source in the edges of destination
	for( _edges[src]->reset(); !(_edges[src]->nullTest()); _edges[src]->fwd() ){
		int currentEdge = _edges[src]->getData();

		// if the current edge is already in dest
		if( _edges[dest]->check( currentEdge ) ){
			// just remove it from the target node
			_edges[currentEdge]->remove(src);
		}else{
			_edges[dest]->appendL(currentEdge);
			_edges[currentEdge]->replace(dest,src);
		}
	}

	// make the source edges list empty!
	_edges[src]->empty();

	// change the labels of source
	// this loop changes the pointee
	for( _region[src]->resetLabelPtrList();
		!(_region[src]->isLabelPtrListNull());
				_region[src]->fwdLabelPtrList() )
	{
		*(_region[src]->getLabelPtrListData()) = dest;
	}

	// now transfer the pointers to the dest region
	_region[dest]->appendToLabelPtrList( (_region[src]->getLabelPtrList()) );
	// the following should be useless: check!
	//_region[src]->emptyLabelPtrList();

	// add the pixels of source to the destination list
	_region[dest]->append(_region[src]);

	// because the src region can be reused, the _onBorder flag should reset
	// NOTE: commented out because the _onBorder flags are set
	// after the end of graph construction
	//_region[src]->_onBorder = false;

	// add the source label to the stack of available labels
	_availableLabels->pushL( src );

	return result;
}



/*****************************************
 *****************************************
 **                                     **
 **   Methods for finding fiducials     **
 **   (several different versions)      **
 **                                     **
 *****************************************
 *****************************************/


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method performs a preparatory step needed for the fiducial         **
 **  detection "from root down (to leaves)".                                 **
 **                                                                          **
 **  The graph (or rather tree) is transversed and all the one-connected     **
 **  black nodes are marked as leaves.                                       **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
void RegionAdjacencyGraph::markLeaves( bool whiteBackground ){
	if( _leavesMarked ){
		return;
	}

	// I need to avoid the regions on the border of the image
	// to reduce false positives
	// ..this will probably cut out some fiducials to be recognized when they are
	// on the border of the image but this is better than having > false positives
	// all the regionLabels are in the labels map, so the regions on the border
	// can be found by just scanning the labels map
	int ** labelsPtrTop = _labelsMap;
	int ** labelsPtrBottom = _labelsMap+(_height-1)*_width;
	for(int x=0;x<_width;x++){
		_region[ *(*labelsPtrTop++) ]->_onBorder = true;
		_region[ *(*labelsPtrBottom++) ]->_onBorder = true;
	}
	int ** labelsPtrLeft = _labelsMap;
	int ** labelsPtrRight = _labelsMap+(_width-1);
	for(int y=0;y<_height;y++){
		_region[ **labelsPtrLeft ]->_onBorder = true;
		labelsPtrLeft += _width;
		_region[ **labelsPtrRight ]->_onBorder = true;
		labelsPtrRight += _width;
	}

	// the following defines the tolerance on the degree
	// for a node to be marked as a leaf
	//int tolerance = 1;

	// count and store how many black leaves in each white node
	// for+if combination that means: "for each white node n"
	dtouch_Region ** regPtr = _region;
	for(int n=0; n<_used; n++){
		if( (*regPtr)->_white == whiteBackground ){
			for( _edges[n]->reset(); !_edges[n]->nullTest(); _edges[n]->fwd() ){
				//for each subnode s of n
				int s = _edges[n]->getData();
				if( _edges[s]->getSize() == 1 ){
					//if s is a leaf increment the number of leaves in n
					(*regPtr)->_noLeaves++;
				}
			}
		}
		regPtr++;
	}

	_leavesMarked = true;

	return;
}

int compareOneToMany( const int &data, const int *array, const int &tolerance)
/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This function checks if 'data' is contained in 'array' with a given     **
 **  tolerance                                                               **
 **  If so, it returns the position of the first element in 'array' that     **
 **  matches 'data', else it returns -1.                                     **
 **  'array' is a variable lenght array. Its dimension must be at least 1,   **
 **  the value of the first element always indicates the number of other     **
 **  elements.                                                               **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
{
  for(int i=1; i<=array[0]; i++){
    if( abs(data - array[i])<=tolerance ){
      return i;
    }
  }
  return -1;
}

int compareOneToMany( const int &data, IntList * list,
						 const int &tolerance){
	for(list->reset();!(list->nullTest());list->fwd()){
		if( abs( data - list->getData() )<=tolerance ){
			return 1;
		}
	}
	return -1;
}

void box(dtouch_Region *reg, DTPoint &min, DTPoint &max, const int& width, const int& height){
	int xMin = width;
	int xMax = 0;
	int yMin = height;
	int yMax = 0;

	for(reg->reset();!reg->nullTest();reg->fwd()) {
		DTPoint tmp = reg->getData();
		(tmp.x > xMax) ? (xMax=tmp.x) : (NULL);
		(tmp.y > yMax) ? (yMax=tmp.y) : (NULL);
		(tmp.x < xMin) ? (xMin=tmp.x) : (NULL);
		(tmp.y < yMin) ? (yMin=tmp.y) : (NULL);
	}

	min.x = xMin;
	min.y = yMin;
	max.x = xMax;
	max.y = yMax;

	return;
}




IntList *RegionAdjacencyGraph::findFiducialRootDownL( const int *connections, int tolerance )
/**
 * connections is an array that rapresents a small graph, it has only 2 levels,
 * the first element is the "root" it indicates the number of subnodes that it has,
 * so it is also the size of the array
 */
{
	int branchTolerance = tolerance +1 ;
	IntList * result;
	result = IntList::newL();

	//result[0] = 0;
	#ifndef __SYMBIAN32__
	int * connectionsCopy = new int[connections[0] +2];
	#else
	int * connectionsCopy = new (ELeave) int[connections[0] +2];
	CleanupStack::PushL(connectionsCopy);
	#endif

	connectionsCopy[0] = connections[0] + 1;
	connectionsCopy[1] = -1;

	// for&if combination: look among the BLACK nodes
	for(int i=0;i<_used;i++){
		if( _edges[i]->getSize() == connectionsCopy[0] ){
			if( _region[i]->_onBorder == false ){
				// this might be the external black region of the fiducial
				memcpy(connectionsCopy+2, connections+1, connections[0]*sizeof(int));
				int branchMatches=0;
				for( _edges[i]->reset(); !_edges[i]->nullTest(); _edges[i]->fwd() ){
					// scan the subnodes (i.e. children ) of i..
					// NOTE: this could be made faster by stopping
					// when the number of matches is big enough
					int subnode = _edges[i]->getData();
					int leavesInSubnode = _region[subnode]->_noLeaves;
					int branchIndex = compareOneToMany( leavesInSubnode, connectionsCopy, 0);
					if( branchIndex > 0 ){
						// if this number of leaves is the same as the number of leaves in one
						// of the fiducial branches.
						// then increment the number of branch matches
						branchMatches++;
						// and remove this branch from the fiducial sequence
						// to avoid matching (i.e. counting) more than once
						connectionsCopy[branchIndex] = -1 ;
					}
				}
				if( branchMatches >= (connectionsCopy[0] - branchTolerance) ){
					// node i is a fiducial root, append i to the results
					// this implies the result array being reallocated
					result->appendL( i );
				}
			}
		}
	}

	CLN_POP( connectionsCopy );
	delete [] connectionsCopy;
	//connectionsCopy = NULL;

	return result;
}

/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method receives the index of a know fiducial root and a pointer    **
 **  to an array containing the fiducial tree (sequence).                    **
 **  It returns a pointer to an array containing the indexes of the branch   **
 **  regions corresponding to root. The first element of the array           **
 **  indicates how many branches have been found. This could be less than    **
 **  connections[0] because tolerance on the number of branches can be used  **
 **  for fiducial recognition. If a branch is not found the correspoing      **
 **  element in the array is set to -1                                       **
 **                                                                          **
 **  Note that this works only when the fiducial has no branches with the    **
 **  same number of leaves (i.e. for any k there is only (or at most)        **
 **  one branch with k leaves).                                              **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
int * RegionAdjacencyGraph::findBranchesL( const int root, const int *connections )
{
	#ifndef __SYMBIAN32__
	int * result = new int[connections[0]+1];
	#else
	int * result = new (ELeave) int[connections[0]+1];
	// no need for cleanup stack because the function
	// cannot leave after this point
	#endif
	result[0] = 0;

	for( int k=1; k<=connections[0];k++ ){
		result[k] = -1;
		for( _edges[root]->reset(); (!_edges[root]->nullTest())&&(result[k]==-1); _edges[root]->fwd() ){
			int subnode = _edges[root]->getData();
			// if subnodes has currentLeaves leaves
			// and the degree of subnode is equal to currentLeaves+1
			if( (_region[subnode]->_noLeaves == connections[k]) &&
						( getDegree(subnode) == (connections[k]+1) )
				){
				// if we are looking for an empty branch
				// make sure that the current subnode is a leaf
				//if( connections[k] == 0 ){
				//	if( getDegree(subnode) > 1 ){
				//		// this is the case..
				//	}
				//}
				result[k] = subnode;
				result[0]++;
			}
		}
	}

	return result;
}
//*/


/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method receives the index of a know fiducial root and a pointer    **
 **  to an array containing the fiducial tree (sequence).                    **
 **  It returns a pointer to an array containing the indexes of the branch   **
 **  regions in the same order as the sequence describing the fiducial       **
 **  tree                                                                    **
 **                                                                          **
 **  If more than one branch has the same number of leaves the               **
 **  corresponding results are (obviously) in random order.                  **
 **  For example, if the sequence is 4-1-1-2-3 the two 1-branches will       **
 **  appear in random order.                                                 **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
int* RegionAdjacencyGraph::findBranchesGenericL( const int root, const int *connections ){
	#ifndef __SYMBIAN32__
	int *result = new int[connections[0]+1];
	int *connectionsCopy = new int[connections[0]];
	#else
	int *result = new (ELeave) int[connections[0]+1];
	CleanupStack::PushL(result);
	int *connectionsCopy = new (ELeave) int[connections[0]];
	CleanupStack::Pop(result);
	// no need for cleanup stack because the function
	// cannot leave after this point
	#endif

	result[0] = connections[0];
	memset(result+1,0,connections[0]*sizeof(int));

	memcpy(connectionsCopy,connections+1,connections[0]*sizeof(int));

	for( _edges[root]->reset(); !_edges[root]->nullTest(); _edges[root]->fwd() ){
		int subnode = _edges[root]->getData();
		// for this subnode to be a  branch all of its other subnodes
		// must be leaves this means that the degree of subnode minus
		// the number of leaves must be equal to 1
		// NOTE THIS IS NOT 100% foolproof!!
		if( (_edges[subnode]->getSize() == _region[subnode]->_noLeaves + 1) && (!(_region[subnode]->_onBorder)) ){
			for(int j=0;j<connections[0];j++){
				if( getNoLeaves(subnode) == connectionsCopy[j] ){
					connectionsCopy[j] = -1;
					result[j+1] = subnode;
					break;
				}
			}
		}
	}

	delete [] connectionsCopy;
	return result;
}

void RegionAdjacencyGraph::decodeFiducialLinearL( int root, int *fiducialSequence, FiducialData &fd ){
	DTPoint result = this->getRegion(root)->getCentre();
	//int angle = -1;

	// store the branch region indexes in branch
	// in this way branch[n] corresponds to the branch with sequence[n] leaves
	int * branch = findBranchesGenericL( root, fiducialSequence );

	#ifndef __SYMBIAN32__
	DTPoint * centre = new DTPoint[branch[0]+1];
	//char * seq = new char[ fiducialSequence[0]+1 ];
	int * di = new int[fiducialSequence[0]];
	#else
	CleanupStack::PushL(branch);
	DTPoint * centre = new (ELeave) DTPoint[branch[0]+1];
	CleanupStack::PushL(centre);
	//char * seq = new (ELeave) char[ fiducialSequence[0]+1 ];
	//CleanupStack::PushL(seq);
	int * di = new (ELeave) int[fiducialSequence[0]];
	//CleanupStack::Pop(seq);
	CleanupStack::Pop(centre);
	CleanupStack::Pop(branch);
	#endif

	// calculate the centre of the fiducial and of each branch
	centre[0] = DTPoint(0,0);
	for(int j=1;j<=branch[0];j++){
		centre[j] = this->getRegion(branch[j])->getCentreRound();
		centre[0] += centre[j];
		fd.setBranchCentre( (j-1), centre[j]);
	}
	centre[0] /= branch[0];

	// cannot find corners in this case!
	fd.setCorner( 0, centre[0] );
	fd.setCorner( 1, centre[0] );
	fd.setCorner( 2, centre[0] );
	fd.setCorner( 3, centre[0] );

	// find the min branch degree and
	// the max branch degree
	int degMin = 255;
	int degMax = 0;
	for(int j=1;j<=fiducialSequence[0];j++){
		if( fiducialSequence[j] < degMin ){
			degMin = fiducialSequence[j];
		}
		if( fiducialSequence[j] > degMax ){
			degMax = fiducialSequence[j];
		}
	}

	DTPoint maxAvg = DTPoint(0,0);
	DTPoint minAvg = DTPoint(0,0);
	{
	// calculate the average centre of branches with degree degMin
	int minCounter=0;
	//branch[n] corresponds to the branch with sequence[n] leaves
	for(int i=1;i<=fiducialSequence[0];i++){
		if( fiducialSequence[i] == degMin ){
			minAvg.x+=centre[i].x;
			minAvg.y+=centre[i].y;
			minCounter++;
		}
	}
	minAvg.x = minAvg.x/minCounter;
	minAvg.y = minAvg.y/minCounter;
	}

	{
	// calculate the average centre of branches with degree degMax
	int maxCounter=0;
	//branch[n] corresponds to the branch with sequence[n] leaves
	for(int i=1;i<=fiducialSequence[0];i++){
		if( fiducialSequence[i] == degMax ){
			maxAvg.x+=centre[i].x;
			maxAvg.y+=centre[i].y;
			maxCounter++;
		}
	}
	maxAvg.x = maxAvg.x/maxCounter;
	maxAvg.y = maxAvg.y/maxCounter;
	}

	/*
	for(int i=0;i<fiducialSequence[0];i++){
		seq[i] = fiducialSequence[i+1] + 48;
	}
	seq[ fiducialSequence[0] ] = 0;
	*/

	for(int i=0;i<fiducialSequence[0];i++){
		di[i] = minAvg.dist(centre[i+1]);
	}

	int * seq = fd.TypeArray() + 1;
	// sort the rest of the branches by increasing distance
	// from the branch with smallest number of leaves
	for(int j=0;j<fiducialSequence[0];j++){
		for(int i=0;i<(fiducialSequence[0]-1);i++){
			if( di[i] > di[i+1] ){
				// swap the distances
				int swpInt = di[i];
				di[i] = di[i+1];
				di[i+1] = swpInt;

				// swap seq[i-1] and seq[i]
				swpInt = seq[i];
				seq[i] = seq[i+1];
				seq[i+1] = swpInt;
			}
		}
	}

	fd.Size() = this->_region[root]->getSize();
	fd.Type() = -1;

	int dx = maxAvg.x - minAvg.x;
	int dy = maxAvg.y - minAvg.y;
	if(dx==0){ dx = 1; }

	double angleD;
	if( dx!= 0 ){
		angleD = atan(-(double)dy/(double)dx);// + 0.8853;
	}else{
		if( dy>0 ){
			dx = 1;
		}else{
			dx = -1;
		}
		angleD = -acos(0.0);
	}

	int sign = ( (dx < 0)?(-1):(1) );

	int tmpAngle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
	if( tmpAngle > 180 ){
		tmpAngle -= 360;
	}
	fd.setAngle(tmpAngle);

	{
		int fw = 0;
		int fh = 0;
		DTPoint fmin,fmax;
		box(_region[root],fmin,fmax,_width,_height);
		fw = fmax.x - fmin.x;
		fh = fmax.y - fmin.y;
		fd.setWidth(fw);
		fd.setHeight(fh);
		DTPoint fcentre = DTPoint( (fmax.x + fmin.x)/2, (fmax.y + fmin.y)/2 );
		result = fcentre;
		//fd.setCentre(fcentre);
		fd.Centre() = fcentre;
	}

	//delete [] seq;
	delete [] di;
	delete [] centre;
	delete [] branch;

	return;
}

/*
void RegionAdjacencyGraph::decodeFiducial4L( int root, int *fiducialSequence, FiducialData &fd ){
	DTPoint result = getRegion(root)->getCentre();
	int type = -1;
	int angle = -1;
	fd.Angle() = -1;
	fd.Type() = -1;

	int *branch = findBranchesL( root, fiducialSequence );

	#ifndef __SYMBIAN32__
	DTPoint *centre = new DTPoint[branch[0]+1];
	DTPoint *imgCentre = new DTPoint[branch[0]+1];
	DTPoint *corner = new DTPoint[branch[0]+1];
	#else
	CleanupStack::PushL(branch);
	DTPoint *centre = new (ELeave) DTPoint[branch[0]+1];
	CleanupStack::PushL(centre);
	DTPoint *imgCentre = new (ELeave) DTPoint[branch[0]+1];
	CleanupStack::PushL(imgCentre);
	DTPoint *corner = new (ELeave) DTPoint[branch[0]+1];
	CleanupStack::Pop(imgCentre);
	CleanupStack::Pop(centre);
	CleanupStack::Pop(branch);
	#endif

	// calculate the centre of the fiducial and of each branch
	centre[0].x = 0;
	centre[0].y = 0;
	for(int j=1;j<=branch[0];j++){
		centre[j] = getRegion(branch[j])->getCentreRound();
		centre[0] += centre[j];
		fd.setBranchCentre( j-1, centre[j]);

		#ifdef _EC_DEBUG
		_display->plotDigit( centre[j], j, 255, 0, 0 );
		#endif
	}
	centre[0] /= branch[0];
	result = centre[0];

	#ifdef _EC_DEBUG
	_display->plotSegment(centre[1],centre[0],128,128,128);
	_display->plotSegment(centre[1],centre[2],255,  0,  0);
	_display->plotSegment(centre[1],centre[3],  0,255,  0);
	_display->plotSegment(centre[1],centre[4],  0,  0,255);
	#endif

	memcpy( imgCentre,centre,(branch[0]+1)*sizeof(DTPoint) );

	// transform the branch coordinates into a coordinate system centred on the
	// branch with one leaf and having the root centre in (5,15)
	// (this is needed to decode square fiducial sequences)
	// calculate the transfrom matrix M
	double M[2][3];
	{
		int dx = centre[0].x - centre[1].x;
		int dy = centre[0].y - centre[1].y;
		if(dx==0){ dx = 1; }

		double angleD = atan(-(double)dy/(double)dx) + 0.8853;
		int sign = ((dx<0)?(-1):(1));

		angle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
		if( angle > 180 ){
			angle -= 360;
		}

		M[0][0] = sign * cos(angleD);
		M[0][1] = sign * sin(-angleD);

		M[1][0] = - M[0][1];
		M[1][1] = M[0][0];

		#ifdef _EC_DEBUG
		cout << "angle: " << angleD;
		cout << "\t\tsign: " << sign << endl << endl;
		#endif
	}

	DTPoint offset(centre[0]);
	// apply the transformation
	for(int i=0;i<=branch[0];i++){
		DTPoint tmpPoint = centre[i] - offset;
		//centre[i].x = (int) round(M[0][0]*tmpPoint.x + M[0][1]*tmpPoint.y );
		centre[i].x = (int) ceil(M[0][0]*tmpPoint.x + M[0][1]*tmpPoint.y );
		//centre[i].y = (int) round(M[1][0]*tmpPoint.x + M[1][1]*tmpPoint.y );
		centre[i].y = (int) ceil(M[1][0]*tmpPoint.x + M[1][1]*tmpPoint.y );
	}

	#ifdef NO_DEBUG
	_display->plotSegment( (centre[1]+DTPoint(100,100)),
		(centre[0]+DTPoint(100,100)),   0, 255, 255 );
	_display->plotSegment( (centre[1]+DTPoint(100,100)),
		(centre[2]+DTPoint(100,100)), 255,   0,   0 );
	_display->plotSegment( (centre[1]+DTPoint(100,100)),
		(centre[3]+DTPoint(100,100)),   0, 255,   0 );
	_display->plotSegment( (centre[1]+DTPoint(100,100)),
		(centre[4]+DTPoint(100,100)),   0,   0, 255 );
	#endif


	DTPoint d1;
	d1 |= (centre[1] - centre[0]);
	DTPoint d12;
	d12 |= (centre[1] - centre[2]);
	DTPoint d13;
	d13 |= (centre[1] - centre[3]);

	if( (d12.x>d1.x) && (d12.y<d1.y) ){
		if( d13.x < d1.x ){ type = 234; } else { type = 243; }
	}else if( (d13.x>d1.x) && (d13.y<d1.y) ){
		if( d12.x < d1.x ){ type = 324; } else { type = 342; }
	}else{
		if( d12.x < d1.x ){ type = 423; } else { type = 432; }
	}

	#ifdef _EC_DEBUG
	if( type > 500 ){
		cout << "type: " << type << endl;
	}
	#endif

	fd.Type() = type;
	fd.Angle() = angle;

	corner[0] = imgCentre[0];
	fd.setCorner(0, imgCentre[0]);
	for(int i=1;i<=branch[0];i++){
		DTPoint tmp = imgCentre[i] - imgCentre[0];
		tmp *= 22;
		tmp /= 10;
		corner[i] = imgCentre[0] + tmp;
	}

	#ifdef _EC_DEBUG
	_display->plotSegment(corner[1],corner[type/100],255,0,0);
	_display->plotSegment(corner[type/100],corner[type%10],255,0,0);
	_display->plotSegment(corner[type%10],corner[(type/10)%10],255,0,0);
	_display->plotSegment(corner[(type/10)%10],corner[1],255,0,0);
	#endif

	fd.setCorner(0, corner[1]);
	fd.setCorner(1, corner[type/100]);
	fd.setCorner(2, corner[type%10]);
	fd.setCorner(3, corner[(type/10)%10]);

	delete [] centre;
	delete [] imgCentre;

	delete [] corner;

	delete [] branch;

	fd.Centre() = result;

	return;
}
*/

/***
 * this method works with the sequence { 5, 0, 1, 2, 3, 4 } ONLY
 * not any permutation of it
 * and expecially NOT { 5, 1, 2, 3, 4, 0 }

*/
void RegionAdjacencyGraph::decodeFiducial5L( int root, int *fiducialSequence, FiducialData& fd ){
	//cout << "decodeFiducial5L" << endl;
	//DTPoint result = getRegionCentre( getRegion(root) );
	DTPoint result = getRegion(root)->getCentreRound();
	fd.Centre() = result;

	fd.Type() = -1;
	fd.Angle() = 0;

	int * branch = findBranchesL( root, fiducialSequence );

	DTPoint centre[6];
	/*
	#ifndef __SYMBIAN32__
	DTPoint * centre = new DTPoint[branch[0]+1];
	#else
	CleanupStack::PushL(branch);
	DTPoint * centre = new (ELeave) DTPoint[branch[0]+1];
	CleanupStack::Pop(branch);
	#endif
	*/
	for(int i=0;i<6;i++){
		centre[i].x = 0;
		centre[i].y = 0;
	}

	// calculate the centre of the fiducial and of each branch
	centre[0].x = 0;
	centre[0].y = 0;

	// handle the empty branch separately
	centre[1] = getRegion(branch[1])->getCentreRound();
	#ifdef _EC_DEBUG
	_display->plotDigit( centre[1], 1, 255, 255, 0 );
	#endif
	fd.setBranchCentre(0, centre[1]);
	for(int j=2;j<=5;j++){
		if( branch[j]>= 0 ){
			centre[j] = getRegion(branch[j])->getCentreRound();
			centre[0] += centre[j];
			fd.setBranchCentre(j-1, centre[j]);
			#ifdef _EC_DEBUG
			_display->plotDigit( centre[j], j-1, 255, 255, 0 );
			#endif
		}
	}
	centre[0] /= 4;
	result = centre[0];

	fd.Centre() = centre[0];

	int seq[4];

	// find the two branches collinear with the empty one
	int permTable[6][2] = {	{2,3}, {2,4}, {2,5}, {3,4}, {3,5}, {4,5} };
	// start from the first possibility: {2,3}
	int minCollinear = collinear( centre[1], centre[2], centre[3] );
	seq[0] = 2;
	seq[2] = 3;
	for(int i=1;i<6;i++){
		int j1 = permTable[i][0];
		int j2 = permTable[i][1];
		int currCollinear = collinear( centre[1], centre[j1], centre[j2] );
		if( currCollinear < minCollinear ){
			minCollinear = currCollinear;
			seq[0] = j1;
			seq[2] = j2;
		}
	}

	// sort seq[0] and seq[2]
	if( centre[1].dist(centre[seq[0]]) > centre[1].dist(centre[seq[2]]) ){
		// swap the two
		int tmp = seq[0];
		seq[0] = seq[2];
		seq[2] = tmp;
	}

	// between the two branches left over, the closest to
	// the empty branch is the second in the sequence
	int minDist = 1000000;
	for(int i=2;i<=5;i++){
		if( (i!=seq[0]) && (i!=seq[2]) ){
			int currDist = centre[1].dist( centre[i] );
			if( currDist < minDist ){
				minDist = currDist;
				seq[1] = i;
			}
		}
	}

	// find the only other branch
	// that's the last of the sequence
	for(int i=2;i<=5;i++){
		if( (i!=seq[0]) && (i!=seq[1]) && (i!=seq[2]) ){
			seq[3] = i;
		}
	}

	/*
	#ifdef _EC_DEBUG
	// plot segments joining the branch centres
	_display->plotSegment( centre[seq[1]], centre[seq[2]], 0, 128, 0 );
	_display->plotSegment( centre[seq[2]], centre[seq[3]], 0, 128, 0 );
	_display->plotSegment( centre[seq[3]], centre[seq[4]], 0, 128, 0 );
	_display->plotSegment( centre[seq[4]], centre[seq[1]], 0, 128, 0 );
	#endif
	//*/

	/*
	char seqChar[5];
	for(int k=0;k<4;k++){
		seqChar[k] = '0' + seq[k];
	}
	seqChar[4] = 0;
	*/
	//fd.Type() = seq[3]*1 + seq[2]*10 + seq[1]*100 + seq[0]*1000;
	//fd.Type() = atoi( seqChar ) + 10000;

	// update the sequence stored in fd
	int * tmpSeq = fd.TypeArray();
	for(int k=0;k<4;k++){
		tmpSeq[k+2] = seq[k] - 1;
	}

	// the angle can be calculated as atan( (y[0] - y[4]) / (x[0] - x[4]) )
	DTPoint p0 = centre[seq[0]];
	DTPoint p1 = centre[seq[1]];

	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;

	double angleD;
	if( dx!= 0 ){
		angleD = atan(-(double)dy/(double)dx);// + 0.8853;
	}else{
		if( dy>0 ){
			dx = 1;
		}else{
			dx = -1;
		}
		angleD = -acos(0.0);
	}

	int sign = ( (dx < 0)?(-1):(1) );

	int tmpAngle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
	if( tmpAngle > 180 ){
		tmpAngle -= 360;
	}
	fd.setAngle(tmpAngle);

	for(int i=0;i<4;i++){
		DTPoint tmp = centre[seq[i]] - fd.Centre();
		tmp *= 42;
		tmp /= 10;
		fd.setCorner( i, fd.Centre() + tmp );
	}

	DTPoint tmp = fd.getCorner(2);
	fd.setCorner(2, fd.getCorner(3));
	fd.setCorner(3, tmp);

	//delete [] centre;
	delete [] branch;

	return;
}


void RegionAdjacencyGraph::decodeFiducial6L( int root, int *fiducialSequence, FiducialData &fd ){
	//DTPoint result = getRegionCentre( getRegion(root) );
	DTPoint result = getRegion(root)->getCentreRound();
	fd.Type() = -1;
	fd.Angle() = -1;

	int *branch = findBranchesL( root, fiducialSequence );

	DTPoint centre[7];
	/*
	#ifndef __SYMBIAN32__
	DTPoint * centre = new DTPoint[branch[0]+1];
	#else
	CleanupStack::PushL(branch);
	DTPoint * centre = new (ELeave) DTPoint[branch[0]+1];
	CleanupStack::Pop(branch);
	#endif
	*/

	// calculate the centre of the fiducial and of each branch
	centre[0].x = 0;
	centre[0].y = 0;
	for(int j=1;j<=6;j++){
		//cout << "branch["<<j<<"]:" << branch[j]; cout.flush();
		//centre[j] = getRegionCentre( getRegion(branch[j]) );
		centre[j] = getRegion(branch[j])->getCentreRound(  );
		centre[0] += centre[j];
		#ifdef _EC_DEBUG
		_display->plotDigit( centre[j], j, 255, 255, 0 );
		#endif
		//cout << " (" << centre[j].x << "," << centre[j].y << ")" << endl;
	}
	centre[0] /= 6;
	result = centre[0];

	#ifdef _EC_DEBUG
	_display->plotSegment(centre[1],centre[0],128,128,128);
	_display->plotSegment(centre[1],centre[2],255,  0,  0);
	_display->plotSegment(centre[1],centre[3],  0,255,  0);
	_display->plotSegment(centre[1],centre[4],  0,  0,255);
	_display->plotSegment(centre[1],centre[5],  0,255,255);
	_display->plotSegment(centre[1],centre[6],255,  0,255);
	#endif

	int counter = 0;
	int collinearity[10];
	int combination[10][2];
	//type = 9;
	for(int i=2;i<=5;i++){
		for(int j=i+1;j<=6;j++){
			int a = i;
			int b = j;
			int c = 1;

			collinearity[counter] = collinear( centre[a], centre[b], centre[c] );
			combination[counter][0] = a;
			combination[counter][1] = b;
			counter++;
		}
	}
	int min = 1000;
	int minIndex = 0;
	for(int i=0;i<10;i++){
		if( min > collinearity[i] ){
			min = collinearity[i];
			minIndex = i;
		}
	}

	int s[5];

	if( centre[1].dist(centre[combination[minIndex][0]]) < centre[1].dist(centre[combination[minIndex][1]]) ){
		s[1] = combination[minIndex][0];
		s[3] = combination[minIndex][1];
	}else{
		s[1] = combination[minIndex][1];
		s[3] = combination[minIndex][0];
	}

	for(int i=0;i<6;i++){
		collinearity[i] = collinear( centre[1], centre[0], centre[i+2] );
		//cout << i <<": "<< collinearity[i] << endl;
	}
	min = 1000;
	minIndex = 0;
	for(int i=0;i<6;i++){
		if( min > collinearity[i] ){
			min = collinearity[i];
			minIndex = i;
		}
	}
	minIndex+=2;
	// branch[minIndex] is the last in sequence
	s[4] = minIndex;

	int cnt=0;
	for(int n=2;n<7;n++){
		if( ( n != s[1] )&&( n != s[3] )&&( n != s[4] ) ){
			s[cnt] = n;
			cnt += 2;
		}
	}

	// look for the branch closer to the last among branch[s[0]] and branch[s[2]]
	if( centre[ s[4] ].dist(centre[ s[0] ]) < centre[ s[4] ].dist(centre[ s[2] ]) ){
		//swap 0 and 2 in sequence
		int tmp = s[0];
		s[0] = s[2];
		s[2] = tmp;
	}

	//fd.Type() = s[4]*1 + s[3]*10 + s[2]*100 + s[1]*1000 + s[0]*10000 + 100000;
	for(int i=0;i<5;i++){
		fd.TypeArray()[i+2] = s[i];
	}

	fd.setBranchCentre(0, centre[1]);
	for(int i=1;i<6;i++){
		fd.setBranchCentre(i, centre[ s[i-1] ]);
	}

	// the angle can be calculated as atan( (y[0] - y[4]) / (x[0] - x[4]) )
	DTPoint p0 = fd.getBranchCentre(1);
	DTPoint p1 = fd.getBranchCentre(5);

	double angleD = atan(-(double)(p1.y - p0.y)/(double)(p1.x - p0.x));// + 0.8853;

	int sign = (((p1.x - p0.x)<0)?(-1):(1));

	int tmpAngle = - ((int) ( angleD*180.0/3.14159265 ) - ( 90*(1-sign) )) ;
	if( tmpAngle > 180 ){
		tmpAngle -= 360;
	}
	fd.setAngle(tmpAngle);

	//delete [] centre;
	delete [] branch;

	fd.Centre() = result;

	return;
}

/*
DTPoint RegionAdjacencyGraph::decodeFiducialGenericL( int root, int *fiducialSequence, int &type, int &angle ){
	DTPoint result;
	type = -2;
	angle = -1;

	int *branch = findBranchesGenericL( root, fiducialSequence );

	//cout << "branch[0]: " << branch[0] << endl;
	//cout << "fiducialSequence[0]: " << fiducialSequence[0] << endl;

	if( branch[0] == fiducialSequence[0] ){
		// calculate the centre of the fiducial and of each branch
		DTPoint *centre = new DTPoint[branch[0]+1];
		centre[0].x = 0;
		centre[0].y = 0;

    for(int j=1;j<=branch[0];j++){
			centre[j] = getRegion(branch[j])->getCentre();
			centre[0] += centre[j];
      #ifdef _EC_DEBUG
			cout << "\tbranch["<<j<<"]:" << branch[j]; cout.flush();
			cout << " (" << centre[j].x << "," << centre[j].y << ")" << endl;
      #endif
		}
		centre[0] /= branch[0];
		result = centre[0];

		#ifdef _EC_DEBUG
	    cout << "centre: " << result << endl;
	    #endif

		delete [] centre;

	}else{
    result = getRegion(root)->getCentre();
  }

	delete [] branch;

	return result;
}
*/

// fiducialData: type, x, y, angle, region#
FiducialDataList * RegionAdjacencyGraph::findFiducialsL( int *fiducialSequence,
						 /*FiducialDataList *fiducialData,*/ bool usingLinearDecoding )
{
	int noFiducials = 0;
	markLeaves();
	IntList * result = findFiducialRootDownL(fiducialSequence, 0);

	CLN_PUSH(result);
	FiducialDataList * fd = FiducialDataList::newL();
	CLN_PUSH(fd);

	noFiducials = result->getSize();
	if( noFiducials == 0 ){
		CLN_POP(fd);
		CLN_POP(result);
		// !!! JMS - Fix memory leak
		delete result;
		//*fiducialData = NULL;
		return fd;
	}

	for(result->reset();!(result->nullTest());result->fwd()){
		FiducialData current;
		current.init(fiducialSequence);
		current.RootRegion() = result->getData();

		if( usingLinearDecoding ){
			decodeFiducialLinearL(result->getData(),fiducialSequence, current );
		}else{
			// use one of the specific methods
			if( fiducialSequence[0] == 6 ){
  				decodeFiducial6L(result->getData(),fiducialSequence, current );
			}else if( fiducialSequence[0] == 5 ){
  				decodeFiducial5L(result->getData(),fiducialSequence, current );
			//}else if( fiducialSequence[0] == 4 ){
			//	decodeFiducial4L(result->getData(),fiducialSequence, current );
			}else{
				// some other fiducial sequence for which
				// geometric decoding is not supported
				// fall back on linear decoding
				decodeFiducialLinearL(result->getData(),fiducialSequence, current );
			}
		}
		//cout << current.getType() << endl;
		fd->appendL( current );
	}

	CLN_POP(fd);
	CLN_POP(result);
	delete result;

	return fd;
}

// methods for generalized fiducials

/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method performs a preparatory step needed for the fiducial         **
 **  detection "from root down (to leaves)".                                 **
 **                                                                          **
 **  The graph (or rather tree) is transversed and all the one-connected     **
 **  nodes are marked as leaves.                                        	 **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
void RegionAdjacencyGraph::markAllLeaves( ){
	if( _leavesMarked ){
		return;
	}
	// I need to avoid the regions on the border of the image
	// these should be marked as "nonLeaves", because they cannot
	// be contained in other regions..
	// ..this will probably cut out some fiducials to be recognized when they are
	// on the border of the image but this is better than having > false positives
	// all the regionLabels are in the labels map, so the regions on the border
	// can be found by just scanning the labels map
	int ** labelsPtrTop = _labelsMap;
	int ** labelsPtrBottom = _labelsMap+(_height-1)*_width;
	for(int x=0;x<_width;x++){
		_region[ *(*labelsPtrTop++) ]->_onBorder = true;
		_region[ *(*labelsPtrBottom++) ]->_onBorder = true;
	}
	int ** labelsPtrLeft = _labelsMap;
	int ** labelsPtrRight = _labelsMap+(_width-1);
	for(int y=0;y<_height;y++){
		_region[ **labelsPtrLeft ]->_onBorder = true;
		labelsPtrLeft += _width;
		_region[ **labelsPtrRight ]->_onBorder = true;
		labelsPtrRight += _width;
	}

	// TODO: it should be faster to use pointers to _region and _edges
	// rather than accessing _region[n] all the time

	// look for leaves (1-connected regions)
	// and register on their parent nodes
	for(int n=0; n<_used; n++){
		if( (_edges[n]->getSize() == 1) && (!(_region[n]->_onBorder)) ){
			// this is a leaf
			// get the parent, i.e. the first and only connected node
			int p = _edges[n]->getFirstData();
			_region[p]->_noLeaves++;
		}
	}

	_leavesMarked = true;

	return;
}

/*
 * A node is defined as a "branch" if its degree is noLeaves + 1
 * Note that a node can be both a leaf and a branch!
 * */
void RegionAdjacencyGraph::markAllBranches( ){
	if( !_leavesMarked ){
		markAllLeaves();
	}

	// TODO: it should be faster to use pointers to _region and _edges
	// rather than accessing _region[n] all the time

	// look for branches
	for(int n=0; n<_used; n++){
		if( (_region[n]->_noLeaves > 0) &&
			(_edges[n]->getSize() == (_region[n]->_noLeaves + 1)) ){
			// this is a branch
			// mark it as such
			_region[n]->_branch = true;
		}
	}

	// register branches at their parents
	// TODO: look for a more efficient way to do this
	for(int n=0; n<_used; n++){
		if( _region[n]->_branch == true ){
			for( _edges[n]->reset(); !_edges[n]->nullTest(); _edges[n]->fwd() ){
				int sn = _edges[n]->getData();
				if( _region[sn]->_branch == false ){
					// this is the parent
					_region[sn]->_noBranches++;
					// stop searching
					break;
				}
			}
		}
	}

	_branchesMarked = true;

	return;
}

/*
 * This methods looks for generalized fiducials.
 * These are defined as nodes that contain only branches and leaves,
 * and at least some branches
 * */
FiducialDataList * RegionAdjacencyGraph::findFiducialsL(){
	if( !_branchesMarked ){
		markAllBranches();
	}
	FiducialDataList * fd = FiducialDataList::newL();
	CLN_PUSH(fd);

	// TODO: it should be faster to use pointers to _region and _edges
	// rather than accessing _region[n] all the time

	// look for branches
	// and register on their parent nodes
	for(int n=0; n<_used; n++){
		// TODO: should the following be _noBranches > 1 or _noBranches >= 1 ???
		if( (_edges[n]->getSize() > 1) && (_region[n]->_noBranches >= 1) &&
			(_edges[n]->getSize() == (_region[n]->_noBranches + _region[n]->_noLeaves + 1)) ){
			//( (_edges[n]->getSize() - (_region[n]->_noBranches + _region[n]->_noLeaves + 1)) <= 1 ) ){

			// if this root has more connections than we can store
			// skip it
			if( FiducialData::getMax() < _edges[n]->getSize() ){
				continue;
			}
			// this is a root, mark it as such
			// find the sequence of this fiducial (i.e. the sequence
			// of leaves in each branch, sorted in ascending order)
			IntList * seq = IntList::newL();
			CLN_PUSH(seq);
			for( _edges[n]->reset(); !(_edges[n]->nullTest()); _edges[n]->fwd() ){
				int sn = _edges[n]->getData();
				// ignore regions on border, they cannot be branches or leaves
				if( ! _region[sn]->_onBorder ){
					if( _region[sn]->_branch ){
						seq->appendL( _region[sn]->_noLeaves );
					}
					// if this is a leaf add zero
					if( _edges[sn]->getSize() == 1 ){
						seq->appendL( 0 );
					}
				}
			}
			sortIntList( seq );
			int * seqArray = IntList2ArrayL( seq );
			CLN_POP(seq);
			delete seq;
			FiducialData current;
			current.init( seqArray );
			delete [] seqArray;

			current.setColor( _region[n]->_white );
			current.setRootRegion(n);

			// the rest of the information about this fiducial
			// is calculated only if the marker is accepted
			//current.Size() = this->_region[n]->getSize();
			current.Centre() = this->_region[n]->getFirstData();

			fd->appendL( current );
		}
	}

	CLN_POP(fd);
	return fd;
}


void RegionAdjacencyGraph::calculateInfo( FiducialData& data ){
	int root = data.RootRegion();

	data.Size() = this->_region[root]->getSize();
	data.Centre() = this->_region[root]->getFirstData();

	//int len = data.getSequence()[0]+1;

	DTPoint branchCentre[FiducialData::_maxBranches];
	int seq[FiducialData::_maxBranches];

	seq[0] = data.getSequence()[0];
	branchCentre[0] = DTPoint(0,0);
	// calculate the centre of each branch
	// for this re-calculate the sequence
	//IntList seq;
	//IntList branchIndex;
	int counter=1;
	for( _edges[root]->reset(); !(_edges[root]->nullTest()); _edges[root]->fwd() ){
		int sn = _edges[root]->getData();
		// ignore regions on border, they cannot be branches or leaves
		if( ! _region[sn]->_onBorder ){
			if( _region[sn]->_branch ){
				seq[counter] = _region[sn]->_noLeaves;
				DTPoint min, max;
				_region[sn]->box(min,max,_width,_height);
				branchCentre[counter++] = DTPoint((min.x+max.x)/2,(min.y+max.y)/2);
			}
			// if this is a leaf add zero
			if( _edges[sn]->getSize() == 1 ){
				seq[counter] = 0;
				DTPoint min, max;
				_region[sn]->box(min,max,_width,_height);
				branchCentre[counter++] = DTPoint((min.x+max.x)/2,(min.y+max.y)/2);
			}
		}
	}

	// TODO: look for the region(s) with min degree and max degree
	int minDeg = 255;
	int maxDeg = 0;
	for(int i=1;i<=seq[0];i++){
		if( seq[i] < minDeg ){
			minDeg = seq[i];
		}
		if( seq[i] > maxDeg ){
			maxDeg = seq[i];
		}
	}

	DTPoint minPoint = DTPoint(0,0);
	DTPoint maxPoint = DTPoint(0,0);
	int minCounter = 0;
	int maxCounter = 0;
	for(int i=1;i<=seq[0];i++){
		if( seq[i] == minDeg ){
			minPoint += branchCentre[i];
			minCounter++;
		}
		if( seq[i] == maxDeg ){
			maxPoint += branchCentre[i];
			maxCounter++;
		}
	}
	minPoint/=minCounter;
	maxPoint/=minCounter;

	int dx = maxPoint.x - minPoint.x;
	int dy = maxPoint.y - minPoint.y;
	if(dx==0){ dx = 1; }

	double angleD = atan(-(double)dy/(double)dx);// + 0.8853;

	int angle = 0;
	int sign = ((dx<0)?(-1):(1));
	if( sign > 0 ){
		angle = 90 + ((int) ( angleD*180.0/3.14159265 ) ) ;
	}else{
		angle = - 90 + ((int) ( angleD*180.0/3.14159265 ) ) ;
	}

	data.Angle() = angle;

	{
		int fw = 0;
		int fh = 0;
		DTPoint fmin,fmax;
		box(_region[root],fmin,fmax,_width,_height);
		fw = fmax.x - fmin.x;
		fh = fmax.y - fmin.y;
		data.setWidth(fw);
		data.setHeight(fh);
	}

	return;
}

/*
 * This methods looks for generalized fiducials.
 * These are defined as nodes that contain only branches and leaves,
 * and at least some branches
 * */
FiducialDataList * RegionAdjacencyGraph::findFiducialsL(int tolerance){
	if( !_branchesMarked ){
		markAllBranches(tolerance);
	}
	FiducialDataList * fd = FiducialDataList::newL();
	CLN_PUSH(fd);

	// TODO: it should be faster to use pointers to _region and _edges
	// rather than accessing _region[n] all the time

	// look for branches
	// and register on their parent nodes
	for(int n=0; n<_used; n++){
		// TODO: should the following be _noBranches > 1 or _noBranches >= 1 ???
		if( (_edges[n]->getSize() > 1) && (_region[n]->_noBranches >= 1) &&
			(_edges[n]->getSize() == (_region[n]->_noBranches + _region[n]->_noLeaves + 1)) ){
			//( (_edges[n]->getSize() - (_region[n]->_noBranches + _region[n]->_noLeaves + 1)) <= 1 ) ){

			// if this root has more connections than we can stor
			// skip it
			if( FiducialData::getMax() < _edges[n]->getSize() ){
				continue;
			}
			// this is a root, mark it as such
			// find the sequence of this fiducial (i.e. the sequence
			// of leaves in each branch, sorted in ascending order)
			IntList * seq = IntList::newL();
			CLN_PUSH(seq);
			for( _edges[n]->reset(); !(_edges[n]->nullTest()); _edges[n]->fwd() ){
				int sn = _edges[n]->getData();
				if( _region[sn]->_branch ){
					seq->appendL( _region[sn]->_noLeaves );
				}
				// if this is a leaf add zero
				if( _edges[sn]->getSize() == 1 ){
					seq->appendL( 0 );
				}
			}
			sortIntList( seq );
			int * seqArray = IntList2ArrayL( seq );
			CLN_POP(seq);
			delete seq; seq = NULL;
			FiducialData current;
			current.init( seqArray );
			delete [] seqArray;

			current.setColor( _region[n]->_white );
			current.setRootRegion(n);
			// TODO: fill in the rest of the data for current!
			fd->appendL( current );
		}
	}

	CLN_POP(fd);
	return fd;
}

// methods for generalized fiducials

/******************************************************************************
 ******************************************************************************
 **                                                                          **
 **  This method performs a preparatory step needed for the fiducial         **
 **  detection "from root down (to leaves)".                                 **
 **                                                                          **
 **  The graph (or rather tree) is transversed and all the one-connected     **
 **  nodes are marked as leaves.                                        	 **
 **                                                                          **
 ******************************************************************************
 *****************************************************************************/
void RegionAdjacencyGraph::markAllLeaves( int tolerance ){
	if( _leavesMarked ){
		return;
	}
	// I need to avoid the regions on the border of the image
	// these should be marked as "nonLeaves", because they cannot
	// be contained in other regions..
	// ..this will probably cut out some fiducials to be recognized when they are
	// on the border of the image but this is better than having > false positives
	// all the regionLabels are in the labels map, so the regions on the border
	// can be found by just scanning the labels map
	int ** labelsPtrTop = _labelsMap;
	int ** labelsPtrBottom = _labelsMap+(_height-1)*_width;
	for(int x=0;x<_width;x++){
		_region[ *(*labelsPtrTop++) ]->_onBorder = true;
		_region[ *(*labelsPtrBottom++) ]->_onBorder = true;
	}
	int ** labelsPtrLeft = _labelsMap;
	int ** labelsPtrRight = _labelsMap+(_width-1);
	for(int y=0;y<_height;y++){
		_region[ **labelsPtrLeft ]->_onBorder = true;
		labelsPtrLeft += _width;
		_region[ **labelsPtrRight ]->_onBorder = true;
		labelsPtrRight += _width;
	}

	// TODO: it should be faster to use pointers to _region and _edges
	// rather than accessing _region[n] all the time

	// look for leaves (1-connected regions)
	// and register on their parent nodes
	for(int n=0; n<_used; n++){
		if( (_edges[n]->getSize() <= tolerance) && (!(_region[n]->_onBorder)) ){
			// this is a leaf
			// get the parent, i.e. the first and only connected node
			int p = _edges[n]->getFirstData();
			_region[p]->_noLeaves++;
		}
	}

	_leavesMarked = true;

	return;
}

/*
 * A node is defined as a "branch" if its degree is noLeaves + 1
 * Note that a node can be both a leaf and a branch!
 * */
void RegionAdjacencyGraph::markAllBranches( int tolerance ){
	if( !_leavesMarked ){
		markAllLeaves( tolerance );
	}

	// TODO: it should be faster to use pointers to _region and _edges
	// rather than accessing _region[n] all the time

	// look for branches
	for(int n=0; n<_used; n++){
		if( (_region[n]->_noLeaves > 0) &&
			(_edges[n]->getSize() == (_region[n]->_noLeaves + 1)) ){
			// this is a branch
			// mark it as such
			_region[n]->_branch = true;
		}
	}

	// register branches at their parents
	// TODO: look for a more efficient way to do this
	for(int n=0; n<_used; n++){
		if( _region[n]->_branch == true ){
			for( _edges[n]->reset(); !_edges[n]->nullTest(); _edges[n]->fwd() ){
				int sn = _edges[n]->getData();
				if( _region[sn]->_branch == false ){
					// this is the parent
					_region[sn]->_noBranches++;
					// stop searching
					break;
				}
			}
		}
	}

	_branchesMarked = true;

	return;
}

#ifndef __SYMBIAN32__
void RegionAdjacencyGraph::visit(int node, int distance, int parent){
	//cout << "visit " << node << ", parent: " << parent << ", dist: " << distance << endl;
	_distance[node] = distance;
	for( _edges[node]->reset(); !_edges[node]->nullTest(); _edges[node]->fwd() ){
		int sn = _edges[node]->getData();
		//cout << " sn: " << sn << " | parent: " << parent << endl;
		if( (sn != parent) && (_region[sn]->getSize() > 1) ){
			this->visit(sn, (distance+1), node);
		}
	}
	//cout << "---" << endl;

	return;
}

void RegionAdjacencyGraph::measureDistanceFromRoot( int root, int background ){
	this->visit(root,0,background);
	return;
}

IntList * RegionAdjacencyGraph::getSequenceL(int root, int background){
	// assumes that root indicates a valid root region
	IntList * result = IntList::newL();

	//cout << "root has " << _edges[root]->getSize() << " connections." << endl;
	//result->appendL( _edges[root]->getSize() - 1 );
	for(_edges[root]->reset();!(_edges[root]->isNull());_edges[root]->fwd()){
		int branch = _edges[root]->getData();
		if( (branch!=background)&&(_region[branch]->getSize()>1) ){
			result->appendL( _edges[branch]->getSize() - 1 );
			//cout << " branch size:" << (_edges[branch]->getSize() - 1) << endl;
		}
	}

	return result;
}

IntList * RegionAdjacencyGraph::getSequenceL(int root, int background, IntList * map){
	// assumes that root indicates a valid root region
	IntList * result = IntList::newL();

	//result->appendL( _edges[root]->getSize() - 1 );
	for(_edges[root]->reset();!(_edges[root]->isNull());_edges[root]->fwd()){
		int branch = _edges[root]->getData();
		if( (branch!=background)&&(_region[branch]->getSize()>1) ){
			result->appendL( _edges[branch]->getSize() - 1 );
			map->appendL( branch );
		}
	}

	return result;
}
#endif //ndef __SYMBIAN32__

/**
 * this methods assumes that the graph contains only one candidate fiducial
 * */
#ifndef __SYMBIAN32__
FiducialData RegionAdjacencyGraph::analyseFiducialL( bool whiteBackground,
													IntList * badRegions )
{
	markAllLeaves();

	DTPoint origin(0,0);

	// look for the region with (minX, minY) = (0,0)
	// and assume that it is the background
	for(int n=0; n<_used; n++){
		// the second condition should not be needed,
		// but just to be on the safe side..
		if( ( _region[n]->_white == whiteBackground ) && (_region[n]->getSize() > 1) ){
			DTPoint min, max;
			box( _region[n], min, max, _width, _height );
			if( min == origin ){
				_background = n;
				break;
			}
		}
	}

	cout << "background: " << _background << ", degree: " << _edges[_background]->getSize() << ", " << BOOL2COLOR(_region[_background]->_white) << endl;

	int root = -1;
	// look among regions connected to the background
	// assume that the first region that is not a leaf is the root
	// leaves could be the results of image degradation
	for( _edges[_background]->reset(); !_edges[_background]->nullTest(); _edges[_background]->fwd() ){
		int sn = _edges[_background]->getData();
		if( (_edges[sn]->getSize() > 1) && (_region[sn]->getSize() > 1) ){
			root = sn;
			break;
		}
	}

	// if all the regions found in the previous loop
	// root was not assigned
	if( root == -1 ){
		// root dissolved from blurring! :-o
		cout << "root dissolved from blurring" << endl << endl;
		FiducialData result;
		int zero = 0;
		result.init( &zero );
		return result;
	}
	cout << "root: " << root << ", degree: " << _edges[root]->getSize() << BOOL2COLOR(_region[root]->_white) << endl;

	IntList * branches = IntList::newL();
	IntList * seqList = IntList::newL();

	for( _edges[root]->reset(); !_edges[root]->nullTest(); _edges[root]->fwd() ){
		int sn = _edges[root]->getData();
		// subnodes different than the background are branches
		if( (_region[sn]->getSize() > 1) && (sn != _background) ){
			// this is a branch
			branches->appendL( sn );

			// store the number of leaves that this branch has
			int noLeaves = _region[sn]->_noLeaves;
			seqList->appendL( noLeaves );

			// if the degree of sn is bigger than noLeaves+1
			// it means that some subnodes are not leaves
			if( _edges[sn]->getSize() > (noLeaves+1) ){
				// identify non-leaves and store them
				for( _edges[sn]->reset(); !_edges[sn]->nullTest(); _edges[sn]->fwd() ){
					int ssn = _edges[sn]->getData();
					if( (_region[ssn]->getSize() > 0) && ( ssn!=root ) && (_edges[ssn]->getSize() > 1) ){
						badRegions->appendL( ssn );
					}
				}
			}
		}else{
			// else this is an isolated point
			// mark it as a bad region
			badRegions->appendL( sn );
		}
	}

	// sort the sequence list
	sortIntList( seqList );

	// create the fiducial data to be returned

	FiducialData result;
	int * seq = IntList2ArrayL( seqList );
	result.init( seq );
	delete [] seq;

	// populate the branches info
	int i=0;
	for(branches->reset();!(branches->nullTest());branches->fwd()){
		DTPoint branchCentre = getRegion( branches->getData() )->getCentreRound();
		result.setBranchCentre(i++,branchCentre);
	}

	result.setAngle(0);
	result.setType(-1);
	result.setRootRegion(root);
	//TODO: populate width and height

	delete branches;
	delete seqList;

	return result;
}
#endif //__SYMBIAN32__

#ifndef __SYMBIAN32__
/// This method returns true if the root is different
/// false otherwise (branches are different)
bool RegionAdjacencyGraph::compareFiducialL( bool whiteBackground,
											FiducialData target,
											IntList * badRegions ){
	// here we go..

	IntList * tmp = IntList::newL();
	FiducialData candidate = analyseFiducialL( whiteBackground, tmp );
	delete tmp;

	const int * targetSeq = target.getSequence();
	const int * candidateSeq = candidate.getSequence();

	// compare target and candidate number of branches
	// if that's different the problem is in the root
	if( targetSeq[0] != candidateSeq[0] ){
		// append the root to badRegions
		badRegions->appendL(candidate.getRootRegion() );
		return true;
	}

	// TODO: this should be defined relative to the fiducial size
	int distThresh = 20;

	// compare the branches of the two fiducials
	// in terms of number of leaves and position
	int size = targetSeq[0];
	for(int j=0;j<size;j++){
		DTPoint candidateCentre = candidate.getBranchCentre(j);
		bool matched = false;
		for(int i=0;i<size;i++){
			DTPoint targetCentre = target.getBranchCentre(i);
			if( targetSeq[i+1] == candidateSeq[j+1] ){
				if( targetCentre.dist( candidateCentre )<distThresh ){
					matched = true;
				}
			}
		}
		if( !matched ){
			badRegions->appendL( candidate.getBranchIndex(j) );
		}
	}

	delete [] targetSeq;
	delete [] candidateSeq;

	return false;
}
#endif //__SYMBIAN32__


#ifndef __SYMBIAN32__
char * RegionAdjacencyGraph::getStatsString( ){
	int noLeaves = 0;
	int noBLeaves = 0;
	int noWLeaves = 0;
	for(int n=0; n<_used; n++){
		if( _region[n]->getSize() > 0 ){
			if( _edges[n]->getSize() == 1 ){
				noLeaves++;
				// increment the number of leaves of the
				// connected "parent" node
				int p = _edges[n]->getFirstData();
				_region[p]->_noLeaves++;
				if( _region[n]->_white ){
					noWLeaves++;
				}else{
					noBLeaves++;
				}
			}
		}
	}

	std::ostringstream out;

	#ifdef _EC_DEBUG
	out << "number of leaves: " << noLeaves << endl;
	out << "\twhite: " << noWLeaves << endl;
	out << "\tblack: " << noBLeaves << endl;
	#endif

	int noWBranches = 0;
	int noBBranches = 0;
	int noWPureBranches = 0;
	int noBPureBranches = 0;

	// count and store how many black leaves in each white node
	// for+if combination that means: "for each white node n"
	for(int n=0; n<_used; n++){
		if( _region[n]->getSize() > 0 ){
			if( _region[n]->_white ){
				if( _region[n]->_noLeaves > 0 ){
					// this is "kind of" a branch
					noWBranches++;
					if( _edges[n]->getSize() - _region[n]->_noLeaves == 1 ){
						// this is "exactly" a branch
						noWPureBranches++;
					}
				}
			}else{
				if( _region[n]->_noLeaves > 0 ){
					// this is "kind of" a branch
					noBBranches++;
					if( _edges[n]->getSize() - _region[n]->_noLeaves == 1 ){
						// this is "exactly" a branch
						noBPureBranches++;
					}
				}
			}
		}
	}

	#ifdef _EC_DEBUG
	out << "number of branches: " << (noWBranches+noBBranches) << endl;
	out << "\twhite: " << noWBranches << endl;
	out << "\tblack: " << noBBranches << endl;
	out << "number of \"pure\" branches: " << (noWPureBranches+noBPureBranches) << endl;
	out << "\twhite: " << noWPureBranches << endl;
	out << "\tblack: " << noBPureBranches << endl;
	#endif

	_leavesMarked = true;

	char * outString = new char[ out.str().length() ];
	strcpy( outString, out.str().c_str() );

	return outString;
}
#endif //__SYMBIAN32__

#ifndef __SYMBIAN32__
char * RegionAdjacencyGraph::getDotString( bool complete ){
	//markLeaves();
	//ofstream file( filename );
	std::ostringstream file;

	file << "strict graph {" << endl;
	file << "node [ style = filled, color = black, regular = true ];" << endl;
	file << "edge [ color = black ];" << endl;
	for(int i=0;i<_used;i++){
		// if the region is not empty and it is not a leaf
		// (empty regions could be leftovers of merging operations
		// during the graph construction)
		if( (_region[i]->getSize() > 0) && ( complete || _edges[i]->getSize() > 1 ) ){
			file << i << " [ ";
			if( ! complete ){
				file << "label=\"";
				file << _region[i]->_noLeaves << " / ";
				file << _region[i]->_noBranches << " / ";
				file << _edges[i]->getSize() << "\", ";
			}else{
				file << "label=\" \", ";
			}
			if( _region[i]->_white ){
				file << "fillcolor = white, fontcolor = black";
			}else{
				file << "fillcolor = black, fontcolor = white";
			}
			if( ! complete ){
				if( _region[i]->_branch ){
					file << ", color = green";
				}
				// TODO: should the following be _noBranches > 1 or _noBranches >= 1 ???
				if( (_region[i]->_noBranches >= 1) &&
					( (_region[i]->_noBranches + 1) == _edges[i]->getSize() ) ){
					file << ", color = red";
				}
				if( (_region[i]->_noBranches >= 1) &&
					((_region[i]->_noBranches + _region[i]->_noLeaves + 1) == _edges[i]->getSize()) ){
					file << ", color = yellow";
				}
			}
			file << " ];" << endl;
		}
	}

	/*
	file << "{rank=same; " << endl;
	for(int i=0;i<_used;i++){
		if( _edges[i]->getSize() == 1 ){
			file << i << " ";
		}
	}
	file << " }" << endl;
	//*/

	for(int i=0;i<_used;i++){
		if( (_region[i]->getSize() > 0) && ( _edges[i]->getSize() > 1 ) ){
			for( _edges[i]->reset(); !_edges[i]->nullTest(); _edges[i]->fwd() ){
				if( complete || _edges[_edges[i]->getData()]->getSize() > 1 ){
					file << i << " -- " << _edges[i]->getData() << ";" << endl;
				}
			}
		}
	}

	file << "}" << endl;

	char * outString = new char[ file.str().length() + 1 ];
	memset( outString, 0, (file.str().length() + 1)*sizeof(char) );
	strcpy( outString, file.str().c_str() );

	return outString;
}
#endif //__SYMBIAN32__

//
