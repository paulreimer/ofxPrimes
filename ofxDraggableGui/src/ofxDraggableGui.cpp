#include "ofxDraggableGui.h"
#include "ofxPoint2f.h"	

//--------------------------------------------------------------
ofxDraggableGui::ofxDraggableGui(string name) : ofxSimpleGuiPage(name)
{
	bDraggable = false;
	
	disableAllEvents();
	enableKeyEvents();
	enableMouseEvents();
}

//--------------------------------------------------------------
void ofxDraggableGui::onPress(int x, int y, int button)
{
	if (!bDraggable)
		return;
	
	for (int i=0; i<controls.size(); i++)
		if (controls[i]->controlType!="Title"
			&&controls[i]->controlType!="Content"
			&& controls[i]->hitTest(x, y)) return;

	// save the offset of where the mouse was clicked...
	// ...relative to the position of the object
	saveX = x - this->x;
	saveY = y - this->y;
}

//--------------------------------------------------------------
void ofxDraggableGui::onDragOver(int x, int y, int button)
{
	if (!bDraggable)
		return;
	
	for (int i=0; i<controls.size(); i++)
		if (controls[i]->controlType!="Title"
			&&controls[i]->controlType!="Content"
			&&controls[i]->hitTest(x, y)) return;

	this->x = x - saveX;    // update x position
	this->y = y - saveY;    // update mouse y position
}

//--------------------------------------------------------------
void ofxDraggableGui::draw()
{
	if(!enabled) return;

	ofxSimpleGuiPage::draw(x, y, false);
 
	if (!bDraggable)
		return;	
	
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
void ofxDraggableGui::activate(bool isActivated)
{
	enabled = isActivated;
	if (isActivated)
		for (int i=0; i<controls.size(); i++)
			controls[i]->enableMouseEvents();		
	else
		for (int i=0; i<controls.size(); i++)
			controls[i]->disableMouseEvents();		
}
