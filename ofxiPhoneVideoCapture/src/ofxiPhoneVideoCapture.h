#pragma once

#include "ofxMSAInteractiveObject.h"

class ofxiPhoneVideoCapture : public ofxMSAInteractiveObject
{
public:
	ofxiPhoneVideoCapture();
	virtual ~ofxiPhoneVideoCapture();
	
	void setup();
	void destroy();

	void capture();
	
	virtual void update()	{};
	virtual void draw()		{};

	ofImage input;
	
	bool bThreaded;
};
