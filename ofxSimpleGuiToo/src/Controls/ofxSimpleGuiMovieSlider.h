#pragma once

#include "ofxSimpleGuiControl.h"

#include "ofxSimpleGuiButton.h"

//------------------------------------------------------------------------------ Movie slider
class ofxSimpleGuiMovieSlider : public ofxSimpleGuiControl
{
public:
	float				pct;
	float				sliderPos;
	float*				value;
	float				barwidth;
#ifndef TARGET_OF_IPHONE
	ofVideoPlayer*		input;
#else
	void*				input;
#endif
	float				videoLength;

	ofxSimpleGuiButton*		pauseBtn;
	ofxSimpleGuiButton*		cue1Btn;
	ofxSimpleGuiButton*		cue2Btn;
	ofxSimpleGuiButton*		resetBtn;

	bool				doPause;
	bool				cue_1, cue_2, resetCue;
	float				cuePoint1, cuePoint2;

#ifndef TARGET_OF_IPHONE
	ofxSimpleGuiMovieSlider(string name, ofVideoPlayer* input);
#else
	ofxSimpleGuiMovieSlider(string name, void* input);
#endif

	void update();

	void onDragOver(int x, int y, int buton);
	void onPress(int x, int y, int button);
	void onRelease();

	void drawWidget(float x, float y);
};
