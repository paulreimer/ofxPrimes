#include "ofxDraggable.h"
ofxDraggable::ofxDraggable() {
  content = NULL;
}

void ofxDraggable::onPress(int mx, int my, int button) {
  // save the offset of where the mouse was clicked...
  // ...relative to the position of the object
  saveX = mx - this->x;
  saveY = my - this->y;
}

void ofxDraggable::onDragOver(int mx, int my, int button) {
  this->x = mx - saveX;    // update x position
  this->y = my - saveY;    // update mouse y position
}

void ofxDraggable::draw() {
  if(content) {
     width = content->getWidth();
     height = content->getHeight();
     content->draw(x, y, width, height);

     // add a border if mouse is pressed or over the object
     if(isMouseDown()) {
        ofNoFill();
        ofSetColor(0xFF0000);
        ofRect(x, y, width, height);
     } else if(isMouseOver()){
        ofNoFill();
        ofSetColor(0x00FF00);
        ofRect(x, y, width, height);
     }
  }
}

