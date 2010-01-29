#include "ofxSimpleGuiToggle.h"

//---------------------------------------------------------------------
ofxSimpleGuiToggle::ofxSimpleGuiToggle(string name, bool &value)
: ofxSimpleGuiButton(name, value)
{
	this->value	= &value;
	setMomentary(false);
	controlType = "Toggle";
	setup();
}	

//---------------------------------------------------------------------
void
ofxSimpleGuiToggle::setup()
{
	setSize(config->gridSize.x - config->margin.x, config->toggleHeight);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiToggle::onKeyEnter()
{
	toggle();
}
