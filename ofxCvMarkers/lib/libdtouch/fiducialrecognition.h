/***************************************************************************
                       fiducialrecognition.h  -  description
                             -------------------
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

*/

#ifndef EC_FIDUCIALRECOGNITION
#define EC_FIDUCIALRECOGNITION

#include "pool.h"
#include "listhash.h"
#include "graphicimage.h"
#include "thresholdfilter.h"

#define _EC_SAUVOLA

//using namespace dtouch;

class RegionAdjacencyGraph;

class FiducialRecognition{
protected:
	unsigned int _width;
	unsigned int _height;

	unsigned char *_thresholdBuffer;

	//int _noFiducialSequences;
	//int **_fiducialSequence;
	//bool *_usingLinearFiducials;
	List<int*> * _fiducialSequence;
	List<bool> * _geometricDecoding;

	int _noRegions;

	FiducialRecognition() { }

protected:
	//virtual void setThreshold( unsigned char * img ) = 0;
	ThresholdFilter * _thresholdFilter;
	ThresholdFilter::ThresholdType _thresholdMethod;

	// TODO: the following should be owned by the rag
	//int _graphSize;
	//Pool<DTPoint> *_regionsPool;
	//ListHash<int> **_edges;
	//dtouch_Region **_region;
	//int **_labelsMap;
	//int *_labels;
	//Pool<int *> *_labelsPtrPool;
	//bool *_stored;

	RegionAdjacencyGraph * _sceneRAG;
	int _ragSize;

	FiducialRecognition( const unsigned int &in_width, const unsigned int &in_height );
	void constructL( const int &in_param1, const int &in_param2 );

public:
	static FiducialRecognition * newL( const unsigned int &in_width, const unsigned int &in_height,
											const int &in_param1, const int &in_param2 );

	void addSequenceL( const int *in_fiducialSequence, bool in_geometricDecoding=false );
	void addSequenceL( const char * in_fiducialSequence, bool in_geometricDecoding=false );

protected:
	void initL();
	void initThresholdL( unsigned int in_width, unsigned int in_height, int in_param1, int in_param2 );

public:
	virtual ~FiducialRecognition();

	//FiducialDataList * process( unsigned char *img );
	//FiducialDataList * process( unsigned char *img, GraphicImage *display, bool displayThreshold=false );
	FiducialDataList * process( unsigned char *img, GraphicImage *display = NULL, bool displayThreshold = false, bool geometryInfo = false );

	unsigned char * getThresholdImage(){ return _thresholdBuffer; }

	int getNoRegions(){ return _noRegions; }

protected:
	//bool(*_check)(int*,bool);
	SequenceStore * _sequenceStore;
public:
	void setSequenceStore( SequenceStore * store ) { _sequenceStore = store; }
	FiducialDataList * processGen( unsigned char *img, GraphicImage *display, bool displayThreshold = false, bool geometryInfo = false );

public:
	void switchThresholdMethodL();
	void setThresholdMethodL( ThresholdFilter::ThresholdType in_type, int in_param1 = -1, int in_param2 = -1 );
	int getNoSequences(){ return _fiducialSequence->getSize(); }

	dtouch_Region * getRegion( const int& region );
};


#endif



