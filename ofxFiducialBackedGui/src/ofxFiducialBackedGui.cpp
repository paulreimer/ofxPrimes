#pragma once

#include "ofxFiducialBackedGui.h"
#include "ofMain.h"

#define MAX_FIDUCIAL_LIFE 20

ofxFiducialBackedGui::ofxFiducialBackedGui(ofxFiducialTracker *tracker) : ofxSimpleGuiPage("Fiducial-Backed") {
	this->tracker = tracker;
	
	x_scale = 1.0;
	y_scale = 1.0;
	ofAddListener(ofEvents.update, (ofxMSAInteractiveObject*) this, &ofxMSAInteractiveObject::_update);
}

void ofxFiducialBackedGui::onPress(int x, int y, int button) {
	for(int i=0; i<controls.size(); i++)
	if (controls[i]->controlType!="Title"
			&&controls[i]->controlType!="Content"
			&&controls[i]->hitTest(x, y)) return;
	
	// save the offset of where the mouse was clicked...
	// ...relative to the position of the object
	saveX = x - this->x;
	saveY = y - this->y;
}

void ofxFiducialBackedGui::onDragOver(int x, int y, int button) {
	for(int i=0; i<controls.size(); i++)
		if (controls[i]->controlType!="Title"
			&&controls[i]->controlType!="Content"
			&&controls[i]->hitTest(x, y)) return;
	
	this->x = x - saveX;    // update x position
	this->y = y - saveY;    // update mouse y position
}

void ofxFiducialBackedGui::onDragOutside(int x, int y, int button) {
	saveX = 0;    // reset mouse x offset
	saveY = 0;    // reset mouse y offset
}

void ofxFiducialBackedGui::setup() {
}
void ofxFiducialBackedGui::update() {
#ifdef USE_FIDUCIALS
	list<ofxFiducial>::iterator fiducial;
	
	for (fiducial = tracker->fiducialsList.begin();
		 fiducial != tracker->fiducialsList.end();
		 fiducial++) {
		// delay creating a filter until somewhat stable
		if (fiducial->life == 2)
			if (fiducial->state != FIDUCIAL_LOST) {
				ofNotifyEvent(this->fiducialFoundEvt, *fiducial);
				fiducial->life += MAX_FIDUCIAL_LIFE;
			} else
				ofNotifyEvent(this->fiducialLostEvt, *fiducial);
			else if (fiducial->life < 2)
				continue;
		
		ofNotifyEvent(this->fiducialUpdatedEvt, *fiducial);
	}
#endif
}

void ofxFiducialBackedGui::draw() {
	if(!enabled) return;

	bool alignRight = false;
	
	if(alignRight) x = ofGetWidth() - x -  config->gridSize.x;
	
	float posX		= 0;
	float posY		= 0;
	float controlX, controlY;

	bool fiducialBacked;
	
	ofSetRectMode(OF_RECTMODE_CORNER);
	

	for(int i=0; i<controls.size(); i++) {
		ofxSimpleGuiControl &control = *controls[i];
		fiducialBacked = (control.fiducialId != NULL_FIDUCIAL_ID) && (control.enabled);
		
		if(control.newColumn) {
			if(alignRight) posX -= config->gridSize.x;
			else posX += config->gridSize.x;
			posY = 0;
		}
		
		if (!fiducialBacked) {
			controlX = posX + x;
			controlY = posY + y;
		} else {
			controlX = control.x;
			controlY = control.y;
		}
		
		control.draw(controlX, controlY);
		ofNoFill();
		ofSetColor(config->borderColor);
		
		ofEnableSmoothing();
		//		glLineWidth(0.1);
		ofRect(controlX, controlY, control.width, control.height);
		//		glLineWidth(1.0);
		ofDisableSmoothing();
		
		
		if (!fiducialBacked) {
			posY = getNextY(posY + control.height + config->padding.y);
			
			float addHeight = posY + control.height;
			if(addHeight > height
			   || y + addHeight > ofGetHeight()) {
				if(alignRight) posX -= config->gridSize.x;
				else posX += config->gridSize.x;
				posY = 0;
			}
		}
	}
	
	int border = 4;
	// add a border if mouse is pressed or over the object
	ofSetLineWidth(border);
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,127);
	if(isMouseDown()) {
		ofFill();
		for(int i=0; i<controls.size(); i++) {
			if (controls[i]->controlType!="Title"
				&&controls[i]->isMouseDown()) {
				ofNoFill();
				break;
			}
		}

		ofRect(x-(border/2), y-(border/2), width+border, height-config->padding.y+border);

	} else if(isMouseOver()) {
		ofNoFill();
		ofRect(x-(border/2), y-(border/2), width+border, height-config->padding.y+border);
	}

	//		ofRect(x, y, width, height - config->padding.y);
	ofDisableAlphaBlending();
	ofSetLineWidth(1.0);
}

