#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxSimpleGuiPage.h"

class ofxDraggableGui : public ofxSimpleGuiPage {
public:
	ofxDraggableGui(string name="Draggable");

	virtual void onPress(int x, int y, int button);
	virtual void onDragOver(int x, int y, int button);

	void draw();

	float angle;
	void rotateRad(float angle);

	//bool hitTest(int tx, int ty); // handles translation and rotation
	bool translateEvents, rotateEvents;
	
	ofMouseEventArgs generateMouseEventArgs(float x, float y, int button);
protected:
	void relocateMouseEvent(ofMouseEventArgs &e);

	void translateMouse	(int& mx, int& my);
	void rotateMouse	(int& mx, int& my);
	int saveX, saveY;
};
