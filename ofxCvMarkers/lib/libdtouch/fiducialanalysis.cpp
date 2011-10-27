/***************************************************************************
                    fiducialanalysis.cpp  -  description
                             -------------------
    begin                : Thu Mar 1 2007
    copyright            : (C) 2007 by Enrico Costanza @ EPFL
    email                : e.costanza@ieee.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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


#include "fiducialanalysis.h"
#include "types.h"

#include "regionadjacencygraph.h"
#include "ragbuilder.h"
#include "listhash.h"

#include "thresholdfilter.h"

#include <string.h>

#include <math.h>
#include <assert.h>

#include <iostream>

//#define _EC_DEBUG

#ifdef _EC_DEBUG
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;
#endif


void FiducialAnalysis::init( unsigned int in_width, unsigned int in_height ){
	_width = in_width;
	_height = in_height;
	int limit = _width*_height;

	//_thresholdBuffer = new unsigned char[limit];

	int graphSize = 200;

	// variables used for findDifferences
	_anlThresholdBuffer = new unsigned char[limit];
	_anlSceneRAG = RegionAdjacencyGraph::newL( _width, _height, graphSize );

	// variables used for findDifferences
	_cmpThresholdBuffer = new unsigned char[limit];
	_cmpSceneRAG = RegionAdjacencyGraph::newL( _width, _height, graphSize );

	// variables used for findDifferences
	_fdThresholdBuffer1 = new unsigned char[limit];
	_fdThresholdBuffer2 = new unsigned char[limit];
	_fdThresholdBuffer3 = new unsigned char[limit];
	_fdSceneRAG1 = RegionAdjacencyGraph::newL( _width, _height, graphSize );
	_fdSceneRAG2 = RegionAdjacencyGraph::newL( _width, _height, graphSize );
	_fdSceneRAG3 = RegionAdjacencyGraph::newL( _width, _height, graphSize );

	// variables used for getRAG
	_getThresholdBuffer = new unsigned char[limit];
	_getSceneRAG = RegionAdjacencyGraph::newL( _width, _height, graphSize );

	return;
}


FiducialAnalysis::FiducialAnalysis( int in_width, int in_height, int in_param1, int in_param2 )
{
	init( in_width, in_height );
	initThreshold( in_width, in_height, in_param1, in_param2 );
}


void FiducialAnalysis::initThreshold( unsigned int in_width, unsigned int in_height, int in_param1, int in_param2 ){
	_thresholdFilter = BernsenThresholdFilter::newL(in_width,in_height, in_param1, in_param2);
	_thresholdMethod = ThresholdFilter::EBernsen;
	return;
}

void FiducialAnalysis::setThresholdMethod( ThresholdFilter::ThresholdType in_type, int in_param1, int in_param2 ){
	{
		delete _thresholdFilter;
		_thresholdFilter = NULL;
	}
	switch(in_type){
		case ThresholdFilter::EBernsen:{
			if( in_param1 < 0){
				in_param1 = 32;
			}
			if( in_param2 < 0){
				in_param2 = 40;
			}
			_thresholdFilter = BernsenThresholdFilter::newL(_width,_height, in_param1, in_param2 );
			_thresholdMethod = ThresholdFilter::EBernsen;
			break;
		}
		case ThresholdFilter::EBernsenFixed:{
			if( in_param1 < 0){
				in_param1 = 32;
			}
			if( in_param2 < 0){
				in_param2 = 40;
			}
			_thresholdFilter = new BernsenFixedThresholdFilter(_width,_height, in_param1, in_param2 );
			_thresholdMethod = ThresholdFilter::EBernsenFixed;
			break;
		}
		case ThresholdFilter::ESauvola:{
			if( in_param1 < 0){
				in_param1 = 32;
			}
			if( in_param2 < 0){
				in_param2 = 12;
			}
			_thresholdFilter = SauvolaThresholdFilter::newL(_width,_height, in_param1, in_param2 );
			_thresholdMethod = ThresholdFilter::ESauvola;
			break;
		}
		case ThresholdFilter::EOstu:{
			_thresholdFilter = new OstuThresholdFilter(_width,_height);
			_thresholdMethod = ThresholdFilter::EOstu;
			break;
		}
		case ThresholdFilter::EFixed:{
			_thresholdFilter = new FixedThresholdFilter(_width,_height);
			_thresholdMethod = ThresholdFilter::EFixed;
			break;
		}
		default:{
			// error!
		}
	};
}

void FiducialAnalysis::switchThresholdMethod(){
	ThresholdFilter::ThresholdType nextThresholdMethod = (ThresholdFilter::ThresholdType) ((_thresholdMethod + 1) % (ThresholdFilter::EFixed+1));
	setThresholdMethod( nextThresholdMethod, -1, -1 );
	return;
}


FiducialAnalysis::~FiducialAnalysis()
{
	//delete [] _thresholdBuffer;

	// deallocate variables related to getRAG
	delete [] _getThresholdBuffer;
	delete _getSceneRAG;

	// deallocate variables related to findDifferences
	delete [] _fdThresholdBuffer1;
	delete [] _fdThresholdBuffer2;
	delete [] _fdThresholdBuffer3;
	delete _fdSceneRAG1;
	delete _fdSceneRAG2;
	delete _fdSceneRAG3;

	// deallocate variables related to compare
	delete [] _cmpThresholdBuffer;
	delete _cmpSceneRAG;

	// deallocate variables related to analyse
	delete [] _anlThresholdBuffer;
	delete _anlSceneRAG;
}

void transverse( int node, int parent, RegionAdjacencyGraph * sceneRAG, int ** labelsMap, GraphicImage * display )
{
	ListHash<int> ** edges = sceneRAG->getEdges();
	dtouch_Region ** region = sceneRAG->getRegions();

	const int noColors = 10;
	unsigned char colorW[noColors][3] = {
			{0xff,0xd8,0xd8},
			{0xd8,0xe9,0xff},
			{0xd8,0xff,0xe3},
			{0xf4,0xf0,0xab},
			{0xf4,0xce,0xab},
			{0xe3,0xd8,0xff},
			{0xd8,0xff,0xf6},
			{0xc5,0xf4,0xab},
			{0xf4,0xab,0xab},
			{0xf4,0xab,0xdd}
	};
	unsigned char colorB[noColors][3] = {
			{0xcb,0x3c,0x31},
			{0x94,0x92,0x08},
			{0x08,0x94,0x33},
			{0x08,0x4d,0x94},
			{0x94,0x08,0x7b},
			{0x94,0x4a,0x08},
			{0x68,0x94,0x08},
			{0x08,0x92,0x94},
			{0x3d,0x08,0x94},
			{0x94,0x08,0x08}
	};

	for(int c=0;c<noColors;c++){
		for(int cc=0;cc<3;cc++){
			unsigned char d = 10;
			if( colorB[c][cc] > d ){
				colorB[c][cc] -= d;
			}
		}
	}

	static int wc=0;
	static int bc=0;
	for(edges[node]->reset();!(edges[node]->isNull());edges[node]->fwd()){
		int sn = edges[node]->getData();
		if( sn == parent ){
			continue;
		}
		if( region[sn] != NULL ){
			if( region[sn]->getSize() > 0 ){
				if( sceneRAG->getWhite(sn) ){
					display->fill( region[sn], labelsMap, colorW[wc%noColors][0], colorW[wc%noColors][1], colorW[wc%noColors][2] );
					wc++;
				}else{
					display->fill( region[sn], labelsMap, colorB[bc%noColors][0], colorB[bc%noColors][1], colorB[bc%noColors][2] );
					bc++;
				}
			}
		}
		transverse(sn,node,sceneRAG,labelsMap,display);

	}

	return;
}

// WARNING: this method ASSUMES image is in grayscale format
int FiducialAnalysis::analyse( unsigned char *img, GraphicImage *display,
						  PointListPtrList * badRegions, IntList * sequence, bool& whiteRoot, PointList * rootRegion )
{
	#ifdef _EC_DEBUG
	cout << "FiducialAnalysis::analyse called" << endl;
	cout << "w: " << _width << ", h: " << _height << endl;
	#endif //_EC_DEBUG

	//unsigned char *thresholdBuffer;
	//thresholdBuffer = new unsigned char[_width*_height];
	_thresholdFilter->setThreshold( img, _anlThresholdBuffer );
	//memcpy( _thresholdBuffer, thresholdBuffer, _width*_height );

	PointListPtrList * result = badRegions;//new PointListPtrList();

	// build RAG
	//RegionAdjacencyGraph * sceneRAG = RegionAdjacencyGraph::newL( _width, _height, graphSize );
	this->_anlSceneRAG->reset();
	int built = RAGBuilder::buildRAGFullBorderL( img, _anlThresholdBuffer, this->_anlSceneRAG );

	while( built != 0 ){
		int size = this->_anlSceneRAG->getSize();
		delete this->_anlSceneRAG;
		this->_anlSceneRAG = RegionAdjacencyGraph::newL(this->_width,this->_height,2*size);
		built = RAGBuilder::buildRAGFullBorderL( img, _anlThresholdBuffer, this->_anlSceneRAG );
	}

	// update the graph size
	// as the graph might have been expanded in buildRAG
	//graphSize = this->_anlSceneRAG->getSize();

	int noRegions = this->_anlSceneRAG->getNoRegions();

	int root, background;
	// find root
	findRoot( this->_anlSceneRAG, img, root, background );

	if( background>=0 ){
		transverse(background,-1,this->_anlSceneRAG, this->_anlSceneRAG->getLabelsMap(),display);
	}else if( root>= 0 ){
		transverse(root,-1,this->_anlSceneRAG, this->_anlSceneRAG->getLabelsMap(),display);
	}else{
		// waring: I fear that the fowlling may actually fail in some rare occasions
		transverse(0,-1,this->_anlSceneRAG, this->_anlSceneRAG->getLabelsMap(),display);
		if( this->_anlSceneRAG->getRegion(0) != NULL ){
			if( this->_anlSceneRAG->getRegion(0)->getSize() > 0 ){
				if( this->_anlSceneRAG->getWhite(0) ){
					display->fill( this->_anlSceneRAG->getRegion(0), this->_anlSceneRAG->getLabelsMap(), 255, 255, 255 );
				}else{
					display->fill( this->_anlSceneRAG->getRegion(0), this->_anlSceneRAG->getLabelsMap(), 0, 0, 0 );
				}
			}
		}
	}

	if( background>= 0 ){
		if( this->_anlSceneRAG->getRegion(background) != NULL ){
			if( this->_anlSceneRAG->getRegion(background)->getSize() > 0 ){
				if( this->_anlSceneRAG->getWhite(background) ){
					display->fill( this->_anlSceneRAG->getRegion(background), this->_anlSceneRAG->getLabelsMap(), 255, 255, 255 );
				}else{
					display->fill( this->_anlSceneRAG->getRegion(background), this->_anlSceneRAG->getLabelsMap(), 0, 0, 0 );
				}
			}
		}
	}
	if( root>= 0 ){
		if( this->_anlSceneRAG->getRegion(root) != NULL ){
			if( this->_anlSceneRAG->getRegion(root)->getSize() > 0 ){
				if( this->_anlSceneRAG->getWhite(root) ){
					display->fill( this->_anlSceneRAG->getRegion(root), this->_anlSceneRAG->getLabelsMap(), 255, 255, 255 );
				}else{
					display->fill( this->_anlSceneRAG->getRegion(root), this->_anlSceneRAG->getLabelsMap(), 0, 0, 0 );
				}
			}
		}
	}

	#ifdef _EC_DEBUG
	cout << "root: " << root << endl;
	cout << "background: " << background << endl << endl;
	#endif //_EC_DEBUG
	if( root < 0 ){
		// TODO: find the regions that cause problems
		// and add them to badRegions

		return -1;
	}

	if( rootRegion != NULL ){
		rootRegion->append( this->_anlSceneRAG->getRegion(root)->getPointList( this->_anlSceneRAG->getLabelsMap(), _width, _height ) );
	}

	#define _NO_DISPLAY
	#ifndef _NO_DISPLAY
	using namespace std;
	//cout << "no regions: " << noRegions << endl;
	//cout << "calling fill on " << root << endl;
	display->fill( this->_anlSceneRAG->getRegion(root), this->_anlSceneRAG->getLabelsMap(), 0, 255, 255 );
	//cout << "calling fill on " << background << endl;
	display->fill( this->_anlSceneRAG->getRegion(background), this->_anlSceneRAG->getLabelsMap(), 255, 0, 255 );

	for(edges[root]->reset();!(edges[root]->isNull());edges[root]->fwd()){
		int branch = edges[root]->getData();
		if( branch != background ){
			//cout << "calling fill on " << branch << endl;
			display->fill( region[branch], this->_anlSceneRAG->getLabelsMap(), 0, 0, 255 );
		}
	}
	#endif//ndef _NO_DISPLAY

	// mark regions that are too far from root
	this->_anlSceneRAG->measureDistanceFromRoot(root, background);

	// scan all the regions
	for(int i=0;i<noRegions;i++){
		dtouch_Region * current = this->_anlSceneRAG->getRegion(i);
		//cout << i << " size: " << _region[i]->getSize() << ", ";
		//cout << "curr size: " << current->getSize() << endl;
		// if a region is too far from the root
		// append it to the results
		if( (current->getSize()>1) && (this->_anlSceneRAG->getDistance(i) > 1) ){
			// append region i to results
			if( this->_anlSceneRAG->getDistance(i) % 2 ){
				//result->append( *(current->getPointList( _this->_anlSceneRAG->getLabelsMap(), _width, _height )) );
				PointList * tmp = current->getPointList( this->_anlSceneRAG->getLabelsMap(), _width, _height );
				//cout << "PointList size: " << tmp->getSize() << endl;
				result->appendL( tmp );
				//display->fill( region[i], this->_anlSceneRAG->getLabelsMap(), 0, 255, 0 );
			}else{
				//display->fill( region[i], this->_anlSceneRAG->getLabelsMap(), 0, 0, 255 );
			}
		}
		if( current->getSize() == 1 ){
			// same if it is just one-pixel big
			result->appendL( current->getPointList(this->_anlSceneRAG->getLabelsMap(), _width, _height ) );
			//display->fill( region[i], this->_anlSceneRAG->getLabelsMap(), 255, 0, 0 );
		}
	}

	// calculate the sequence
	// this ignores possible problems and
	// calculates the sequence as if the problems were fixed
	if( sequence != NULL ){
		if( sequence->isEmpty() ){
			IntList * tmpSequence = this->_anlSceneRAG->getSequenceL(root,background);
			for(tmpSequence->reset();!(tmpSequence->isNull());tmpSequence->fwd()){
				sequence->appendL( tmpSequence->getData() );
			}
			delete tmpSequence;
			whiteRoot = false;
			if( this->_anlSceneRAG->getRegion(root)->_white ){
				whiteRoot = true;
			}
		}
	}

	return 0;
}

int getNoEdges( int n, dtouch_Region ** regions, ListHash<int> ** edges ){
	int result = 0;
	//cout << "getNoEdges( " << n << " )" << endl;
	for(edges[n]->reset();!(edges[n]->isNull());edges[n]->fwd()){
		int sn = edges[n]->getData();
		//cout << "  sn: " << sn << ", size: " << regions[sn]->getSize() << ", onBorder: ";
		//if( regions[sn]->isOnBorder() ){cout << "true" << endl;}else{cout << "false" << endl;}
		if( (regions[sn]->getSize() > 1) && ( !(regions[sn]->isOnBorder()) ) ){
			result++;
		}
	}

	return result;
}

void FiducialAnalysis::findRoot( RegionAdjacencyGraph * rag, unsigned char * img,
														int& root, int &background )
{
	dtouch_Region ** regions = rag->getRegions();
	ListHash<int> ** edges = rag->getEdges();
	int noRegions = rag->getNoRegions();
	rag->markAllBranches();

	root = -1;
	background = -1;

	// because the image was padded, assume the region on the border is the background
	// because of merging, the bacground/border region could not be the fist one
	for(int i=0;i<noRegions;i++){
		#ifdef _EC_DEBUG2
		cout << "r " << std::setw(2) << i << std::setw(0);
		if( regions[i]->isOnBorder() ){
			cout << " on b.";
		}else{
			cout << "      ";
		}
		if( regions[i]->getSize() > 1 ){
			cout << " z";
		}else{
			cout << "  ";
		}
		cout << " " << edges[i]->getSize() << " edges" << endl;
		#endif //_EC_DEBUG2
		if( (regions[i]->isOnBorder()) && (regions[i]->getSize() > 1) ){
			background = i;
			//break;
		}
	}

	#ifdef _EC_DEBUG
	cout << "background: " << background << endl;
	#endif //_EC_DEBUG

	int rootCandidate = -1;
	// if background is connected to more than one
	// multiply-connected region
	// assume that the background is actually the root
	{
		int counter = 0;
		for(edges[background]->reset();!(edges[background]->isNull());edges[background]->fwd()){
			int sn = edges[background]->getData();
			#ifdef _EC_DEBUG
			cout << sn << "(" << edges[sn]->getSize() << "), ";
			#endif //_EC_DEBUG
			if( edges[sn]->getSize() > 1 ){
				counter++;
				rootCandidate = sn;
			}
		}
		if(counter>1){
			#ifdef _EC_DEBUG
			cout << endl << "the root is the background.." << endl;
			#endif //_EC_DEBUG
			root = background;
			background = -1;
			return;
		}
		if(counter<1){
			#ifdef _EC_DEBUG
			cout << endl << "empty.." << endl;
			#endif //_EC_DEBUG
			root = -1;
			background = -1;
			return;
		}
	}

	#ifdef _EC_DEBUG
	cout << endl;
	#endif //_EC_DEBUG

	root = background;
	// at this point, there is (only) one
	// region connected to background that is not singly connected
	// we need to transverse the tree and look for a sub-region that
	// contains more than one non-leaf (and it is different from the background)
    // shall I change this into "that contains one or more branches?"
	{
		int counter = 0;
        int prevBackground = -1;
        int prevRoot = -1;
		while( counter < 2 ){
            prevBackground = background;
            prevRoot = root;

			background = root;
			root = rootCandidate;
			rootCandidate = -1;
			#ifdef _EC_DEBUG
			cout << "root: " << root << ", background: " << background << endl;
			#endif //_EC_DEBUG
			counter = 0;
            // for each subnode of root sn
			for(edges[root]->reset();!(edges[root]->isNull());edges[root]->fwd()){
				int sn = edges[root]->getData();
				if( sn==background ){
					continue;
				}
				#ifdef _EC_DEBUG
				cout << sn << "(" << edges[sn]->getSize() << "), ";
				#endif //_EC_DEBUG
                // if sn is not a leaf increment the counter
				if( edges[sn]->getSize() > 1 ){
					counter++;
					rootCandidate = sn;
				}
			}
			if( rootCandidate == -1 ){ // equivalent to counter == 0
				#ifdef _EC_DEBUG
				cout << endl << "cannot find a root candidate" << endl;
				#endif //_EC_DEBUG
				//root = -1;
				//background = -1;
                root = prevRoot;
                background = prevBackground;
				return;
			}
			#ifdef _EC_DEBUG
			cout << endl;
			#endif // _EC_DEBUG
		}
	}

	return;
}

// WARNING: this method ASSUMES image is in grayscale format
bool FiducialAnalysis::compare( unsigned char *img, GraphicImage *display,
		IntList * origSeq )
{
	bool result = false;
	//int graphSize = 4000;

	//unsigned char *thresholdBuffer = new unsigned char[_width*_height];
	_thresholdFilter->setThreshold( img, this->_cmpThresholdBuffer );

	// build RAG
	//RegionAdjacencyGraph * sceneRAG = RegionAdjacencyGraph::newL( _width, _height, graphSize );
	this->_cmpSceneRAG->reset();

	int built = RAGBuilder::buildRAGFullBorderL( img, this->_cmpThresholdBuffer, this->_cmpSceneRAG );

	while( built != 0 ){
		int size = this->_cmpSceneRAG->getSize();
		delete this->_cmpSceneRAG;
		this->_cmpSceneRAG = RegionAdjacencyGraph::newL(this->_width,this->_height,2*size);
		built = RAGBuilder::buildRAGFullBorderL( img, this->_cmpThresholdBuffer, this->_cmpSceneRAG );
	}

	int noRegions = this->_cmpSceneRAG->getNoRegions();

	int root, background;
	// find root
	findRoot( this->_cmpSceneRAG, img, root, background );
	noRegions = this->_cmpSceneRAG->getNoRegions();
	//cout << "noRegions: " << noRegions << endl;
	//cout << "root: " << root << endl;
	//cout << "background: " << background << endl;
	if( root < 0 ){
		result = false;
	}else{
		//display->fill( sceneRAG->getRegion(root), sceneRAG->getLabelsMap(), 0, 255, 255 );
		//display->fill( sceneRAG->getRegion(background), sceneRAG->getLabelsMap(), 255, 0, 255 );

		// mark regions that are too far from root
		this->_cmpSceneRAG->measureDistanceFromRoot(root, background);

		// calculate the sequence
		// this ignores possible problems and
		// calculates the sequence as if the problems were fixed
		IntList * sequence = this->_cmpSceneRAG->getSequenceL(root,background);

		/*
		cout << "sequence: ";
		for(sequence->reset();!(sequence->isNull());sequence->fwd()){
			cout << sequence->getData() << " ";
		}
		cout << endl;

		cout << "    orig: ";
		for(origSeq->reset();!(origSeq->isNull());origSeq->fwd()){
			cout << origSeq->getData() << " ";
		}
		cout << endl;
		//*/

		result = compareIntList(origSeq,sequence);

		IntList * tmpSequence = origSeq;
		sortIntList(tmpSequence);

		#ifdef _EC_DEBUG
		cout << "original  sequence: ";
		for(tmpSequence->reset();!(tmpSequence->isNull());tmpSequence->fwd()){
			cout << tmpSequence->getData() << ",";
		}
		cout << endl;
		#endif //def _EC_DEBUG

		tmpSequence = sequence;
		sortIntList(tmpSequence);

		#ifdef _EC_DEBUG
		cout << "candidate sequence: ";
		for(tmpSequence->reset();!(tmpSequence->isNull());tmpSequence->fwd()){
			cout << tmpSequence->getData() << ",";
		}
		cout << endl;
		#endif //def _EC_DEBUG

		delete sequence;
	}

	return result;
}


