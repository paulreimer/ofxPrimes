/***************************************************************************
                             -------------------
                    fiducialrecognition.cpp  -  description
    begin                : Tue Aug 27 2002
    copyright            : (C) 2002 by Enrico Costanza
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


/*  Changes

	Code optimization by Jorge M Santiago
	Small amount of further optimisation by Justen Hyde

*/

#include "fiducialrecognition.h"
#include "types.h"

#include "regionadjacencygraph.h"
#include "ragbuilder.h"
#include "listhash.h"

#include "thresholdfilter.h"

#ifdef __SYMBIAN32__
#include <string.h>
#include <e32base.h>
#endif

#include <math.h>
#include <assert.h>

#ifndef __SYMBIAN32__
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

#ifdef _EC_DEBUG
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;
using std::ios;
#endif



void FiducialRecognition::initL()
{
	_fiducialSequence = List<int*>::newL();
	_geometricDecoding = List<bool>::newL();

	if( (_width % 2 != 0) || (_height % 2 != 0) ){
		#ifndef __SYMBIAN32__
		cout << "width or height are not multiple of 2!!" << endl;
		#endif // __SYMBIAN32__
		assert( false );
		// TODO: throw exception? return?
	}

	// init thresholding buffer (used by any thresholding algorithm)
	#ifndef __SYMBIAN32__
	_thresholdBuffer = new unsigned char[_width*_height];
	#else
	_thresholdBuffer = new (ELeave) unsigned char[_width*_height];
	#endif
	memset(_thresholdBuffer, 127, _width*_height*sizeof(unsigned char));

	_ragSize = 1000;
	_sceneRAG = RegionAdjacencyGraph::newL( _width, _height, _ragSize );
	//_sceneRAG = RegionAdjacencyGraph::newL( _width, _height );

	return;
}

FiducialRecognition * FiducialRecognition::newL( const unsigned int &in_width, const unsigned int &in_height,
										const int &in_param1, const int &in_param2 ){
	FiducialRecognition * self = new FiducialRecognition( in_width, in_height );
	self->constructL(in_param1,in_param2);
	return self;
}


FiducialRecognition::FiducialRecognition( const unsigned int &in_width, const unsigned int &in_height ) :
			_width(in_width),
			_height(in_height)
{ }

void FiducialRecognition::constructL( const int &in_param1, const int &in_param2 ){
	initL();
	initThresholdL( _width, _height, in_param1, in_param2 );
}

void FiducialRecognition::initThresholdL( unsigned int in_width, unsigned int in_height, int in_param1, int in_param2 )
{
	_thresholdFilter = BernsenThresholdFilter::newL(in_width,in_height, in_param1, in_param2);
	_thresholdMethod = ThresholdFilter::EBernsen;
	return;
}

