#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxFiducial.h"

class ofxFiducialBackedObject : public ofxMSAInteractiveObject {
public:
	ofxFiducialBackedObject();
	virtual ~ofxFiducialBackedObject();

	float angle;
	void rotateRad(float angle);

	bool hitTest(int tx, int ty); // handles translation and rotation
	bool translateEvents, rotateEvents;
		
	ofxFiducial* fiducial;
	
	unsigned char& type() { return this->ref_type; }
	
protected:
	void translateMouse(int* mx, int* my);
	void rotateMouse(int* mx, int* my);
	ofMouseEventArgs* relocateMouseEvent(ofMouseEventArgs *e, bool do_rotate = true, bool do_translate = true);

	unsigned char ref_type;
};