//--------------------------------------
int countNeighbours(PointList * r,int ** labelsMap, dtouch_Region ** region, int& noWhite, int& noBlack, int _width,int _height){
	noWhite=0;
	noBlack=0;
	//if( r->getSize() == 0 ){return 0;}
	if( r->getSize() <= 1 ){return 0;}
	DTPoint f = r->getFirstData();
	//cout << "x,y: " << f.x << "," << f.y << endl;
	int label = *(labelsMap[f.x+f.y*_width]);
	//cout << "label: " << label << endl;
	if( region[label]->getSize()<=1 ){
		return 0;
	}

	bool white = region[label]->_white;
	IntList * seen = IntList::newL();
	seen->appendL(label);

	if( white ){
		noWhite++;
	}else{
		noBlack++;
	}

	const int dx[] = { 1, 1, 1, 0,-1,-1,-1, 0};
	const int dy[] = {-1, 0, 1, 1, 1, 0,-1,-1};
	for(r->reset();!(r->isNull());r->fwd()){
		DTPoint p = r->getData();
		for(int i=0;i<8;i++){
			DTPoint n(p.x+dx[i],p.y+dy[i]);
			if( n.x < 0 || n.y < 0 || n.x >= _width || n.y >= _height ){
				continue;
			}

			int l = *(labelsMap[n.x+n.y*_width]);

			if( l != label ){
				if( ! seen->contains(l) ){
					if( region[l]->getSize()>1 ){
						seen->appendL(l);
						if( region[l]->_white ){
							noWhite++;
						}else{
							noBlack++;
						}
					}
				}
			}
		}
	}
	int result = seen->getSize() - 1;
	delete seen;
	return result;
}

