#pragma once

#include "ofxSimpleGuiControl.h"

//------------------------------------------------------------------------------ toggle
class ofxSimpleGuiContent  : public ofxSimpleGuiControl
{
public:
	float			fixwidth;
	float			fixheight;
	ofBaseDraws		*content;
	
	short			textAlignment;
	enum			textAlignments
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,
		ALIGN_JUSTIFIED,
	};

	ofxSimpleGuiContent(string name, ofBaseDraws& content, float fixwidth=200.0);

	void setup();

	void drawWidget(float x, float y);
	
	inline virtual map<int,ofImage>& chrome();	
	void resetChrome();
	
private:
	static map<int,ofImage> _chrome;
};
