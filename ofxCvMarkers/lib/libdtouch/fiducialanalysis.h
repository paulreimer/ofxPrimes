
/***************************************************************************
                       fiducialanalysis.h  -  description
                             -------------------
    begin                : Thu Mar  1 2007
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


#ifndef EC_FIDUCIALANALYSIS
#define EC_FIDUCIALANALYSIS

#include "pool.h"
#include "listhash.h"
#include "graphicimage.h"
#include "regionadjacencygraph.h"
#include "thresholdfilter.h"

//using namespace dtouch;

class FiducialAnalysis{
protected:

	unsigned int _width;
	unsigned int _height;

	/*
	int _graphSize;
	Pool<DTPoint> *_regionsPool;
	ListHash<int> **_edges;
	dtouch_Region **_region;
	int **_labelsMap;
	int *_labels;
	bool *_stored;
	Pool<int *> *_labelsPtrPool;
	//*/

	//IntList * _sequence;
	//int _root;
	//int _background;

	// variables used for analyse
	unsigned char * _anlThresholdBuffer;
	RegionAdjacencyGraph * _anlSceneRAG;

	// variables used for compare
	unsigned char * _cmpThresholdBuffer;
	RegionAdjacencyGraph * _cmpSceneRAG;

	// variables used for findDifferences
	unsigned char * _fdThresholdBuffer1;
	unsigned char * _fdThresholdBuffer2;
	unsigned char * _fdThresholdBuffer3;
	RegionAdjacencyGraph * _fdSceneRAG1;
	RegionAdjacencyGraph * _fdSceneRAG2;
	RegionAdjacencyGraph * _fdSceneRAG3;

	// variables used for getRAG
	unsigned char * _getThresholdBuffer;
	RegionAdjacencyGraph * _getSceneRAG;

	FiducialAnalysis() { }

public:
	FiducialAnalysis( int in_width, int in_height, int in_param1, int in_param2 );

protected:
	void init( unsigned int in_width, unsigned int in_height );
	void initThreshold( unsigned int in_width, unsigned int in_height, int in_param1, int in_param2 );
	ThresholdFilter * _thresholdFilter;
	ThresholdFilter::ThresholdType _thresholdMethod;

	void findRoot( RegionAdjacencyGraph * rag, unsigned char * img, int& root, int &background );
	void findMatch(int& maxSize, int&maxIndex, const int target1,
		dtouch_Region ** region1, dtouch_Region ** region2,
			   int ** labelsMap1, int ** labelsMap2, const int noRegions2, bool colorAware=false);
public:
	~FiducialAnalysis();

	int analyse( unsigned char *img, GraphicImage *display,
				PointListPtrList * badRegions, IntList * sequence, bool& whiteRoot, PointList * rootRegion = NULL );

	bool compare( unsigned char *img, GraphicImage *display,
			IntList * origSeq );

	PointListPtrList * findDifferences( unsigned char *img1, unsigned char *img2,
		GraphicImage *display );

	//bool breaksUniformly( unsigned char *img1, unsigned char *img2,
	//	GraphicImage *display );

	DTPoint getRootSize(unsigned char *img);

	int getRAG( unsigned char *img, GraphicImage *display, IntList *** rag, int& noRegions );
public:
	void switchThresholdMethod();
	void setThresholdMethod( ThresholdFilter::ThresholdType in_type, int in_param1 = -1, int in_param2 = -1 );
	ThresholdFilter * getThresholdFilter() { return _thresholdFilter; }
	//unsigned char * getThresholdBuffer() { return _thresholdBuffer; }

};

#endif // FIDUCIALANALYSIS



