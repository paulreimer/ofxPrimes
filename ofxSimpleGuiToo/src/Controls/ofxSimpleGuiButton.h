#pragma once

#include "ofxSimpleGuiControl.h"


//------------------------------------------------------------------------------ button
class ofxSimpleGuiButton : public ofxSimpleGuiControl {
	
public:
	
	bool*	value;
	bool	oldValue;
	
	bool	momentary;
	
	//---------------------------------------------------------------------
	ofxSimpleGuiButton(string name, bool &value) : ofxSimpleGuiControl(name) {
		setMomentary(true);
		this->value	= &value;
		controlType = "Button";
		setup();
	}
	
	virtual void setup() {
		setSize(config->gridSize.x - config->padding.x, config->buttonHeight);
	}
	
	void loadFromXML(ofxXmlSettings &XML) {
		set(XML.getValue("controls:" + controlType + "_" + key + ":value", 0));
	}
	
	void saveToXML(ofxXmlSettings &XML) {
		XML.addTag(controlType + "_" + key);
		XML.pushTag(controlType + "_" + key);
		XML.addValue("name", name);
		XML.addValue("value", getValue());
		XML.popTag();
	}

	ofxSimpleGuiButton& setMomentary(bool m) {
		momentary = m;
		return *this;
	}	
	
	bool getValue() {
		return (*value);
	}
	void set(bool b) {
		if (oldValue != b)
		{		
			oldValue = (*value) = b;
			ofNotifyEvent(valueChangedEvt, args);
		}
	}
	void toggle() {
		oldValue = (*value) = !(*value);
		ofNotifyEvent(valueChangedEvt, args);
	}
	
	void onPress(int x, int y, int button) {
		if(momentary) set(true);
		else toggle();
	}
	
	void onRelease(int x, int y, int button) {
		if(momentary) set(false);
	}	
	
	void keyPressed( int key ) {
		if(key==keyboardShortcut) onPress(0, 0, 0);
	}
	
	void keyReleased( int key ) {
		if(key==keyboardShortcut) onRelease(0, 0, 0);
	}
	
	void draw(float x, float y) {
		setPos(x, y);
		
		glPushMatrix();
		glTranslatef(x, y, 0);
		
		ofEnableAlphaBlending();
		ofFill();
		setTextBGColor();
		setFullColor(*value);
		ofRect(0, 0, width, height);
		
		// if a toggle
		if((*value) && !momentary) {
			setTextColor();
			if (1)
			{
				float s=height/2;

				float r = s*(2.0/3.0);
				float o = sqrtf(r*r/2);

				setFullColor();
				ofCircle(s, s, r);

				setTextColor();
				ofLine(s-o, s-o, s+o, s+o);
				ofLine(s-o, s+o, s+o, s-o);
			}
		}
		setTextBGColor();
		ofRect(height, 0, width - height, height);

		ofFill();

		setTextColor();
//		config->font.drawString(name, config->fontOffset.x, config->fontOffset.y);
		config->font.drawString(name, height+4, config->fontOffset.y+6);

		ofDisableAlphaBlending();
		
		glPopMatrix();
	}
	
};
