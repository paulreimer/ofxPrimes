#pragma once

#include "ofxSimpleGuiPage.h"

#include "ofxFiducialTracker.h"
#include "ofxFiducial.h"

#define USE_FIDUCIALS
#define NULL_FIDUCIAL_ID -1
#define FIDUCIAL_ORIGIN_CORNER 1

class ofxFiducialBackedGui : public ofxSimpleGuiPage 
{
public:
	ofxFiducialBackedGui(string name="Fiducial Gui");
//	~ofxFiducialBackedGui();
	
	void setup();
	void update();
	void draw();

	ofPoint offset;
	float size;
	float angle;

	void activate(bool isActivated = true);	

protected:
	ofxSimpleGuiControl &addControl(ofxSimpleGuiControl& control);

//	void relocateMouseEvent(ofMouseEventArgs &old_ev, ofMouseEventArgs &new_ev);
	void relocateMouseEvent(ofMouseEventArgs &e);

	void _mouseMoved(ofMouseEventArgs &e);
	void _mousePressed(ofMouseEventArgs &e);	
	void _mouseDragged(ofMouseEventArgs &e);	
	void _mouseReleased(ofMouseEventArgs &e);	
};
