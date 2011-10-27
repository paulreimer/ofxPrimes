#pragma once

#include "ofxSimpleGuiControl.h"

//------------------------------------------------------------------------------ button
class ofxSimpleGuiButton : public ofxSimpleGuiControl
{
public:

	bool*	value;
	bool	oldValue;

	bool	momentary;

	ofImage checkedImage;

	ofxSimpleGuiButton(string name, bool &value);

	virtual void setup();

	void loadFromXML(ofxXmlSettings &XML);
	void saveToXML(ofxXmlSettings &XML);

	ofxSimpleGuiButton& setMomentary(bool m);

	bool getValue();
	void set(bool b);
	void toggle();

	void onPress(int x, int y, int button);
	void onRelease(int x, int y, int button);

	void keyPressed(int key);
	void keyReleased(int key);

	void drawWidget(float x, float y);
};
