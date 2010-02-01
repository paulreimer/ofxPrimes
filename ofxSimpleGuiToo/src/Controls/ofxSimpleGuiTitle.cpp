#include "ofxSimpleGuiTitle.h"

map<int,ofImage> ofxSimpleGuiTitle::_chrome;

//---------------------------------------------------------------------
ofxSimpleGuiTitle::ofxSimpleGuiTitle(string name, bool &value)
: ofxSimpleGuiControl(name)
{
	beToggle	= false;
	beenPressed = false;
	
	this->value	= &value;
	
	value		= NULL;
	controlType = "Title";
	newColumn	= true;
	setup();
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::setup()
{
	setSize(config->font.stringWidth(name) + 2*config->fontOffset.x,
			config->font.stringHeight(name) + 2*config->fontOffset.y);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::loadFromXML(ofxXmlSettings &XML)
{
	if(!value) return;
	set(XML.getValue("controls:" + controlType + "_" + key + ":value", 0));
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::saveToXML(ofxXmlSettings &XML)
{
	if(!value) return;
	XML.addTag(controlType + "_" + key);
	XML.pushTag(controlType + "_" + key);
	XML.addValue("name", name);
	XML.addValue("value", getValue());
	XML.popTag();
}

//---------------------------------------------------------------------
bool
ofxSimpleGuiTitle::getValue()
{
	if(!value) return false;
	return (*value);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::set(bool b)
{
	if(!value) return;
	
	if (oldValue != b)
	{		
		oldValue = (*value) = b;
		ofNotifyEvent(valueChangedEvt, args);
	}
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::toggle()
{
	if(!value) return;
	set(!(*value));
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::setToggleMode(bool b)
{
	if(!value) return;
	beToggle = b;
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::onPress(int x, int y, int button)
{
	if(!value) return;
	beenPressed = true;	
	if(beToggle)
		set(!(*value));
	else
		set(true);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::onRelease(int x, int y, int button)
{
	if(!value) return;
	if(!beToggle)
		set(false);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::drawWidget(float x, float y)
{
	setPos(x, y);
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	ofEnableAlphaBlending();
	ofFill();
	
	// if a toggle
	if(value && (*value) && beToggle) {
		setTextColor();
		//ofLine(0, 0, box.width, box.height);
		//ofLine(box.width, 0, 0, box.height);
	}
	
	setTextColor(value != NULL);
	config->font.drawString(name, config->fontOffset.x, config->fontOffset.y);
	
	ofDisableAlphaBlending();
	
	glPopMatrix();
}


//---------------------------------------------------------------------
inline map<int,ofImage>&
ofxSimpleGuiTitle::chrome()
{
	return _chrome;
}

//---------------------------------------------------------------------
void
ofxSimpleGuiTitle::resetChrome()
{}
