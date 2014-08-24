#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiSliderContent.h"
#include "ofxSimpleGuiSliderString.h"

class ofxSimpleGuiSliderContent : public ofxSimpleGuiSliderString
{
public:
	map<int, ofBaseDraws*> contents;
	float			fixwidth;
	float			fixheight;
	float			sliderwidth;
	float			sliderheight;
	ofBaseDraws		*content;

	ofxSimpleGuiSliderContent(string name, int &value,
							  map<int, string> strs,
							  map<int, ofBaseDraws*> contents,
							  int min=0, int max=0);
	void setup();

	void updateContent();

	virtual void onPress(int x, int y, int button);
	virtual void onDragOver(int x, int y, int button);
	virtual void onDragOutside(int x, int y, int button);

	void drawWidget(float x, float y);};
