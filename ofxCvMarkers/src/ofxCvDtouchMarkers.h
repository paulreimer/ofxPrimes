#pragma once

#include "ofMain.h"

#include "fiducialrecognition.h"
#include "fiducialTracking.h"

#include "ofxOpenCv.h"
#include "ofxCvDtouchStores.h"

class ofxCvDtouchMarkerFinder : public ofBaseUpdates, public ofRectangle {
public:

	ofxCvDtouchMarkerFinder();
	virtual ~ofxCvDtouchMarkerFinder();

	void setup();
	void update();
	void destroy();

	unsigned char* getPixels() {
		return displayBufferPtr->getImageBuffer();
	}

	ofxCvGrayscaleImage		displayImg;
	ofxCvGrayscaleImage		grayImg;

	int fiducialsTarget;
	std::list<FiducialData>	fiducials;

	ThresholdFilter::ThresholdType thresholdMethod, currThresholdMethod;
	int thresholdParam1, currThresholdParam1;
	int thresholdParam2, currThresholdParam2;

	int trackingDist;
	int trackingThresh;

	bool noFiducials;
	bool doTracking;
	bool doGenerateRecognition;
	bool displayThresholded;
	bool calcGeometry;

	std::string				storeFilename;
protected:
	FiducialRecognition*	fiducialRecognitionPtr;
	FiducialTracking*		fiducialTrackingPtr;
	GraphicImage*			displayBufferPtr;

	SequenceFileStore*		store;
//	NonNullStore<1>				store;
//private:
};
