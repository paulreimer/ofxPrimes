#pragma once

#include "ofMain.h"

#include "ofxTuioCursor.h"

#include "ofxMSAInteractiveObject.h"

#ifndef USE_MSAREMOTE
#define USE_MSAREMOTE
#endif

#ifndef USE_TOON
#undef	USE_TOON
#endif

#define DEFAULT_PORT 3333
#define tuioCursorSpeedMult	0.01f
#define tuioCursorDistMult	1.0f

#ifdef USE_MSAREMOTE
#include "msaRemoteClient.h"
#endif

#ifdef USE_TOON
#include <TooN/TooN.h>
#include <TooN/se3.h>
#endif

class ofxRemoteControl : public ofxMSAInteractiveObject {
public:
	ofxRemoteControl(int port = DEFAULT_PORT);
	virtual ~ofxRemoteControl();
	
	void setup();
	void update();
	
	void destroy();

	virtual void cursorAdded	(const ofxTuioCursor &cursor) {};
	virtual void cursorRemoved	(const ofxTuioCursor &cursor) {};
	virtual void cursorUpdated	(const ofxTuioCursor &cursor) {};
	
#ifdef USE_TOON
	TooN::SE3<> se3FromRemote() const;
#endif

	struct RemotePosition {
		ofPoint rot;
		ofPoint offset;
	} pos;
	
protected:
	void _cursorAdded			(ofxTuioCursor& tuioCursor);
	void _cursorRemoved			(ofxTuioCursor& tuioCursor);
	void _cursorUpdated			(ofxTuioCursor& tuioCursor);

	int port;
	
#ifdef USE_MSAREMOTE
	msaRemoteClient msaRemote;
#endif

	struct RemoteSettings {
		float	lerpSpeed;
	} settings;
	
	float touchesAnglePrev, touchesAngle;
	float touchesDistPrev, touchesDist;
	bool newCursorDetected;
	bool bClamp;
};
