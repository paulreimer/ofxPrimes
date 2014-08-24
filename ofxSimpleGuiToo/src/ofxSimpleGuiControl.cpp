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


#include "ofxSimpleGuiControl.h"

map<int,ofImage> ofxSimpleGuiControl::_chrome;

ofxSimpleGuiControl::ofxSimpleGuiControl(string name) {
	controlType = "";
	this->config = &defaultSimpleGuiConfig;
	setName(name);
	setKey(key);
	setPos(0, 0);
	lock	  = false;
	focused	  = false;
	newColumn = false;
	setKeyboardShortcut(0);

//	args.control = this;
	args = this;
	units = "";

	setup();
	initChrome();

	disableAllEvents();		// just for safety to make sure nothing is registered twice
//	enableAppEvents();
//	enableMouseEvents();
//	disableKeyEvents();
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setConfig(ofxSimpleGuiConfig *config) {
	this->config = config;
	setup();

	return *this;
}


ofxSimpleGuiControl &ofxSimpleGuiControl::setName(string newName) {
	name = newName;
	origName = name;
	if(key.compare("") == 0) setKey("");	// if key has not been set yet, set name as key too

	return *this;
}


ofxSimpleGuiControl &ofxSimpleGuiControl::setKey(string newKey) {
	if(newKey.compare("") == 0) key = name;
	else key = newKey;
	for(int i=0; i<key.size(); i++) {
		if(key[i] == ' ') key[i] = '_';
	}

	return *this;
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setTextColor(bool clickable) {
	if(isMouseOver() && clickable) ofSetColor(config->textOverColor);
	else ofSetColor(config->textColor);

	return *this;
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setTextBGColor(bool clickable) {
	if(isMouseOver() && clickable) ofSetColor(config->textBGOverColor);
	else ofSetColor(config->textBGColor);

	return *this;
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setFullColor(bool forceActive) {
	if(isMouseDown() || forceActive) ofSetColor(config->fullActiveColor);
	else if(isMouseOver()) ofSetColor(config->fullOverColor);
	else ofSetColor(config->fullColor);

	return *this;
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setEmptyColor() {
	ofSetColor(config->emptyColor);
	//		if(isMouseOver()) ofSetColor(config->overColor.r, config->overColor.g, config->overColor.b);
	//		if(focused && !isMouseOver()) ofSetColor(config->focusColor.r, config->focusColor.g, config->focusColor.b);


	return *this;
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setKeyboardShortcut(char c) {
	keyboardShortcut = c;
	if(c) {
	//	printf("ofxSimpleGuiControl::setKeyboardShortcut %s %c\n", name.c_str(), c);
		name = origName + " (" + c + ")";
	} else {
		name = origName;
	}

	return *this;
}

ofxSimpleGuiControl &ofxSimpleGuiControl::setUnits(string units) {
	this->units = units;

	return *this;
}

void ofxSimpleGuiControl::initChrome()
{
	if (chrome().empty())
		resetChrome();
}

void ofxSimpleGuiControl::resetChrome()
{
//		chrome()[BORDER_LEFT]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-border-left.png");
//		chrome()[BORDER_RIGHT]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-border-right.png");
	chrome()[BORDER_TOP]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-border-top.png");
	chrome()[BORDER_BOTTOM]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-border-bottom.png");
	chrome()[FILL_MIDDLE]	.loadImage(config->chromeDir + "ofxSimpleGuiControl-fill-middle.png");

	ofTexture& tex = chrome()[FILL_MIDDLE].getTextureReference();
	tex.bind();
	glTexParameterf(tex.texData.textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	tex.unbind();
}

void ofxSimpleGuiControl::draw() { draw(x, y); }
void ofxSimpleGuiControl::draw(float x, float y)
{
	double o;
	ofPoint padding = config->padding;

	map<int,ofImage>::iterator im_it;

	ofEnableAlphaBlending();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	im_it = chrome().find(FILL_MIDDLE);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		im.draw(x - padding.x, y - padding.y, width + 2*padding.x, height + 2*padding.y);
	}

	im_it = chrome().find(BORDER_LEFT);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		o = im.width * height/im.height;
		im.draw(x - o - padding.x, y - padding.y, o, y + height + 2*padding.y);
	}

	im_it = chrome().find(BORDER_RIGHT);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		o = im.width * height/im.height;
		im.draw(x + width + padding.x, y - padding.y, o, y + height + 2*padding.y);
	}

	im_it = chrome().find(BORDER_TOP);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		o = im.height * width/im.width;
		im.draw(x - padding.x, y - o - padding.y, width + 2*padding.x, o);
	}

	im_it = chrome().find(BORDER_BOTTOM);
	if (im_it != chrome().end())
	{
		ofImage& im = im_it->second;
		o = im.height * width/im.width;
		im.draw(x - padding.x, y + height + padding.y, width + 2*padding.x, o);
	}

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	ofDisableAlphaBlending();

	drawWidget(x, y);
}
