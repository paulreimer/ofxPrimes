#pragma once

#include "ofxSimpleGuiIncludes.h"
//#include "ofxFiducialBackedControl.h"


#include "ofxFiducialTracker.h"
#include "ofxFiducial.h"

#define NULL_FIDUCIAL_ID -1

class ofxFiducialBackedGui  : public ofxSimpleGuiPage {
public:
	ofxFiducialBackedGui(ofxFiducialTracker *tracker=NULL);
//	~ofxFiducialBackedGui();
	
	/* virtual */void onPress(int x, int y, int button);
	/* virtual */void onDragOver(int x, int y, int button);
	/* virtual */void onDragOutside(int x, int y, int button);

	void setup();
	void update();
	void draw();

	ofxSimpleGuiControl		&addControl(ofxSimpleGuiControl& control);

	ofxFiducialTracker *tracker;	
	float x_scale, y_scale;
protected:
	int saveX, saveY;
	
	ofEvent<ofxFiducial> fiducialFoundEvt;
	ofEvent<ofxFiducial> fiducialLostEvt;
	ofEvent<ofxFiducial> fiducialUpdatedEvt;

	void fiducialFound(ofxFiducial &fiducial);
	void fiducialLost(ofxFiducial &fiducial);
	void fiducialUpdated(ofxFiducial &fiducial);
};
