#pragma once

#include "ofxSimpleGuiControl.h"


//------------------------------------------------------------------------------ fps counter
class ofxSimpleGuiFPSCounter : public ofxSimpleGuiControl
{
	
public:
	ofxSimpleGuiFPSCounter();
	
	void setup();

	void loadFromXML(ofxXmlSettings &XML) {};
	void saveToXML(ofxXmlSettings &XML) {};
	
	void drawWidget(float x, float y);
};
