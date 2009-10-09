#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiSliderBase.h"
typedef unsigned char byte;

class ofxSimpleGuiSliderByte : public ofxSimpleGuiSliderBase<byte> {
	
public:
	ofxSimpleGuiSliderByte(string name, byte &value, byte min, byte max, float smoothing = 0) : ofxSimpleGuiSliderBase<byte>(name, value, min, max, smoothing) {
		controlType = "SliderByte";
	}

};
