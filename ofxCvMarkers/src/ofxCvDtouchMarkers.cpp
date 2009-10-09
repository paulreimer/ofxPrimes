#pragma once

#include "ofxCvDtouchMarkers.h"

ofxCvDtouchMarkerFinder::ofxCvDtouchMarkerFinder() {
	fiducialRecognitionPtr = NULL;
	displayBufferPtr = NULL;

	noFiducials = 0;
	fiducialsTarget = 5;
	
	thresholdMethod = currThresholdMethod = ThresholdFilter::EFixed;
	thresholdParam1 = currThresholdParam1 = -1;
	thresholdParam2 = currThresholdParam2 = -1;

	doGenerateRecognition = false;
	
	trackingDist = width / 12;
	trackingThresh = 5;
	
	doTracking = false;
	displayThresholded = false;
	calcGeometry = false;
}

ofxCvDtouchMarkerFinder::~ofxCvDtouchMarkerFinder() {
	destroy();
}

void ofxCvDtouchMarkerFinder::setup() {
	if(fiducialRecognitionPtr != NULL)
		delete fiducialRecognitionPtr;

	fiducialRecognitionPtr = FiducialRecognition::newL(width,height,20,32);
	
	if (store != NULL)
		delete store;
	
	if (!storeFilename.empty()) {
		store = new SequenceFileStore(storeFilename.c_str());
		fiducialRecognitionPtr->setSequenceStore(store);
	}

	//		store.loadSeqFromFile(storeFilename.c_str());


	if(fiducialTrackingPtr != NULL)
		delete fiducialTrackingPtr;
	
	if(doTracking)
		fiducialTrackingPtr = FiducialTracking::newL(trackingDist, trackingThresh);
	
//	int seq[] = { 5, 0, 1, 2, 3, 4 };
//	int seq[] = { 1, 1, 3, 5 };
//	store.addSequence(seq, false);

	displayImg.allocate(width, height);
	grayImg.allocate(width, height);
	
	// this class is used to display debug graphical information 
	// from FiducialRecognition
	displayBufferPtr = new GraphicImageGrayscale(width,height);

	//displayBufferPtr = new GraphicImageRGB(width,height);
	displayBufferPtr->setImageBuffer((unsigned char**)&displayImg.getCvImage()->imageData);
}

void ofxCvDtouchMarkerFinder::update() {
	// Remove this later by doing memcmp?
	fiducials.clear();
	
	FiducialDataList* fiducialData = NULL;
	if (fiducialRecognitionPtr) {
		if((thresholdParam1 != currThresholdParam1)
		   ||(thresholdParam2 != currThresholdParam2)
		   ||(thresholdMethod != currThresholdMethod
			  && (ThresholdFilter::EBernsen<= thresholdMethod
				  || thresholdMethod <= ThresholdFilter::EFixed))
		) {
			fiducialRecognitionPtr->setThresholdMethodL( thresholdMethod, thresholdParam1, thresholdParam2 );

			currThresholdMethod = thresholdMethod;
			currThresholdParam1 = thresholdParam1;
			currThresholdParam2 = thresholdParam2;
		}

		if(doGenerateRecognition)
			fiducialData = fiducialRecognitionPtr->processGen(grayImg.getPixels(),
															  displayBufferPtr,
															  displayThresholded,
															  calcGeometry);
		else
			fiducialData = fiducialRecognitionPtr->process(grayImg.getPixels(),
														   displayBufferPtr,
														   displayThresholded,
														   calcGeometry);
		
		grayImg.flagImageChanged();

		displayImg.flagImageChanged();
	}
	
	if(doTracking && fiducialTrackingPtr != NULL){
		if(fiducialData == NULL)
			fiducialData = FiducialDataList::newL();
		
		if(trackingDist != fiducialTrackingPtr->_distanceThreshold)
			fiducialTrackingPtr->_distanceThreshold = (trackingDist>1)? trackingDist:1;

		if(trackingThresh != fiducialTrackingPtr->_trackingThreshold)
			fiducialTrackingPtr->_trackingThreshold = (trackingThresh>1)? trackingThresh:1;

		fiducialTrackingPtr->process(fiducialData);
	}
	
	if (fiducialData != NULL) {		
		noFiducials = fiducialData->getSize();
		
		for(fiducialData->reset();!(fiducialData->nullTest());fiducialData->fwd())
			fiducials.push_back(fiducialData->getData());
	}
}

void ofxCvDtouchMarkerFinder::destroy() {
	grayImg.clear();
	displayImg.clear();
	
	if (store != NULL)
		delete store;

	if(fiducialRecognitionPtr != NULL)
		delete fiducialRecognitionPtr;

	if(fiducialTrackingPtr != NULL)
		delete fiducialTrackingPtr;

	if(displayBufferPtr != NULL)
		delete displayBufferPtr;
}
