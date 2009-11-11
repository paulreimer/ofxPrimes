/***********************************************************************

 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.

 based on Todd Vanderlin's ofxSimpleGui API
 http://toddvanderlin.com/

 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ***********************************************************************/


#include "ofxSimpleGuiPage.h"

ofxSimpleGuiPage::ofxSimpleGuiPage(string name) : ofxSimpleGuiControl(name) {
	disableAllEvents();
	width = 0;
	height = 0;
	ofAddListener(ofEvents.update, (ofxMSAInteractiveObject*) this, &ofxMSAInteractiveObject::_update);
}

ofxSimpleGuiPage::~ofxSimpleGuiPage() {
	// delete all controls
}


void ofxSimpleGuiPage::loadFromXML(ofxXmlSettings &XML) {
	for(int i=0; i < controls.size(); i++) {
		controls[i]->loadFromXML(XML);
	}
}


void ofxSimpleGuiPage::saveToXML(ofxXmlSettings &XML) {
	for(int i=0; i < controls.size(); i++) {
		controls[i]->saveToXML(XML);
	}
}

float ofxSimpleGuiPage::getNextY(float y) {
	int iy = (int)ceil(y/config->gridSize.y);
//	return y;
	return (iy) * config->gridSize.y;
}


void ofxSimpleGuiPage::draw(float x, float y, bool alignRight) {

	if(alignRight) x = ofGetWidth() - x -  config->gridSize.x;
		
	float posX		= 0;
	float posY		= 0;

	ofSetRectMode(OF_RECTMODE_CORNER);

	for(int i=0; i<controls.size(); i++) {
		ofxSimpleGuiControl &control = *controls[i];

		if (!control.enabled)
			continue;

		if(control.newColumn) {
			if(alignRight) posX -= config->gridSize.x;
			else posX += config->gridSize.x;
			posY = 0;
		}
		
		float controlX = posX + x;
		float controlY = posY + y;
		
		control.draw(controlX, controlY);
		ofNoFill();
		ofSetColor(config->borderColor);

		ofEnableSmoothing();
		//		glLineWidth(0.1);
		ofRect(controlX, controlY, control.width, control.height);
//		glLineWidth(1.0);
		ofDisableSmoothing();
		
		posY = getNextY(posY + control.height + config->padding.y);

		float addHeight = posY + control.height;
		if(addHeight > height
		   || y + addHeight > ofGetHeight()) {
			if(alignRight) posX -= config->gridSize.x;
			else posX += config->gridSize.x;
			posY = 0;
		}
		
		//		if(guiFocus == controls[i]->guiID) controls[i]->focused = true;		// MEMO
		//		else							   controls[i]->focused = false;
	}

}


ofxSimpleGuiControl &ofxSimpleGuiPage::addControl(ofxSimpleGuiControl& control) {
	control.enableMouseEvents();
	control.enableKeyEvents();
	if (control.newColumn) {
		height = MAX(height, control.height);
		width += MAX(config->gridSize.x,control.width);
	} else {
		width = MAX(width,control.width);
		height += getNextY(control.height);
	}
	
	controls.push_back(&control);
	return control;
}

ofxSimpleGuiButton &ofxSimpleGuiPage::addButton(string name, bool &value) {
	return (ofxSimpleGuiButton &)addControl(* new ofxSimpleGuiButton(name, value));
}

ofxSimpleGuiContent &ofxSimpleGuiPage::addContent(string name, ofBaseDraws &content, float fixwidth) {
	if(fixwidth == -1) fixwidth = config->gridSize.x - config->padding.x;
	return (ofxSimpleGuiContent &)addControl(* new ofxSimpleGuiContent(name, content, fixwidth));
}

ofxSimpleGuiFPSCounter &ofxSimpleGuiPage::addFPSCounter() {
	return (ofxSimpleGuiFPSCounter &)addControl(* new ofxSimpleGuiFPSCounter());
}

//ofxSimpleGuiQuadWarp &ofxSimpleGuiPage::addQuadWarper(string name, float x, float y, float sw, float sh, ofPoint &&pts) {
//	return (ofxSimpleGuiQuadWarp &)addControl(* new ofxSimpleGuiQuadWarp(name, x, y, sw, sh, pts));
//}
//
#ifndef TARGET_OF_IPHONE
ofxSimpleGuiMovieSlider &ofxSimpleGuiPage::addMovieSlider(string name, ofVideoPlayer* input) {
	return (ofxSimpleGuiMovieSlider &)addControl(* new ofxSimpleGuiMovieSlider(name, input));
}
#endif

ofxSimpleGuiSliderInt &ofxSimpleGuiPage::addSlider(string name, int &value, int min, int max) {
	return (ofxSimpleGuiSliderInt &)addControl(* new ofxSimpleGuiSliderInt(name, value, min, max, 0));
}

ofxSimpleGuiSliderByte &ofxSimpleGuiPage::addSlider(string name, byte &value, byte min, byte max) {
	return (ofxSimpleGuiSliderByte &)addControl(* new ofxSimpleGuiSliderByte(name, value, min, max, 0));
}

ofxSimpleGuiSliderFloat &ofxSimpleGuiPage::addSlider(string name, float &value, float min, float max, float smoothing) {
	return (ofxSimpleGuiSliderFloat &)addControl(* new ofxSimpleGuiSliderFloat(name, value, min, max, smoothing));
}

ofxSimpleGuiSliderDouble &ofxSimpleGuiPage::addSlider(string name, double &value, double min, double max, double smoothing) {
	return (ofxSimpleGuiSliderDouble &)addControl(* new ofxSimpleGuiSliderDouble(name, value, min, max, smoothing));
}

ofxSimpleGuiSlider2d &ofxSimpleGuiPage::addSlider2d(string name, ofPoint& value, float xmin, float xmax, float ymin, float ymax) {
	return (ofxSimpleGuiSlider2d &)addControl(* new ofxSimpleGuiSlider2d(name, value, xmin, xmax, ymin, ymax));
}

ofxSimpleGuiSliderString &ofxSimpleGuiPage::addSlider(string name, int &value, map<int,string> strs, int min, int max) {
	return (ofxSimpleGuiSliderString &)addControl(* new ofxSimpleGuiSliderString(name, value, strs, min, max));
}

ofxSimpleGuiSliderContent &ofxSimpleGuiPage::addSlider(string name, int &value,
													  map<int, string> strs,
													  map<int, ofBaseDraws*> contents,
													  int min, int max) {
	return (ofxSimpleGuiSliderContent &)addControl(* new ofxSimpleGuiSliderContent(name, value, strs, contents, min, max));
}
	
ofxSimpleGuiTitle &ofxSimpleGuiPage::addTitle(string name, bool* value) {
	return (ofxSimpleGuiTitle &)addControl(* new ofxSimpleGuiTitle(name, value));
}

ofxSimpleGuiToggle &ofxSimpleGuiPage::addToggle(string name, bool &value) {
	return (ofxSimpleGuiToggle &)addControl(* new ofxSimpleGuiToggle(name, value));
}

ofxSimpleGuiControl &ofxSimpleGuiPage::setConfig(ofxSimpleGuiConfig *config) {
	for(int i=0; i<controls.size(); i++)
		controls[i]->setConfig(config);
	
	return *this;	
}