IntList * getRegionIndexes(IntList * sequenceMap,int root,ListHash<int> ** edges){
	IntList * toVisit = IntList::newL();
	toVisit->appendL(root);
	for(sequenceMap->reset();!(sequenceMap->isNull());sequenceMap->fwd()){
		int currBranchIndex = sequenceMap->getData();
		if( currBranchIndex!=root ){
			toVisit->appendL(currBranchIndex);
			for(edges[currBranchIndex]->reset();!(edges[currBranchIndex]->isNull());edges[currBranchIndex]->fwd()){
				int currentLeaf = edges[currBranchIndex]->getData();
				if(currentLeaf!=currBranchIndex){
					toVisit->appendL(currentLeaf);
				}
			}
		}
	}
	return toVisit;
}

void seekAdjacent(PointList * src, PointList * dest, DTPoint p){
	for(src->reset();!(src->isNull());src->fwd()){
		DTPoint curr = src->getData();
		//cout << "curr: " << curr << ", p: " << p << endl;
		if(curr.is8Neighbour(p)){
			//cout << " is8Neighbour" << endl;
			dest->appendL(curr);
			src->remove(curr);
			seekAdjacent(src,dest,curr);
		}
	}
}

List<PointList *> * extractComponents( PointList * src ){
	PointListPtrList * result = PointListPtrList::newL();

	src->reset();
	ListItem<DTPoint> * first = src->getCurrent();
	ListItem<DTPoint> * ptr = NULL;
	// for each point p
	for(src->reset();!(src->isNull());src->fwd()){
		DTPoint p = src->getData();
		PointList * currList = NULL;
		// if p is part of one existing list
		for(result->reset();!(result->isNull());result->fwd()){
			if( result->getData()->contains(p) ){
				// use that as the current list
				currList = result->getData();
			}
		}
		// else
		// create a new list
		if( currList==NULL ){
			currList = PointList::newL();
			result->appendL(currList);
			currList->appendL(p);
		}
		// for each point q != p
		for(ptr=first;ptr!=NULL;ptr=ptr->getNext()){
			DTPoint q = ptr->getData();
			if( !(p==q) ){
				// if q.isNeighbour(p)
				if( q.is8Neighbour(p) ){
					// append q to the current list
					currList->appendL(q);
				}
			}
		}
	}

	return result;
}
//--------------------------------------

