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


#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxSimpleGuiConfig.h"
#include "ofxXmlSettings.h"

class ofxSimpleGuiControl;
typedef ofxSimpleGuiControl* controlEvtArgs;
/*
struct controlEvtArgs {
	ofxSimpleGuiControl* control;

	controlEvtArgs(ofxSimpleGuiControl* _control = NULL) {
		control = _control;
	}
};
*/

class ofxSimpleGuiControl : public ofxMSAInteractiveObject {
public:
	string		name;		// Human readable name this is what is displayed on screen (includes keyboard shortcut)
	string		origName;	// the original name (excluding keyboard shortcut)
	string		key;		// Machine readable name (don't change this after creating control, used for saving/loading)
	string		controlType;
	string		units;
	bool		lock;
	bool		focused;
	bool		newColumn;
	char		keyboardShortcut;

	ofxSimpleGuiControl(string name);
	virtual ofxSimpleGuiControl& setName(string newName);
	virtual ofxSimpleGuiControl& setKey(string newKey);
	virtual ofxSimpleGuiControl& setConfig(ofxSimpleGuiConfig *config);

	virtual ofxSimpleGuiControl& setTextColor(bool clickable = true);
	virtual ofxSimpleGuiControl& setTextBGColor(bool clickable = true);
	virtual ofxSimpleGuiControl& setFullColor(bool forceActive = false);
	virtual ofxSimpleGuiControl& setEmptyColor();
	virtual ofxSimpleGuiControl& setKeyboardShortcut(char c);
	virtual ofxSimpleGuiControl& setUnits(string units);

	virtual void loadFromXML(ofxXmlSettings &XML) {}
	virtual void saveToXML(ofxXmlSettings &XML) {}

	virtual void setup() {}

	void initChrome();
	virtual void resetChrome();

	void draw(float x, float y);
	void draw();

	virtual void drawWidget(float x, float y) {}
	virtual void drawWidget() { drawWidget(x, y); }

	virtual void onKeyUp() {}
	virtual void onKeyDown() {}
	virtual void onKeyLeft() {}
	virtual void onKeyRight() {}
	virtual void onKeyEnter() {}

	controlEvtArgs args;
	ofEvent<controlEvtArgs> valueChangedEvt;

	ofxSimpleGuiConfig	*config;
protected:

	short			textAlignment;
	enum			textAlignments
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER,
		ALIGN_JUSTIFIED,
	};

	enum ChromeTypes
	{
		BORDER_LEFT,
		BORDER_RIGHT,
		BORDER_TOP,
		BORDER_BOTTOM,
		FILL_MIDDLE,
		CAPTION_BORDER_LEFT,
		CAPTION_BORDER_RIGHT,
		CAPTION_BORDER_TOP,
		CAPTION_BORDER_BOTTOM,
		CAPTION_FILL_MIDDLE,
	};

	inline virtual map<int,ofImage>& chrome()
	{ return _chrome; }

private:
	static map<int,ofImage> _chrome;
};
