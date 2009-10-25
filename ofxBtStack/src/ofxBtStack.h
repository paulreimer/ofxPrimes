#pragma once

#include "ofMain.h"
#include "ofxThread.h"
#include "ofxMSAInteractiveObject.h"

extern "C" {
#include <btstack/utils.h>
}

class ofxBtStack : /*public ofxThread, */public ofxMSAInteractiveObject
{
public:
	ofxBtStack();
//	virtual ofxBtStack();
	void init();
	
	virtual void setup()	{};
	virtual void update()	{};
	virtual void draw()		{};
	
//	void threadedFunction();
//	void destroy();
	
	int fps;

	bd_addr_t addr;
	
	void packet_handler(uint8_t packet_type, uint8_t *packet, uint16_t size);
	
	virtual void newData	(uint8_t x, uint8_t y, uint8_t z) {};
	virtual void newState	(char *text) {};
};
