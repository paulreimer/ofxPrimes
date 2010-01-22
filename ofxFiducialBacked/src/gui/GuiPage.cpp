#include "GuiPage.h"
#include "ofxPoint2f.h"

namespace ofxFiducialBacked {
namespace gui {
//--------------------------------------------------------------
GuiPage::GuiPage(string name)
: ofxSimpleGuiPage(name)
{
	enableAppEvents();

	angle = 0.0;
	scale = 1.0;
	offset.set(0, 0);

	bWrapControls = false;
}

//--------------------------------------------------------------
void
GuiPage::setup()
{
	activate(true);
}

//--------------------------------------------------------------
void GuiPage::setOffset(ofPoint offset)
{
	this->offset = offset;
}

//--------------------------------------------------------------
void GuiPage::setAngle(float angle)
{
	this->angle = angle;
}

//--------------------------------------------------------------
void GuiPage::setScale(float scale)
{
	this->scale = scale;
}

//--------------------------------------------------------------
void
GuiPage::update()
{}

//--------------------------------------------------------------
void
GuiPage::draw()
{
	if(!enabled) return;
	
	if (x == FIDUCIAL_INVALID_COORDS && y == FIDUCIAL_INVALID_COORDS)
		return;

	glPushMatrix();
	
	glTranslatef(x,	y, 0.0);
	glRotatef	(ofRadToDeg(angle), 0.0, 0.0, 1.0);
	glTranslatef(offset.x, offset.y, 0.0);
	glScalef	(scale, scale, 1.0);

	ofxSimpleGuiPage::draw(0,0, false);

	glPopMatrix();
}


//--------------------------------------------------------------
ofxSimpleGuiControl&
GuiPage::addControl(ofxSimpleGuiControl& control) {
	ofxSimpleGuiPage::addControl(control);
	control.disableMouseEvents();
	return control;
}

//--------------------------------------------------------------
void
GuiPage::relocateMouseEvent(ofMouseEventArgs& e)
{
	ofxPoint2f _new(e.x, e.y);
	ofxPoint2f ref(x, y);
	
	_new.rotateRad(-angle, ref);

	e.x = (_new.x - x - offset.x)/scale;
	e.y = (_new.y - y - offset.y)/scale;
}

//--------------------------------------------------------------
void
GuiPage::_mouseMoved(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);

	ofxMSAInteractiveObject::_mouseMoved(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mouseMoved(e_copy);
}

//--------------------------------------------------------------
void
GuiPage::_mousePressed(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);

	ofxMSAInteractiveObject::_mousePressed(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mousePressed(e_copy);

	// save the offset of where the mouse was clicked...
	// ...relative to the position of the object
	bool draggable = false;
	for(int i=0; i<controls.size(); i++) {
		if (controls[i]->controlType == "Content"
			&& controls[i]->isMouseDown()) {
			draggable = true;
			break;
		}
	}
	if (draggable) {
		saveX = e_copy.x;
		saveY = e_copy.y;
	}
}

//--------------------------------------------------------------
void
GuiPage::_mouseDragged(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);

	ofxMSAInteractiveObject::_mouseDragged(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mouseDragged(e_copy);

	bool draggable = false;
	for(int i=0; i<controls.size(); i++) {
		if (controls[i]->controlType == "Content"
			&& controls[i]->isMouseDown()) {
			draggable = true;
			break;
		}
	}
	
	if (draggable)
		offset += ofPoint(e_copy.x - saveX, e_copy.y - saveY);
}

//--------------------------------------------------------------
void
GuiPage::_mouseReleased(ofMouseEventArgs &e)
{
	ofMouseEventArgs e_copy(e);
	relocateMouseEvent(e_copy);

	ofxMSAInteractiveObject::_mouseReleased(e_copy);
	for (int i=0; i<controls.size(); i++)
		controls[i]->_mouseReleased(e_copy);
}

//--------------------------------------------------------------
bool
GuiPage::hitTest(int tx, int ty)
{
	return ((tx > x) && (tx < x + width) && (ty > y) && (ty < y + height));
//	return ((tx > 0) && (tx < scale*width) && (ty > 0) && (ty < scale*height));
}
	
//--------------------------------------------------------------
void
GuiPage::activate(bool isActivated)
{
	enabled = isActivated;
	if (isActivated)
	{
		ofAddListener(ofEvents.mousePressed,		this, &GuiPage::_mousePressed);
		ofAddListener(ofEvents.mouseMoved,			this, &GuiPage::_mouseMoved);
		ofAddListener(ofEvents.mouseDragged,		this, &GuiPage::_mouseDragged);
		ofAddListener(ofEvents.mouseReleased,		this, &GuiPage::_mouseReleased);
	}
	else
	{
		ofRemoveListener(ofEvents.mousePressed,		this, &GuiPage::_mousePressed);
		ofRemoveListener(ofEvents.mouseMoved,		this, &GuiPage::_mouseMoved);
		ofRemoveListener(ofEvents.mouseDragged,		this, &GuiPage::_mouseDragged);
		ofRemoveListener(ofEvents.mouseReleased,	this, &GuiPage::_mouseReleased);
	}
}
	
} //gui
} //ofxFiducialBacked
