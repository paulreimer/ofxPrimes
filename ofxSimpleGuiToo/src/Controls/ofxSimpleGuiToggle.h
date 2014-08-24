#pragma once

#include "ofxSimpleGuiButton.h"

//------------------------------------------------------------------------------ toggle
class ofxSimpleGuiToggle : public ofxSimpleGuiButton
{

public:
	bool	*value;
	bool	oldValue;

	ofxSimpleGuiToggle(string name, bool &value);

	virtual void setup();

	void onKeyEnter();
};