PointListPtrList * FiducialAnalysis::findDifferences(
	unsigned char *img1, unsigned char *img2, GraphicImage *display )
{
	//cout << "FiducialAnalysis::findDifferences called" << endl;
	PointListPtrList * result = PointListPtrList::newL();

	int limit = _width*_height;
	//unsigned char * thresholdBuffer1 = new unsigned char[limit];
	_thresholdFilter->setThreshold( img1, this->_fdThresholdBuffer1 );

	//unsigned char * thresholdBuffer2 = new unsigned char[limit];
	_thresholdFilter->setThreshold( img2, this->_fdThresholdBuffer2 );

	// build RAG for img1
	//RegionAdjacencyGraph * sceneRAG1 = RegionAdjacencyGraph::newL( _width, _height );
	this->_fdSceneRAG1->reset();

	// TODO: check the following!!
	int built = RAGBuilder::buildRAGFullBorderL( img1, this->_fdThresholdBuffer1, this->_fdSceneRAG1 );
	while( built==-1 ){
		int size = this->_fdSceneRAG1->getSize();
		delete this->_fdSceneRAG1;
		this->_fdSceneRAG1 = RegionAdjacencyGraph::newL(this->_width,this->_height,size*2);
		built = RAGBuilder::buildRAGFullBorderL( img1, this->_fdThresholdBuffer1, this->_fdSceneRAG1 );
	}

	// build RAG for img2
	//RegionAdjacencyGraph * sceneRAG2 = RegionAdjacencyGraph::newL( _width, _height );
	this->_fdSceneRAG2->reset();

	built = RAGBuilder::buildRAGFullBorderL( img2, this->_fdThresholdBuffer2, this->_fdSceneRAG2 );
	if( built==-1 ){
		int size = this->_fdSceneRAG2->getSize();
		delete this->_fdSceneRAG2;
		this->_fdSceneRAG2 = RegionAdjacencyGraph::newL(this->_width,this->_height,size*2);
		built = RAGBuilder::buildRAGFullBorderL( img2, this->_fdThresholdBuffer2, this->_fdSceneRAG2 );
	}

	int root1, background1;
	// find root
	findRoot( this->_fdSceneRAG1, img1, root1, background1 );

	//display->fill( region1[root1], labelsMap1, 0, 255, 255 );
	//display->fill( region1[background1], labelsMap1, 255, 0, 255 );

	int root2, background2;
	// find root
	findRoot( this->_fdSceneRAG2, img2, root2, background2 );
	//noRegions2 = sceneRAG2->getNoRegions();

	// calculate difference of thresholded images
	unsigned char * diffImg = new unsigned char[limit];
	unsigned char * ptr = diffImg;
	unsigned char * ip1 = img1;
	unsigned char * tp1 = this->_fdThresholdBuffer1;
	unsigned char * ip2 = img2;
	unsigned char * tp2 = this->_fdThresholdBuffer2;
	for(int i=0;i<limit;i++){
		*ptr++ = (( ( *ip1++ >= *tp1++ ) == ( *ip2++ >= *tp2++ ) )?(255):(0));
	}

	#ifdef _EC_DEBUG
	ip1 = img1;
	tp1 = this->_fdThresholdBuffer1;
	ip2 = img2;
	tp2 = this->_fdThresholdBuffer2;
	FILE * f = fopen( "diffA.raw", "w" );
	for(int i=0;i<limit;i++){
		fputc( ( ( *ip1 >= *tp1 )?(255):(0)), f );
		fputc( ( ( *ip1 >= *tp1 )?(255):(0)), f );
		fputc( ( ( *ip1++ >= *tp1++ )?(255):(0)), f );
	}
	fclose(f);

	f = fopen( "diffB.raw", "w" );
	for(int i=0;i<limit;i++){
		fputc( ( ( *ip2 >= *tp2 )?(255):(0)), f );
		fputc( ( ( *ip2 >= *tp2 )?(255):(0)), f );
		fputc( ( ( *ip2++ >= *tp2++ )?(255):(0)), f );
	}
	fclose(f);

	f = fopen( "diff.raw", "w" );
	for(int i=0;i<limit;i++){
		fputc( diffImg[i], f );
		fputc( diffImg[i], f );
		fputc( diffImg[i], f );
	}
	fclose(f);
	#endif //def _EC_DEBUG

	//unsigned char * thresholdBuffer3 = new unsigned char[limit];
	//_thresholdFilter->setThreshold( diffImg, thresholdBuffer3 );
	memset(this->_fdThresholdBuffer3,127,limit*sizeof(unsigned char));

	// build RAG for the diff image
	//RegionAdjacencyGraph * sceneRAG3 = RegionAdjacencyGraph::newL( _width, _height );
	this->_fdSceneRAG3->reset();
	built = RAGBuilder::buildRAGFullBorderL( diffImg, this->_fdThresholdBuffer3, this->_fdSceneRAG3 );
	if( built==-1 ){
		int size = this->_fdSceneRAG3->getSize();
		delete this->_fdSceneRAG3;
		this->_fdSceneRAG3 = RegionAdjacencyGraph::newL(this->_width,this->_height,size*2);
		built = RAGBuilder::buildRAGFullBorderL( diffImg, this->_fdThresholdBuffer3, this->_fdSceneRAG3 );
	}

	int noRegions3 = this->_fdSceneRAG3->getNoRegions();

	// now we have the connected components of the difference image
	for(int r=0;r<noRegions3;r++){
		if( this->_fdSceneRAG3->getRegion(r)->getSize()>0 ){
			if( !this->_fdSceneRAG3->getRegion(r)->_white  ){
				PointList * currComponent = this->_fdSceneRAG3->getRegion(r)->getPointList(this->_fdSceneRAG3->getLabelsMap(),_width,_height);
				int nw1,nb1,nw2,nb2;
				countNeighbours(currComponent,this->_fdSceneRAG2->getLabelsMap(),this->_fdSceneRAG2->getRegions(),nw2,nb2,_width,_height);

				// there is an issue here: because in the difference image
				// all regions of interest are black, 8-connectivity is used.
				// However, in the original image, the regions could be white,
				// so 4-connectivity is used. Because of this conflict the first
				// point of the region (in the diff image) could actually be an isolated
				// pixel.  Here we check if this is the case, and if it is we remove it
				DTPoint f = currComponent->getFirstData();
				int fl = *(this->_fdSceneRAG1->getLabelsMap()[f.x+f.y*_width]);
				if( this->_fdSceneRAG1->getRegion(fl)->getSize() <= 1 ){
					currComponent->remove( f );
				}

				countNeighbours(currComponent,this->_fdSceneRAG1->getLabelsMap(),this->_fdSceneRAG1->getRegions(),nw1,nb1,_width,_height);
				//DTPoint first = region3[r]->getFirstData();
				// if there is a mismatch
				//if( ((nw1!=nw2) || (nb1!=nb2)) && (nb1+nw1)>0 ){
				if( ((nw1!=nw2) || (nb1!=nb2)) ){ //&& (nb1+nw1)>0 ){
					// add this component to the results
					result->appendL(currComponent);
				}else{
					delete currComponent;
				}
			}
		}
	}

	delete [] diffImg;

	// deallocate all
//	delete [] thresholdBuffer1;
//	delete [] thresholdBuffer2;
//	delete [] thresholdBuffer3;
//
//	delete sceneRAG1;
//	delete sceneRAG2;
//	delete sceneRAG3;

	return result;
}

