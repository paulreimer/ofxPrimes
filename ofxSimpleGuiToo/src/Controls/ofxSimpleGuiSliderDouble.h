#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiSliderBase.h"


class ofxSimpleGuiSliderDouble : public ofxSimpleGuiSliderBase<double> {

public:
	ofxSimpleGuiSliderDouble(string name, double &value, double min, double max, double smoothing = 0) : ofxSimpleGuiSliderBase<double>(name, value, min, max, smoothing) {
		controlType = "SliderDouble";
	}
};
