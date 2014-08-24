#include "ofxSimpleGuiMovieSlider.h"

#ifndef TARGET_OF_IPHONE
//---------------------------------------------------------------------
ofxSimpleGuiMovieSlider::ofxSimpleGuiMovieSlider(string name, ofVideoPlayer* input)
: ofxSimpleGuiControl(name)
{
	setSize(config->gridSize.x - config->margin.x, config->sliderHeight);

	barwidth  = 0;
	this->input	= input;

	//init
	if(input)
		videoLength = input->getDuration();

	//Buttons
	doPause = false;
	pauseBtn = new ofxSimpleGuiButton("Pause", doPause);
	cue1Btn  = new ofxSimpleGuiButton("Cue 1", cue_1);
	cue2Btn  = new ofxSimpleGuiButton("Cue 2", cue_2);
	resetBtn = new ofxSimpleGuiButton("Reset", resetCue);

	pauseBtn->setMomentary(false);
	resetCue = false;
	cue_1 = false;
	cue_2 = false;
	cuePoint1 = 0.0;
	cuePoint2 = 1.0;
	controlType = "MovieSlider";
	setup();
}

//---------------------------------------------------------------------
void
ofxSimpleGuiMovieSlider::update()
{

	if(!enabled) return;

	if(!input) return;
	if(!lock) barwidth = ofMap(input->getPosition(), 0.0, 1.0, 0.0, (float)width);

	if(resetCue) {
		cuePoint1 = 0.0;
		cuePoint2 = 1.0;
		input->setPosition(0.0);
		resetCue = false;
	}
	if(cue_1) {
		cuePoint1 = input->getPosition();
		cue_1 = false;
	}
	if(cue_2) {
		cuePoint2 = input->getPosition();
		cue_2 = false;
	}
	if(!lock) {
		if(input->getPosition() >= cuePoint2) {
			input->setPosition(cuePoint1);
		}
		else if(input->getPosition() <= cuePoint1) {
			input->setPosition(cuePoint1);
		}
	}
	enabled = false;
}

//---------------------------------------------------------------------
void
ofxSimpleGuiMovieSlider::onDragOver(int x, int y, int buton)
{
	if(lock) {
		//cuePoint1 = 0.0;
		//cuePoint2 = 1.0;

		barwidth = x - this->x;
		if(barwidth <= 0) barwidth = 0;
		if(barwidth >= width) barwidth = width;

		input->setPaused(true);
		input->setPosition(ofMap(barwidth, 0.0, (float)width, 0.0, 1.0));
	}
}

//---------------------------------------------------------------------
void
ofxSimpleGuiMovieSlider::onPress(int x, int y, int button)
{
	//cuePoint1 = 0.0;
	//cuePoint2 = 1.0;
	lock = true;
	input->setPaused(true);
	barwidth = x - this->x;
	input->setPaused(true);
	input->setPosition(ofMap(barwidth, 0.0, (float)width, 0.0, 1.0));
}

//---------------------------------------------------------------------
void
ofxSimpleGuiMovieSlider::onRelease()
{
	lock = false;
	input->play();
	input->setPaused(doPause);
}

//---------------------------------------------------------------------
void
ofxSimpleGuiMovieSlider::drawWidget(float x, float y)
{

	enabled = true;
	//update postion of gui object
	setPos(x, y);

	glPushMatrix();
	glTranslatef(x, y, 0);
	ofEnableAlphaBlending();
	ofFill();
	//		if(isMouseOver()) ofSetColor(config->overColor.r, config->overColor.g, config->overColor.b);
	//		if(focused && !isMouseOver()) ofSetColor(config->focusColor.r, config->focusColor.g, config->focusColor.b);

	setFullColor();
	ofRect(0, 0, barwidth, height);

	ofSetColor(config->textColor);
	float inputpos = ofMap(input->getPosition(), 0.0, 1.0, 0.0, videoLength);
	ostringstream info;
	info << name << ":" << ofToString(inputpos, 3) << "/" << ofToString(videoLength, 3) << endl;
	// TODO:
	//ofDrawBitmapString(info.str(), config->fontOffset.x, height+config->fontOffset.y);
	config->font.drawString(info.str(), config->fontOffset.x, height+config->fontOffset.y);

	// cues
	//		if(cuePoint1 > 0.0) {
	//			ofSetColor(config->overColor.r, config->overColor.g, config->overColor.b, 200);
	//			ofRect(ofMap(cuePoint1, 0.0, 1.0, 0.0, width), 0, 1, height);
	//		}
	//		if(cuePoint2 < 1.0) {
	//			ofSetColor(config->overColor.r, config->overColor.g, config->overColor.b, 200);
	//			ofRect(ofMap(cuePoint2, 0.0, 1.0, 0.0, width), 0, 1, height);
	//		}

	ofDisableAlphaBlending();
	glPopMatrix();



	// a bit of a hack but for simple no images to load :)
	pauseBtn->draw(x, y+35);
	cue1Btn->draw(x+23, y+35);
	cue2Btn->draw(x+46, y+35);
	resetBtn->draw(x+69, y+35);

	ofSetColor(0xffffff);
	config->font.drawString("1", x+30, y+49);
	config->font.drawString("2", x+53, y+49);
	config->font.drawString("R", x+76, y+49);
	/*
	 ofDrawBitmapString("1", x+30, y+49);
	 ofDrawBitmapString("2", x+53, y+49);
	 ofDrawBitmapString("R", x+76, y+49);
	 */
	ofSetColor(0xffffff);
	ofFill();

	if(!doPause) {
		ofRect(x+6, y+38, 2, 14);
		ofRect(x+13, y+38, 2, 14);
	}
	else if(doPause){
		glPushMatrix();
		glTranslatef(x+6, y+37, 0);
		ofTriangle(0, 0, 0, 16, 8, 8);
		glPopMatrix();
	}
}
#endif