ofxSimpleGuiControl &ofxFiducialBackedGui::addControl(ofxSimpleGuiControl& control) {
	control.enableMouseEvents();
	control.enableKeyEvents();

	if (control.fiducialId==NULL_FIDUCIAL_ID) {
		if (control.newColumn) {
			height = MAX(height, control.height);
			width += MAX(config->gridSize.x,control.width);
		} else {
			width = MAX(width,control.width);
			height += getNextY(control.height);
		}
	} else {
		control.enabled = false;
		
		ofAddListener(fiducialFoundEvt, this, &ofxFiducialBackedGui::fiducialFound);
		ofAddListener(fiducialLostEvt, this, &ofxFiducialBackedGui::fiducialLost);
		ofAddListener(fiducialUpdatedEvt, this, &ofxFiducialBackedGui::fiducialUpdated);
	}
	controls.push_back(&control);
	
	return control;
}

void ofxFiducialBackedGui::fiducialFound(ofxFiducial &fiducial) {
	int id=fiducial.getId();
	for(int i=0; i<controls.size(); i++)
		if (controls[i]->fiducialId = id)
			controls[i]->enabled = true;
}

void ofxFiducialBackedGui::fiducialLost(ofxFiducial &fiducial) {
	int id=fiducial.getId();
	for(int i=0; i<controls.size(); i++)
		if (controls[i]->fiducialId = id)
			controls[i]->enabled = false;
}

void ofxFiducialBackedGui::fiducialUpdated(ofxFiducial &fiducial) {
	int id=fiducial.getId();
	for(int i=0; i<controls.size(); i++) {
		if (controls[i]->fiducialId = id) {
			ofxSimpleGuiControl *control = controls[i];
			string controlType = control->controlType;

			control->setPos(fiducial.current.xpos*x_scale,
							fiducial.current.ypos*y_scale);
			
			float angle = fiducial.getAngle();
			
			if (controlType=="SliderInt") {
				ofxSimpleGuiSliderInt* control = dynamic_cast<ofxSimpleGuiSliderInt*>(controls[i]);
				*control->value = (int)ofMap(angle,
											 0.0,TWO_PI,
											 control->min,control->max);
			} else if (controlType=="SliderFloat") {
				ofxSimpleGuiSliderFloat* control = dynamic_cast<ofxSimpleGuiSliderFloat*>(controls[i]);
				*control->value = (float)ofMap(angle,
											   0.0,TWO_PI,
											   control->min,control->max);
			} else if (controlType=="SliderDouble") {
				ofxSimpleGuiSliderDouble* control = dynamic_cast<ofxSimpleGuiSliderDouble*>(controls[i]);
				*control->value = (double)ofMap(angle,
												0.0,TWO_PI,
												control->min,control->max);
			} else if (controlType=="SliderByte") {
				ofxSimpleGuiSliderByte* control = dynamic_cast<ofxSimpleGuiSliderByte*>(controls[i]);
				*control->value = (byte)ofMap(angle,
											  0.0,TWO_PI,
											  control->min,control->max);
			} else if (controlType=="Toggle") {
				ofxSimpleGuiToggle* control = dynamic_cast<ofxSimpleGuiToggle*>(controls[i]);
				*control->value = fabs(angle) > ofDegToRad(45.0);
			}
		}
	}
}

