#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiSliderContent.h"
#include "ofxSimpleGuiSliderBase.h"

class ofxSimpleGuiSliderContent : public ofxSimpleGuiSliderString {

public:
	map<int, ofBaseDraws*> contents;
	float			fixwidth;
	float			fixheight;
	float			sliderwidth;
	float			sliderheight;
	ofBaseDraws		*content;
	
	ofxSimpleGuiSliderContent(string name, int &value,
							 map<int, string> strs,
							 map<int, ofBaseDraws*> contents,
							 int min=0, int max=0)
	: ofxSimpleGuiSliderString(name, value, strs, min, max? max : MAX(strs.size()-1,0))
	{

		controlType = "SliderContent";

		this->contents = contents;
	}
	
	void setup()
	{
		sliderwidth	= config->gridSize.x - config->padding.x;
		sliderheight = config->sliderHeight + config->sliderTextHeight;
		
		fixwidth = sliderwidth;
		fixheight = fixwidth * content->getHeight()/content->getWidth();

		setSize(MAX(fixwidth, sliderwidth),
				(fixheight + config->sliderTextHeight + sliderheight));

		pct		 = ofMap((*value), min, max, 0.0, sliderwidth);
		barwidth = pct;
	}
	
	void update()
	{
		map<int, string>::iterator chk_str = strs.find(*value);
		map<int, ofBaseDraws*>::iterator chk_content = contents.find(*value);

		if (chk_str == strs.end())
			return;

		if (chk_content != contents.end())
		{
			if (chk_content->second == content)
				return;

			content = chk_content->second;
			
			int sliderheight = config->sliderHeight + config->sliderTextHeight;

			if (content->getWidth() || content->getHeight())
				fixheight = fixwidth * content->getHeight()/content->getWidth();
			else
				fixheight = 0;

			setSize(width, fixheight + config->sliderTextHeight + config->padding.y + sliderheight);
		}
	}
	
	//--------------------------------------------------------------------- draw
	void draw(float x, float y)
	{
	    if(content->getWidth() == 0 && content->getHeight() ==0) return;
		
		float textOffset; 
		if (!bValueRightAlign)
			textOffset = config->fontOffset.x;
		else
			textOffset = width - config->font.stringWidth(name) - config->fontOffset.x;
		
		setPos(x, y);
		setup();
		
		glPushMatrix();
		glTranslatef(x, y, 0);
		ofEnableAlphaBlending();
		ofFill();
		//		glColor4f(0, 0, 0, 0.8f);
		//		ofRect(0, 0, width, fixheight);
		
		ofSetColor(0xffffff);
		content->draw(0, 0, width, fixheight);
		
		ofFill();
		setTextBGColor();
		ofRect(0, fixheight, width, config->sliderTextHeight);
		
		setTextColor();
		
		config->font.drawString(name, textOffset, fixheight + config->fontOffset.y);
		ofDisableAlphaBlending();
		glPopMatrix();

		x += fixwidth;
		y += fixheight + config->sliderTextHeight + config->padding.y;

		ofxSimpleGuiSliderString::draw(x, y);
	}
		
};
