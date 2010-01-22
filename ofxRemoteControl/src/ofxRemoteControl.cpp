#include "ofxRemoteControl.h"
#include "ofxTuioCursor.h"

#ifdef USE_TOON
using namespace TooN;
#endif

ofxRemoteControl::ofxRemoteControl(int port) {
	this->port = port;
	if (verbose) printf("ofxRemoteControl::ofxRemoteControl()\n");
}

ofxRemoteControl::~ofxRemoteControl() {
	if (verbose) printf("ofxRemoteControl::~ofxRemoteControl()\n");
	destroy();
}

void ofxRemoteControl::setup() {
#ifdef USE_MSAREMOTE
	msaRemote.start(port);
	
	ofAddListener(msaRemote.cursorAdded, this, &ofxRemoteControl::_cursorAdded);
	ofAddListener(msaRemote.cursorRemoved, this, &ofxRemoteControl::_cursorRemoved);
	ofAddListener(msaRemote.cursorUpdated, this, &ofxRemoteControl::_cursorUpdated);
#endif
	
	pos.rot.set		(0, 0, 0);
	pos.offset.set	(0, 0, 0);
//	pos.offset.set	(0.5f, 0.5f, 4.0f);
	
	touchesAnglePrev = touchesAngle = 0.0;
	touchesDistPrev = touchesDist = 0.0;
}

void ofxRemoteControl::update() {
#ifdef USE_MSAREMOTE
	if (!msaRemote.hasMessage()) return;
	
	msaRemote.getMessage();
	
	// invert x axis, and subtract 90 degrees
	pos.rot.x =	msaRemote.accelData.x*90;
	pos.rot.y = msaRemote.accelData.y*90;
	
	// do finger stuff
	list<ofxTuioCursor*>cursorList = msaRemote.getTuioCursors();
	list<ofxTuioCursor*>::iterator it = cursorList.begin();
	
	ofxTuioCursor*	first = *it;
	if (verbose) cout << cursorList.size() << " fingers" << endl;

	if (cursorList.size() == 2) {
		ofxTuioCursor* second = *(++it);
		touchesAngle = first->getAngleDegrees(second);
		touchesDist = first->getDistance(second);
		
		if (newCursorDetected) {
			newCursorDetected = false;
			touchesAnglePrev = touchesAngle;
			touchesDistPrev = touchesDist;
		}
		
		pos.rot.z += touchesAngle - touchesAnglePrev;
		pos.offset.z -= (touchesDist - touchesDistPrev)*4;
		
		touchesAnglePrev = touchesAngle;
		touchesDistPrev = touchesDist;
	} else if (cursorList.size() == 1) {
		pos.offset.x += first->getXSpeed() * tuioCursorSpeedMult;
		pos.offset.y += first->getYSpeed() * tuioCursorSpeedMult;
	}
#endif
}

#ifdef USE_TOON
SE3<> ofxRemoteControl::se3FromRemote() const {
	/*	from http:/www.j3d.org/matrix_faq/matrfaq_latest.html
	 
		|  CE      -CF       D  |
	 M =|  BDE+AF  -BDF+AE  -BC |
		| -ADE+BF   ADF+BE   AC |
	 
	 This is the final rotation matrix.
	 
	 The individual values of A,B,C,D,E and F are evaluated first.
	 */	
	ofPoint rotation = pos.rot;
	double cosX = cos(ofDegToRad(rotation.x));
	double sinX = sin(ofDegToRad(rotation.x));
	double cosY = cos(ofDegToRad(rotation.y));
	double sinY = sin(ofDegToRad(rotation.y));
	double cosZ = cos(ofDegToRad(rotation.z));
	double sinZ = sin(ofDegToRad(rotation.z));
	/*
	 double rotCoeffs[3][3] = {
	 { cosY*cosZ,				  -cosY*sinZ,				    sinY		},
	 { sinX*sinY*cosZ + cosX*sinZ, -sinX*sinY*sinZ + cosX*cosZ, -sinX*cosY	},
	 {-cosX*sinY*cosZ + sinX*sinZ,  cosX*sinY*sinZ + sinX*cosZ,  cosX*cosY	}
	 };
	 */	
	Matrix<3,3> rotMat;//(rotCoeffs);

	rotMat[0][0] =  cosY*cosZ;
	rotMat[0][1] = -cosY*sinZ;
	rotMat[0][2] =  sinY;

	rotMat[1][0] =  sinX*sinY*cosZ + cosX*sinZ;
	rotMat[1][1] = -sinX*sinY*sinZ + cosX*cosZ;
	rotMat[1][2] = -sinX*cosY;

	rotMat[2][0] = -cosX*sinY*cosZ + sinX*sinZ;
	rotMat[2][1] =  cosX*sinY*sinZ + sinX*cosZ;
	rotMat[2][2] =  cosX*cosY;

	ofPoint translation = pos.offset;
	Vector<3> transVect = makeVector(translation.x, translation.y, translation.z);

//	SE3<> worldFromRemote(transVect, rotMat);
//	worldFromRemote.get_rotation() = SO3<>(Matrix<3>(rotMat));
//	worldFromRemote.get_translation() = Vector<3>(transVect);
//	return worldFromRemote;
	return (SE3<>(SO3<>(rotMat), transVect));
}
#endif

void ofxRemoteControl::destroy() {
	if (verbose) printf("ofxRemoteControl::destroy()\n");
}

//--------------------------------------------------------------
void ofxRemoteControl::_cursorAdded(ofxTuioCursor &tuioCursor) {
	if (tuioCursor.getFingerId() < 2) {
		newCursorDetected = true;
	}
	cursorAdded(tuioCursor);
}


//--------------------------------------------------------------
void ofxRemoteControl::_cursorRemoved(ofxTuioCursor &tuioCursor) {
	cursorRemoved(tuioCursor);
}

//--------------------------------------------------------------
void ofxRemoteControl::_cursorUpdated(ofxTuioCursor &tuioCursor) {
	cursorUpdated(tuioCursor);
}
