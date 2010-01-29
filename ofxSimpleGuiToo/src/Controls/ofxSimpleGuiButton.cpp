#include "ofxSimpleGuiButton.h"

//---------------------------------------------------------------------
ofxSimpleGuiButton::ofxSimpleGuiButton(string name, bool &value)
: ofxSimpleGuiControl(name)
{
	setMomentary(true);
	this->value	= &value;
	controlType = "Button";
	setup();
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::setup()
{
	setSize(config->gridSize.x - config->padding.x, config->buttonHeight);
	checkedImage.loadImage(config->chromeDir + "control-checked.png");
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::loadFromXML(ofxXmlSettings &XML)
{
	set(XML.getValue("controls:" + controlType + "_" + key + ":value", 0));
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::saveToXML(ofxXmlSettings &XML)
{
	XML.addTag(controlType + "_" + key);
	XML.pushTag(controlType + "_" + key);
	XML.addValue("name", name);
	XML.addValue("value", getValue());
	XML.popTag();
}

//---------------------------------------------------------------------
ofxSimpleGuiButton&
ofxSimpleGuiButton::setMomentary(bool m)
{
	momentary = m;
	return *this;
}	

//---------------------------------------------------------------------
bool
ofxSimpleGuiButton::getValue()
{
	return (*value);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::set(bool b)
{
	if (oldValue != b)
	{		
		oldValue = (*value) = b;
		ofNotifyEvent(valueChangedEvt, args);
	}
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::toggle()
{
	oldValue = (*value) = !(*value);
	ofNotifyEvent(valueChangedEvt, args);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::onPress(int x, int y, int button)
{
	if(momentary) set(true);
	else toggle();
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::onRelease(int x, int y, int button)
{
	if(momentary) set(false);
}	

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::keyPressed(int key)
{
	if(key==keyboardShortcut) onPress(0, 0, 0);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::keyReleased(int key)
{
	if(key==keyboardShortcut) onRelease(0, 0, 0);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiButton::drawWidget(float x, float y)
{
	setPos(x, y);
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	ofEnableAlphaBlending();
	ofFill();
	
	// if a toggle
	if((*value) && !momentary)
		checkedImage.draw(width - height - config->padding.y, 0, height, height);

	ofFill();
	
	setTextColor();
	config->font.drawString(name, config->fontOffset.x, config->fontOffset.y+6);
	
	ofDisableAlphaBlending();
	
	glPopMatrix();
}
