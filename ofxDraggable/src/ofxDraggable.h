#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxSimpleGuiToo.h"

class ofxDraggable : public ofxMSAInteractiveObject {
public:
   ofBaseDraws *content; // this will point to your image, video, grabber etc.

   ofxDraggable();

   void onPress(int mx, int my, int button);
   void onDragOver(int mx, int my, int button);

   void draw();

  protected:
   int saveX, saveY;
};
