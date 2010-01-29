#include "ofxSimpleGuiFPSCounter.h"

//---------------------------------------------------------------------
ofxSimpleGuiFPSCounter::ofxSimpleGuiFPSCounter()
: ofxSimpleGuiControl("FPS Counter")
{
	controlType = "FPSCounter";
	setup();
}

//---------------------------------------------------------------------
void
ofxSimpleGuiFPSCounter::setup()
{
	setSize(config->gridSize.x - config->margin.x, config->titleHeight);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiFPSCounter::drawWidget(float x, float y)
{
	setPos(x, y);
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	setTextColor(false);
	config->font.drawString("FPS: " + ofToString(ofGetFrameRate()), config->fontOffset.x, config->fontOffset.y);
	
	glPopMatrix();
}
