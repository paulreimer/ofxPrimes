#pragma once

#include "ofxFiducialBackedGui.h"
#include "ofxPoint2f.h"

//--------------------------------------------------------------
ofxFiducialBackedGui::ofxFiducialBackedGui(string name)
: ofxSimpleGuiPage(name)
{
	enableAppEvents();
	
	angle	= 0.0;
	size	= 1.0;
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::setup()
{
	activate(true);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::update()
{
	offset.set(-(size/2)-16, (size/2)+16, 0.0);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::draw()
{
	if(!enabled) return;
	
	if (x == FIDUCIAL_INVALID_COORDS && y == FIDUCIAL_INVALID_COORDS)
		return;

	float scaleFac = size/100;

	glPushMatrix();
	
	glTranslatef(x,	y, 0.0);
	glRotatef	(ofRadToDeg(angle), 0.0, 0.0, 1.0);
	glTranslatef(offset.x, offset.y, 0.0);
	glScalef	(scaleFac, scaleFac, 1.0);

	ofxSimpleGuiPage::draw(0,0, false);
	
	glPopMatrix();	
}


//--------------------------------------------------------------
ofxSimpleGuiControl&
ofxFiducialBackedGui::addControl(ofxSimpleGuiControl& control) {
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

//--------------------------------------------------------------
void
ofxFiducialBackedGui::relocateMouseEvent(ofMouseEventArgs& e)
{
	float scaleFac = size/100;
	
	ofxPoint2f _new(e.x, e.y);
	ofxPoint2f ref(x, y);
	
	_new.rotateRad(-angle, ref);

	e.x = (_new.x - x - offset.x)/scaleFac;
	e.y = (_new.y - y - offset.y)/scaleFac;
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::_mouseMoved(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mouseMoved(e_copy);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::_mousePressed(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mousePressed(e_copy);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::_mouseDragged(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mouseDragged(e_copy);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::_mouseReleased(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mouseReleased(e_copy);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGui::activate(bool isActivated)
{
	enabled = isActivated;
	if (isActivated)
	{
		ofAddListener(ofEvents.mousePressed,		this, &ofxFiducialBackedGui::_mousePressed);
		ofAddListener(ofEvents.mouseMoved,			this, &ofxFiducialBackedGui::_mouseMoved);
		ofAddListener(ofEvents.mouseDragged,		this, &ofxFiducialBackedGui::_mouseDragged);
		ofAddListener(ofEvents.mouseReleased,		this, &ofxFiducialBackedGui::_mouseReleased);
	}
	else
	{
		ofRemoveListener(ofEvents.mousePressed,		this, &ofxFiducialBackedGui::_mousePressed);
		ofRemoveListener(ofEvents.mouseMoved,		this, &ofxFiducialBackedGui::_mouseMoved);
		ofRemoveListener(ofEvents.mouseDragged,		this, &ofxFiducialBackedGui::_mouseDragged);
		ofRemoveListener(ofEvents.mouseReleased,	this, &ofxFiducialBackedGui::_mouseReleased);
	}
}
