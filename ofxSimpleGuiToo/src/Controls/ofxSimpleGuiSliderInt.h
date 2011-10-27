#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiSliderBase.h"


class ofxSimpleGuiSliderInt : public ofxSimpleGuiSliderBase<int> {

public:
	ofxSimpleGuiSliderInt(string name, int &value, int min, int max, float smoothing = 0) : ofxSimpleGuiSliderBase<int>(name, value, min, max, smoothing) {
		controlType = "SliderInt";
	}

	void drawSliderText() {
		string leftText, rightText;

		leftText = name+": ";

		string& sliderText = bValueRightAlign? rightText : leftText;
		sliderText = ofToString((*value))+units;

		config->font.drawString(leftText,
								config->fontOffset.x,
								config->sliderHeight + config->fontOffset.y);

		if (bValueRightAlign)
			config->font.drawString(rightText,
									width - config->font.stringWidth(rightText) - config->fontOffset.x,
									config->sliderHeight + config->fontOffset.y);
	}

};
