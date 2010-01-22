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
	: ofxSimpleGuiSliderString(name, value, strs, min, max? max : MAX(MAX(strs.size(),contents.size())-1,0))
	{
		controlType = "SliderContent";

		this->contents = contents;
		this->content = NULL;
		
		updateContent();
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

	void updateContent()
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
	
	
	virtual void onPress(int x, int y, int button) {
		updateSlider();
		updateContent();
	}
	
	virtual void onDragOver(int x, int y, int button) {
		updateSlider();
		updateContent();
	}
	
	virtual void onDragOutside(int x, int y, int button) {
		updateSlider();
		updateContent();
	}
	
	//--------------------------------------------------------------------- draw
	void draw(float x, float y)
	{
	    if(content != NULL
		   && (content->getWidth() != 0 || content->getHeight() !=0))
		{
			setPos(x, y);
			setup(); // refresh heights/widths
			
			glPushMatrix();
			glTranslatef(x, y, 0);
//			ofEnableAlphaBlending();
			ofFill();
			//		glColor4f(0, 0, 0, 0.8f);
			//		ofRect(0, 0, width, fixheight);
			
			ofSetColor(0xffffff);
			content->draw(0, 0, width, fixheight);

			glPopMatrix();

			y += fixheight;
		}

		ofxSimpleGuiSliderString::draw(x, y);
	}

};