/*
bool FiducialAnalysis::breaksUniformly(
	unsigned char *img1, unsigned char *img2, GraphicImage *display )
{
	//PointListPtrList * result = new PointListPtrList();
	bool result = false;

	int limit = _width*_height;
	unsigned char * thresholdBuffer = new unsigned char[limit];
	_thresholdFilter->setThreshold( img1, thresholdBuffer );

	// build RAG for img1
	RegionAdjacencyGraph * sceneRAG1 = RegionAdjacencyGraph::newL( _width, _height );
	RAGBuilder::buildRAGFullBorderL( img1, thresholdBuffer, sceneRAG1 );

	// build RAG for img2
	RegionAdjacencyGraph * sceneRAG2 = RegionAdjacencyGraph::newL( _width, _height );
	RAGBuilder::buildRAGFullBorderL( img2, thresholdBuffer, sceneRAG2 );

	int root1, background1;
	// find root
	findRoot( sceneRAG1, img1, root1, background1 );

	if( root1 >= 0 ){
		//display->fill( region1[root1], sceneRAG1->getLabelsMap(), 0, 255, 255 );
		//display->fill( region1[background1], sceneRAG1->getLabelsMap(), 255, 0, 255 );
		// mark regions that are too far from root
		sceneRAG1->measureDistanceFromRoot(root1, background1);

		IntList * sequence1Map = IntList::newL();
		IntList * sequence1 = IntList::newL();
		// calculate the sequence
		// this ignores possible problems and
		// calculates the sequence as if the problems were fixed
		IntList * tmpSequence = sceneRAG1->getSequenceL(root1,background1,sequence1Map);
		for(tmpSequence->reset();!(tmpSequence->isNull());tmpSequence->fwd()){
			sequence1->appendL( tmpSequence->getData() );
		}
		delete tmpSequence;

		bool leavesBroken = true;

		// see if there are any leaves left
		// for each branch in image 1
		for(sequence1->reset(),sequence1Map->reset();!(sequence1->isNull());sequence1->fwd(),sequence1Map->fwd()){
			//int currBranch = sequence1->getData();
			int currBranchIndex = sequence1Map->getData();

			// look for the region that is the closest to
			// the current branch of image 1
			int maxSize = 0;
			int img2BranchIndex = 0;
			findMatch(maxSize,img2BranchIndex,currBranchIndex,sceneRAG1->getRegions(),sceneRAG2->getRegions(),sceneRAG1->getLabelsMap(),sceneRAG2->getLabelsMap(),sceneRAG2->getNoRegions());

			// for each leaf in the current branch
			ListHash<int> * currBranchEdges = sceneRAG1->getEdges(currBranchIndex);
			for(currBranchEdges->reset();!(currBranchEdges->isNull());currBranchEdges->fwd()){
				int currLeafIndex = currBranchEdges->getData();
				// see if the current leaf exists in img2
				if( currLeafIndex != root1 ){
					PointList * tmp = sceneRAG1->getRegion(currLeafIndex)->getPointList(sceneRAG1->getLabelsMap(),_width,_height);
					// and look for correspondaces in img2
					// calculate the intersection between the original leaf and the
					// blurred branch
					PointList * inter = intersect(sceneRAG1->getRegion(currLeafIndex),sceneRAG2->getRegion(img2BranchIndex),sceneRAG1->getLabelsMap(),sceneRAG2->getLabelsMap(),_width,_height);

					// if the intersection of the the original leaf and the blurred branch
					// is "maximal" (i.e. it has the same size as the original leaf)
					// it means that there is no blurred leaf corresponding to this one
					if( inter->getSize() != tmp->getSize() ){
						leavesBroken = false;
					}
					delete inter;
					delete tmp;
				}
			}
		}

		if( leavesBroken==true ){
			int root2, background2;
			// find root
			//bool rootDifference = false;
			//cout << "calling findRoot( &sceneRAG2, img2, root2, background2 )" << endl;
			findRoot( sceneRAG2, img2, root2, background2 );
			//noRegions2 = sceneRAG2->getNoRegions();

			if( root2<0 ){
				result = true;
			}else{
				// mark regions that are too far from root
				sceneRAG2->measureDistanceFromRoot(root2, background2);

				// calculate the sequence
				// this ignores possible problems and
				// calculates the sequence as if the problems were fixed
				IntList * sequence2 = sceneRAG2->getSequenceL(root2,background2);

				if( sequence1->getSize() == sequence2->getSize() ){
					result = false;
				}else{
					result = true;
				}

				delete sequence2;
			}
		}else{
			result = false;
		}

		delete sequence1;
		delete sequence1Map;
	}

	// deallocate all
	delete [] thresholdBuffer;

	delete sceneRAG1;
	delete sceneRAG2;

	return result;
}
*/