void FiducialRecognition::setThresholdMethodL( ThresholdFilter::ThresholdType in_type, int in_param1, int in_param2 ){
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
			_thresholdFilter = BernsenFixedThresholdFilter::newL(_width,_height, in_param1, in_param2 );
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

void FiducialRecognition::switchThresholdMethodL(){
	ThresholdFilter::ThresholdType nextThresholdMethod = (ThresholdFilter::ThresholdType) ((_thresholdMethod + 1) % (ThresholdFilter::EFixed+1));
	setThresholdMethodL( nextThresholdMethod, -1, -1 );
	return;
}

void FiducialRecognition::addSequenceL( const int *in_fiducialSequence, bool in_geometricDecoding )
{
	if( in_fiducialSequence==NULL ){
		return;
	}
	#ifndef __SYMBIAN32__
	int * toAppend = new int[in_fiducialSequence[0]+1];
	#else
	int * toAppend = new (ELeave) int[in_fiducialSequence[0]+1];
	#endif
	memcpy( toAppend, in_fiducialSequence, (in_fiducialSequence[0]+1)*sizeof(int) );

	_fiducialSequence->appendL(toAppend);
	_geometricDecoding->appendL(in_geometricDecoding);

	#ifndef __SYMBIAN32__
	for(int i=0;i<=toAppend[0];i++){
		cout << toAppend[i] << ":";
	}
	cout << endl;
	#endif//ndef __SYMBIAN32__

	return;
}

void FiducialRecognition::addSequenceL( const char * in_fiducialSequence, bool in_geometricDecoding )
{
	bool white;
	int * tmp = FiducialData::parseSequenceL( in_fiducialSequence, white );
	CLN_PUSH(tmp);

	// TODO: accept white sequences

	addSequenceL( tmp, in_geometricDecoding );
	CLN_POP(tmp);
	delete [] tmp;

	return;
}


FiducialRecognition::~FiducialRecognition()
{
	delete _sceneRAG;
	_sceneRAG = NULL;

	delete _thresholdFilter;
	_thresholdFilter = NULL;

	// cleanup threshold buffer (used for any thresholding algorithm)
	delete [] _thresholdBuffer;
	_thresholdBuffer = NULL;

	// start cleanup fiducial sequences
	for(_fiducialSequence->reset();!(_fiducialSequence->nullTest());_fiducialSequence->fwd()){
		delete [] _fiducialSequence->getData();
	}
	delete _fiducialSequence;
	delete _geometricDecoding;
	// end cleanup fiducial sequences

	// start cleanup memory related to the graph
	// TODO: move this to the graph class
	// end cleanup memory related to the graph
}

/*
// WARNING: process ASSUMES image is in grayscale format
FiducialDataList * FiducialRecognition::process( unsigned char *img ){
	FiducialDataList * fiducialData = new FiducialDataList();

	setThreshold( img );

	RegionAdjacencyGraph sceneRAG( _width, _height, _graphSize, _regionsPool,
			_edges, _region, _labelsMap, _labels );

	RAGBuilder ragBuilder( img, _width, _height,
						_thresholdBuffer, &sceneRAG );

	//
	//int ragBuilt = ragBuilder.buildRAG();
	//int ragBuilt = ragBuilder.buildRAGBorder();
	int ragBuilt = ragBuilder.buildRAGFullBorder(_graphSize/2);

	// update the graph size (stored in MyCameraFramework)
	// as the graph might have been expanded in buildRAG
	_graphSize = sceneRAG.getSize();
	_edges = sceneRAG.getEdges();
	_region = sceneRAG.getRegions();

	if( ragBuilt != 0 ){
		return fiducialData;
	}

	//int noRegions = sceneRAG.getNoRegions();

	#ifdef _EC_DEBUG
	int noFiducials = 0;
	#endif //def _EC_DEBUG

	for(_fiducialSequence->reset(), _geometricDecoding->reset();
			!(_fiducialSequence->nullTest());
				_fiducialSequence->fwd(),_geometricDecoding->fwd()){
		int * currSeq = _fiducialSequence->getData();
		bool currGeometricDecoding = _geometricDecoding->getData();
		FiducialDataList * tmp = sceneRAG.findFiducials( currSeq,
						!currGeometricDecoding );
		fiducialData->append( tmp );

		#ifdef _EC_DEBUG
		int tmpNoFiducials = tmp->getSize();
		noFiducials += tmpNoFiducials;
		#endif //def _EC_DEBUG

		delete tmp;
		tmp = NULL;

		//FiducialData *tmpFiducialData;

		//int tmpNoFiducials = sceneRAG.findFiducials( _fiducialSequence[k],
		//				&tmpFiducialData, _usingLinearFiducials[k] );

		//FiducialData *result = new FiducialData [tmpNoFiducials+noFiducials];
		//memcpy( result, *fiducialData, noFiducials * sizeof(FiducialData) );
		//memcpy( result+noFiducials, tmpFiducialData, tmpNoFiducials * sizeof(FiducialData) );

		//delete [] *fiducialData;
		// *fiducialData = result;
		//delete [] tmpFiducialData;

	}

	return fiducialData;
}
//*/

// process ASSUMES image is in grayscale format
// geometryInfo is not currently used
FiducialDataList * FiducialRecognition::process(
						unsigned char *img,
						GraphicImage *display,
						bool displayThreshold,
						bool geometryInfo )
{
	if( _fiducialSequence->getSize()==0 ){
		return processGen(img,display,displayThreshold,geometryInfo);
	}
	if( img == NULL ){
		return NULL;
	}

	_thresholdFilter->setThreshold( img, _thresholdBuffer );

	if( display ){
		if( displayThreshold ){
			display->markThreshold( img, _thresholdBuffer );
		}
	}

	//RegionAdjacencyGraph * _sceneRAG = RegionAdjacencyGraph::newL( _width, _height, _graphSize, _regionsPool,
	//		_edges, _region, _labelsMap, _labels );
	_sceneRAG->reset();
	//RAGBuilder ragBuilder( img, _width, _height, _thresholdBuffer, _sceneRAG );

	#ifndef __SYMBIAN32__
	int ragBuilt = RAGBuilder::buildRAGFullBorderL( img, _thresholdBuffer, _sceneRAG );
	//cout << "ragBuilt: " << ragBuilt << endl;
	#else
	int err = KErrNone;
	int ragBuilt = 0;
	TRAP(err, ragBuilt = RAGBuilder::buildRAGFullBorderL( img, _thresholdBuffer, _sceneRAG ) );
	// KErrNone should be zero, so a value != 0 will do for the code below
	while(err!=KErrNone){
		ragBuilt=err;
		RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("RAGBuilder::buildRAGFullBorderL left!") );
		// TODO: limit the graph size?
		delete _sceneRAG;
		_sceneRAG=NULL;
		_ragSize -= _ragSize/5;
		TRAP(err, _sceneRAG = RegionAdjacencyGraph::newL( _width, _height, _ragSize ) );
		TRAP(err, ragBuilt = RAGBuilder::buildRAGFullBorderL( img, _thresholdBuffer, _sceneRAG ) );
	}
	#endif

	_noRegions = _sceneRAG->getNoRegions();

	if( ragBuilt != 0 ){
		#ifndef __SYMBIAN32__
		// TODO: remove this after debugging
		if( display ){
			display->fillRect(_width/3,_height/3,_width/3,_height/3);
		}
		#endif //ndef __SYMBIAN32__
		return NULL;
	}

	#ifndef __SYMBIAN32__
	FiducialDataList * fiducialData = FiducialDataList::newL();
	#else
	//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("calling FiducialDataList::newL") );
	FiducialDataList * fiducialData = NULL;
	TRAPD( err1, fiducialData = FiducialDataList::newL() );
	//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("FiducialDataList::newL left") );
	#endif

	int fidSeqIndex=0;
	for(_fiducialSequence->reset(), _geometricDecoding->reset();
			!(_fiducialSequence->nullTest());
				_fiducialSequence->fwd(),_geometricDecoding->fwd()){
		int * currSeq = _fiducialSequence->getData();
		bool currGeometricDecoding = _geometricDecoding->getData();

		#ifndef __SYMBIAN32__
		FiducialDataList * tmp = _sceneRAG->findFiducialsL( currSeq,
						!currGeometricDecoding );
		#else
		FiducialDataList * tmp = NULL;
		TRAPD( err2, tmp = _sceneRAG->findFiducialsL( currSeq,
						!currGeometricDecoding ) );
		if( err2 != KErrNone ){
			delete tmp;
			tmp = NULL;
			return fiducialData;
		}
		#endif

		if( display ){
			for(tmp->reset();!(tmp->nullTest());tmp->fwd()){
				FiducialData curr = tmp->getData();
				display->fill(_sceneRAG->getRegion( curr.getRootRegion() ), _sceneRAG->getLabelsMap(), 0, 255, 0);
			}
		}

		FiducialDataList * ret = FiducialDataList::newL();
		//type is defined as the index in the sequence LUT table
		for(tmp->reset();!(tmp->nullTest());tmp->fwd() ){
			FiducialData curr = tmp->getData();
			curr.setType(fidSeqIndex);
			ret->appendL(curr);
		//tmp->getCurrent()->Data().setType(fidSeqIndex);
		}
		fiducialData->append( ret );

		fidSeqIndex++;
		delete ret;
		delete tmp;
		tmp = NULL;

	}

	return fiducialData;
}

