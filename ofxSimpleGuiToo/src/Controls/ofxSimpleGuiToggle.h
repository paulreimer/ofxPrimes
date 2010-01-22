#pragma once

#include "ofxSimpleGuiControl.h"


//------------------------------------------------------------------------------ toggle
class ofxSimpleGuiToggle : public ofxSimpleGuiButton {
	
public:
	bool	*value;
	bool	oldValue;
	
	//---------------------------------------------------------------------
	ofxSimpleGuiToggle(string name, bool &value) : ofxSimpleGuiButton(name, value) {
		this->value	= &value;
		setMomentary(false);
		controlType = "Toggle";
		setup();
	}	
	
	virtual void setup() {
		setSize(config->gridSize.x - config->padding.x, config->toggleHeight);
	}

	void onKeyEnter() {
		toggle();
	}
};