void FiducialAnalysis::findMatch(int& maxSize, int&maxIndex, const int target1, dtouch_Region ** region1, dtouch_Region ** region2,
			   int ** labelsMap1, int ** labelsMap2, const int noRegions2,bool colorAware){
	// for each region r in img2
	for(int r=0;r<noRegions2;r++){
		// calculate the size of the intersection between r and root1
		// and find the max
		if(colorAware){
			if( region1[target1]->_white != region2[r]->_white ){
				continue;
			}
		}
		//cout << "r: " << r << ", ";
		PointList * intersection = intersect(region1[target1],region2[r],labelsMap1,labelsMap2,_width,_height);
		//cout << intersection->getSize();
		if( maxSize < intersection->getSize() ){
			maxSize = intersection->getSize();
			maxIndex = r;
		}
		delete intersection;
		//cout << endl;
	}
	//cout << "match: " << maxIndex << endl;
}

// WARNING: this method ASSUMES image is in grayscale format
DTPoint FiducialAnalysis::getRootSize( unsigned char *img )
{
	unsigned char *thresholdBuffer;

	thresholdBuffer = new unsigned char[_width*_height];
	_thresholdFilter->setThreshold( img, thresholdBuffer );

	// build RAG
	RegionAdjacencyGraph * sceneRAG = RegionAdjacencyGraph::newL( _width, _height );

	int built = RAGBuilder::buildRAGFullBorderL( img, thresholdBuffer, sceneRAG );
	while( built==-1 ){
		int size = sceneRAG->getSize();
		delete sceneRAG;
		sceneRAG = RegionAdjacencyGraph::newL( _width, _height, size*2 );
		built = RAGBuilder::buildRAGFullBorderL( img, thresholdBuffer, sceneRAG );
	}

	int root, background;
	// find root
	findRoot( sceneRAG, img, root, background );

	DTPoint result = DTPoint(0,0);
	if( root >= 0 ){
		DTPoint min, max;
		sceneRAG->getRegion(root)->box(min,max,_width,_height);

		result = max - min;
	}

	delete sceneRAG;

	return result;
}

