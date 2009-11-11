#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxSimpleGuiPage.h"

class ofxDraggableGui : public ofxSimpleGuiPage {
public:
	ofxDraggableGui(string name="Draggable");

	virtual void onPress(int x, int y, int button);
	virtual void onDragOver(int x, int y, int button);

	void draw();
	void activate(bool isActivated = true);

	bool bDraggable;

protected:
	int saveX, saveY;
};
