#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiSliderBase.h"

class ofxSimpleGuiSliderString : public ofxSimpleGuiSliderBase<int> {

public:
	map<int, string> strs;

	ofxSimpleGuiSliderString(string name, int &value, map<int, string> strs, int min=0, int max=0)
	: ofxSimpleGuiSliderBase<int>(name, value, min, max? max : MAX(strs.size()-1,0)) {

		controlType = "SliderString";

		this->strs = strs;
	}
	
	void drawSliderText() {
		map<int, string>::iterator chk = strs.find(*value);
		string leftText, rightText;
		leftText = name+": ";

		string& sliderText = bValueRightAlign? rightText : leftText;
		
		if (chk != strs.end())
			sliderText += chk->second;
		else
			sliderText += "???";

		config->font.drawString(leftText,
								config->fontOffset.x,
								config->sliderHeight + config->fontOffset.y);

		if (bValueRightAlign)
			config->font.drawString(rightText,
									width - config->font.stringWidth(rightText) - config->fontOffset.x,
									config->sliderHeight + config->fontOffset.y);
	}

};
