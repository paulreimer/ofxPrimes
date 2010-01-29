#pragma once

#include "ofxSimpleGuiControl.h"


//------------------------------------------------------------------------------ quad warp
class ofxSimpleGuiQuadWarp : public ofxSimpleGuiControl
{
public:
	float			drawx, drawy;
	ofPoint			mouse;
	ofPoint			*pts;
	ofPoint			*pts_int;
	ofPoint			*cvpts;
	int				onPnt;
	bool			mouseOn;
	float			sw, sh;
	
	ofxSimpleGuiQuadWarp(string name, float x, float y, float sw, float sh, ofPoint **inpts);
	
	bool checkDistanceToPoint();

	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased();
	
	void drawWidget(float x, float y);

};