// WARNING: process ASSUMES image is in grayscale format
FiducialDataList * FiducialRecognition::processGen(
						unsigned char * img,
						GraphicImage * display, bool displayThreshold, bool geometryInfo  )
{
	if( img == NULL ){
		#ifndef __SYMBIAN32__
		cout << "img is NULL?" << endl;
		#endif //ndef __SYMBIAN32__
		return NULL;
	}

	unsigned char * tmpImg = img;
	unsigned char * tmpThr = _thresholdBuffer;
	_thresholdFilter->setThreshold( tmpImg, tmpThr );

	if( display ){
		if( displayThreshold ){
			display->markThreshold( img, _thresholdBuffer );
		}
	}

	_sceneRAG->reset();
	//RAGBuilder ragBuilder( img, _width, _height, _thresholdBuffer, _sceneRAG );

	//int ragBuilt = ragBuilder.buildRAGFullBorder(_graphSize/2);
	int ragBuilt = RAGBuilder::buildRAGFullBorderL( img, _thresholdBuffer, _sceneRAG );
	//cout << "ragBuilt: " << ragBuilt << endl;

	if( ragBuilt != 0 ){
		#ifndef __SYMBIAN32__
		if( display ){
			display->fillRect(_width/3,_height/3,2*_width/3,2*_height/3);
		}
		#endif //ndef __SYMBIAN32__
		return NULL;
	}

	#ifndef __SYMBIAN32__
	FiducialDataList * fiducialData = FiducialDataList::newL();
	#else
	//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("calling FiducialDataList::newL") );
	FiducialDataList * fiducialData = NULL;
	TRAPD( err1, fiducialData = FiducialDataList::newL() );
	//RFileLogger::Write(_L("debug"), _L("debug.txt"), EFileLoggingModeAppend, _L("FiducialDataList::newL left") );
	#endif


	FiducialDataList * tmp = _sceneRAG->findFiducialsL( );

	if( _sequenceStore != NULL  ){
		//cout << "sequenceStore not null" << endl;
		for(tmp->reset();!(tmp->nullTest());tmp->fwd()){
			FiducialData curr = tmp->getData();
			//cout << "about to call contains(" << endl;
			if( _sequenceStore->contains( curr.getSequence(), curr.getColor() ) ){
				// calculate the additional information about this marker
				if( geometryInfo ){
					_sceneRAG->calculateInfo( curr );
				}else{
					curr.Size() = 1500;
					curr.Angle() = 0;
				}

				// append current result to filtered
				fiducialData->appendL( curr );
				if( display ){
					display->fill(_sceneRAG->getRegion( curr.getRootRegion() ), _sceneRAG->getLabelsMap());
				}
				//cout << curr.getSequenceString() << " " << curr.Angle() << endl;
			}
		}
	}else{
		//cout << "sequenceStore null" << endl;
		fiducialData->append( tmp );
	}

	//fiducialData->append( tmp );
	delete tmp;
	tmp = NULL;

	return fiducialData;
}

dtouch_Region * FiducialRecognition::getRegion( const int& region ){
	return this->_sceneRAG->getRegion( region );
}


// end of file
