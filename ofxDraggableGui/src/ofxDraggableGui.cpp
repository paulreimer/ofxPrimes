#pragma once

#include "ofxDraggableGui.h"
#include "ofxPoint2f.h"	

ofxDraggableGui::ofxDraggableGui(string name) : ofxSimpleGuiPage(name)
{
	angle = 0;
	
	rotateEvents = false;
	translateEvents = false;
	
	disableAllEvents();
	enableKeyEvents();
	enableMouseEvents();
}

void ofxDraggableGui::onPress(int x, int y, int button)
{
	for (int i=0; i<controls.size(); i++)
		if (controls[i]->controlType!="Title"
			&&controls[i]->controlType!="Content"
			&& controls[i]->hitTest(x, y)) return;

	// save the offset of where the mouse was clicked...
	// ...relative to the position of the object
	saveX = x - this->x;
	saveY = y - this->y;
}

void ofxDraggableGui::onDragOver(int x, int y, int button)
{
	for (int i=0; i<controls.size(); i++)
		if (controls[i]->controlType!="Title"
			&&controls[i]->controlType!="Content"
			&&controls[i]->hitTest(x, y)) return;

	this->x = x - saveX;    // update x position
	this->y = y - saveY;    // update mouse y position
}

void ofxDraggableGui::draw()
{
	if(!enabled) return;

	glPushMatrix();
	
	glTranslatef( width/2,  height/2, 0.0);
	glRotatef	(ofRadToDeg(angle), 0.0, 0.0, 1.0);
	glTranslatef(-width/2, -height/2, 0.0);
	
	ofxSimpleGuiPage::draw(x, y, false);
 
	int border = 4;
	// add a border if mouse is pressed or over the object
	ofSetLineWidth(border);
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,127);
	if(isMouseDown())
	{
		ofFill();
		for (int i=0; i<controls.size(); i++) {
			if (controls[i]->controlType!="Title"
				&&controls[i]->isMouseDown())
			{
				ofNoFill();
				break;
			}
		}

		ofRect(x-(border/2), y-(border/2), width+border, height-config->padding.y+border);

	}
	else if(isMouseOver())
	{
		ofNoFill();
		ofRect(x-(border/2), y-(border/2), width+border, height-config->padding.y+border);
	}

	//		ofRect(x, y, width, height - config->padding.y);
	ofDisableAlphaBlending();
	ofSetLineWidth(1.0);
	
	glPopMatrix();
}

//--------------------------------------------------------------
void ofxDraggableGui::rotateRad(float angle)
{
	this->angle = angle;
}
/*
bool ofxDraggableGui::hitTest(int tx, int ty)
 {
	if (rotateEvents)	{ rotateMouse(tx, ty); }
	if (translateEvents){ translateMouse(tx, ty); }
	
	return ((tx > x) && (tx < x + width) && (ty > y) && (ty < y + height));
}
*/
void ofxDraggableGui::translateMouse(int& mx, int& my)
{
	mx -= x;
	my -= y;
}

void ofxDraggableGui::rotateMouse(int& mx, int& my)
{
	ofxPoint2f _new(mx, my);
	ofxPoint2f _ref(x+width/2, y+height/2);
	//ofxPoint2f _ref(x, y);
	_new.rotateRad(-angle, _ref);
	
	mx = _new.x;
	my = _new.y;
}

void ofxDraggableGui::relocateMouseEvent(ofMouseEventArgs& e)
{
	if (rotateEvents)
		rotateMouse(e.x, e.y);
	
	if (translateEvents)
		translateMouse(e.x, e.y);
}

ofMouseEventArgs ofxDraggableGui::generateMouseEventArgs(float x, float y, int button)
{
	ofMouseEventArgs args;
	args.x = x;
	args.y = y;
	args.button = button;
	
	relocateMouseEvent(args);

	return args;
}