// WARNING: this method ASSUMES image is in grayscale format
int FiducialAnalysis::getRAG( unsigned char *img, GraphicImage *display, IntList *** rag, int& noRegions )
{
	#ifdef _EC_DEBUG
	cout << "FiducialAnalysis::analyse called" << endl;
	cout << "w: " << _width << ", h: " << _height << endl;
	#endif //_EC_DEBUG

	_thresholdFilter->setThreshold( img, _getThresholdBuffer );

	// build RAG
	this->_getSceneRAG->reset();
	int built = RAGBuilder::buildRAGFullBorderL( img, _getThresholdBuffer, this->_getSceneRAG );

	while( built != 0 ){
		int size = this->_getSceneRAG->getSize();
		delete this->_getSceneRAG;
		this->_getSceneRAG = RegionAdjacencyGraph::newL( _width, _height, size*2 );
		built = RAGBuilder::buildRAGFullBorderL( img, this->_getThresholdBuffer, this->_getSceneRAG );
	}

	noRegions = this->_getSceneRAG->getNoRegions();
	*rag = new IntList*[noRegions];

	for(int i=0;i<noRegions;i++){
		(*rag)[i] = IntList::newL();
		ListHash<int> * curr = _getSceneRAG->getEdges(i);
		for(curr->reset();!(curr->isNull());curr->fwd()){
			int edge = curr->getData();
			(*rag)[i]->appendL( edge );
		}
	}

	return 0;
}


// end of file
