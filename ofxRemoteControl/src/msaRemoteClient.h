#include "ofxOsc.h"

#include "ofxTuioCursor.h"
#include "ofxTuioObject.h"

// default port 3333
#define PORT 3333

#define PORTRAIT		0
#define LANDSCAPE		1

#define ON_THE_BOTTOM	0
#define ON_THE_TOP		1
#define ON_THE_LEFT		2
#define ON_THE_RIGHT	3


//--------------------------------------------------------
class msaRemoteClient {
public:
	msaRemoteClient(){
		currentFrame = lastFrame = maxFingerID = -1;
		lerpSpeed = 0.095;
		accelData = 0;
	};

	void start(int _port){
		receiver.setup( _port );
	};

	void start(){
		receiver.setup( PORT );
	};

	bool hasMessage() {
		return receiver.hasWaitingMessages();
	}

	void getMessage(){

		while( receiver.hasWaitingMessages() )
		{
			ofxOscMessage m;
			receiver.getNextMessage( &m );

			if ( m.getAddress() == "/tuio/2Dobj" )
			{

				if( m.getArgAsString( 0 ) == "set" ){
					if ((currentFrame<lastFrame) && (currentFrame>0)) return;

					long sid  = (long)(m.getArgAsInt32(1));
					int fiducial = m.getArgAsInt32(2);
					float xpos =  m.getArgAsFloat(3);
					float ypos =  m.getArgAsFloat(4);
					float angle =  m.getArgAsFloat(5);
					float xspeed =  m.getArgAsFloat(6);
					float yspeed =  m.getArgAsFloat(7);
					float rspeed =  m.getArgAsFloat(8);
					float maccel =  m.getArgAsFloat(9);
					float raccel =  m.getArgAsFloat(10);

					list<ofxTuioObject*>::iterator tobj;
					for (tobj=objectList.begin(); tobj != objectList.end(); tobj++)
						if((*tobj)->getSessionId()==sid) break;

					if (tobj == objectList.end()) {
						ofxTuioObject *addObject = new ofxTuioObject(sid, fiducial, xpos, ypos, angle);
						objectList.push_back(addObject);


						ofNotifyEvent(objectAdded, *(objectList.back()), this);

					}else if ( ((*tobj)->getX()!=xpos) || ((*tobj)->getY()!=ypos) || ((*tobj)->getAngle()!=angle) || ((*tobj)->getXSpeed()!=xspeed) || ((*tobj)->getYSpeed()!=yspeed) || ((*tobj)->getRotationSpeed()!=rspeed) || ((*tobj)->getMotionAccel()!=maccel) || ((*tobj)->getRotationAccel()!=raccel) ) {
						(*tobj)->update(xpos,ypos,angle,xspeed,yspeed,rspeed,maccel,raccel);

						ofNotifyEvent(objectUpdated, **tobj, this);
					}
				} else if( m.getArgAsString( 0 ) == "alive"  ){
					if ((currentFrame<lastFrame) && (currentFrame>0)) return;
					for (int i=1;i<m.getNumArgs();i++) {

						long sid  = (long)(m.getArgAsInt32(i));;
						newObjectList.push_back((long)(sid));

						list<long>::iterator iter;
						iter = find(aliveObjectList.begin(), aliveObjectList.end(), (long)(sid));
						if (iter != aliveObjectList.end()) aliveObjectList.erase(iter);
					}
					list<long>::iterator alive_iter;
					for (alive_iter=aliveObjectList.begin(); alive_iter != aliveObjectList.end(); alive_iter++) {
						list<ofxTuioObject*>::iterator tobj;
						for (tobj=objectList.begin(); tobj!=objectList.end(); tobj++) {
							ofxTuioObject *deleteObject = (*tobj);
							if(deleteObject->getSessionId()==*alive_iter) {

								ofNotifyEvent(objectRemoved, **tobj, this);

								objectList.erase(tobj);

								delete deleteObject;
								break;
							}
						}
					}
					aliveObjectList = newObjectList;
					newObjectList.clear();
				} else if( m.getArgAsString( 0 ) == "fseq" ){

					if(currentFrame>0) lastFrame = currentFrame;
					currentFrame  = (int)(m.getArgAsInt32(1));
				}
			} else if ( m.getAddress() == "/tuio/2Dcur" )
			{
				if( m.getArgAsString( 0 ) == "set" ){
					if ((currentFrame<lastFrame) && (currentFrame>0)) return;

					long sid  = (long)(m.getArgAsInt32(1));
					float xpos = m.getArgAsFloat(2);
					float ypos =  m.getArgAsFloat(3);
					float xspeed =  m.getArgAsFloat(4);
					float yspeed =  m.getArgAsFloat(5);
					float maccel =  m.getArgAsFloat(6);

					list<ofxTuioCursor*>::iterator tit;
					for (tit=cursorList.begin(); tit != cursorList.end(); tit++)
						if((*tit)->getSessionId()==sid) break;

					if (tit == cursorList.end()) {
						int fid = (int)(cursorList.size());

						if ((int)(cursorList.size())<=maxFingerID) {
							list<ofxTuioCursor*>::iterator closestCursor = freeCursorList.begin();

							for(list<ofxTuioCursor*>::iterator testCursor = freeCursorList.begin();testCursor!= freeCursorList.end(); testCursor++) {
								if((*testCursor)->getDistance(xpos,ypos)<(*closestCursor)->getDistance(xpos,ypos)) closestCursor = testCursor;
							}

							fid = (*closestCursor)->getFingerId();
							freeCursorList.erase(closestCursor);
							delete *closestCursor;
						} else maxFingerID = fid;

						ofxTuioCursor *addCursor = new ofxTuioCursor(sid,fid,xpos,ypos);
						cursorList.push_back(addCursor);

						ofNotifyEvent(cursorAdded, *(cursorList.back()), this);

					} else if ( ((*tit)->getX()!=xpos) || ((*tit)->getY()!=ypos) || ((*tit)->getXSpeed()!=xspeed) || ((*tit)->getYSpeed()!=yspeed) || ((*tit)->getMotionAccel()!=maccel) ) {
						(*tit)->update(xpos,ypos,xspeed,yspeed,maccel);

						ofNotifyEvent(cursorUpdated, **tit, this);
					}
				} else if( m.getArgAsString( 0 ) == "alive" ){
					if ((currentFrame<lastFrame) && (currentFrame>0)) return;
					for (int i=1;i<m.getNumArgs();i++) {

						long sid  = (long)(m.getArgAsInt32(i));;
						newCursorList.push_back((long)(sid));

						list<long>::iterator iter;
						iter = find(aliveCursorList.begin(), aliveCursorList.end(), (long)(sid));
						if (iter != aliveCursorList.end()) aliveCursorList.erase(iter);
					}
					list<long>::iterator alive_iter;
					for (alive_iter=aliveCursorList.begin(); alive_iter != aliveCursorList.end(); alive_iter++) {
						list<ofxTuioCursor*>::iterator tit;
						for (tit=cursorList.begin(); tit != cursorList.end(); tit++) {
							ofxTuioCursor *deleteCursor = (*tit);
							if(deleteCursor->getSessionId()==*alive_iter) {

								ofNotifyEvent(cursorRemoved, **tit, this);

								cursorList.erase(tit);

								if (deleteCursor->getFingerId()==maxFingerID) {
									maxFingerID = -1;
									delete deleteCursor;

									if (cursorList.size()>0) {
										list<ofxTuioCursor*>::iterator clist;
										for (clist=cursorList.begin(); clist != cursorList.end(); clist++) {
											int fid = (*clist)->getFingerId();
											if (fid>maxFingerID) maxFingerID=fid;
										}

										list<ofxTuioCursor*>::iterator flist;
										for (flist=freeCursorList.begin(); flist != freeCursorList.end(); flist++) {
											ofxTuioCursor *freeCursor = (*flist);
											if (freeCursor->getFingerId()>maxFingerID) delete freeCursor;
											else freeCursorBuffer.push_back(freeCursor);
										}

										freeCursorList = freeCursorBuffer;
										freeCursorBuffer.clear();
									}
								} else if (deleteCursor->getFingerId()<maxFingerID) freeCursorList.push_back(deleteCursor);

								break;
							}
						}

					}
					aliveCursorList = newCursorList;
					newCursorList.clear();
				} else if( m.getArgAsString( 0 ) == "fseq" ){

					if(currentFrame>0) lastFrame = currentFrame;
					currentFrame  = (int)(m.getArgAsInt32(1));
				}
			} else if ( m.getAddress() == "/msaremote/accelerometer" ) {
				if( m.getNumArgs() == 3 ) {
					accelData.x = ofLerp(accelData.x, m.getArgAsFloat(0), lerpSpeed);
					accelData.y = ofLerp(accelData.y, m.getArgAsFloat(1), lerpSpeed);
					accelData.z = ofLerp(accelData.z, m.getArgAsFloat(2), lerpSpeed);

					remoteOrientation = (fabs(accelData.y) > fabs(accelData.x))? PORTRAIT : LANDSCAPE;
					if (remoteOrientation == LANDSCAPE)
						remoteHomeButton = (accelData.x < 0)? ON_THE_RIGHT : ON_THE_LEFT;
					else
						remoteHomeButton = (accelData.y < 0)? ON_THE_BOTTOM: ON_THE_TOP;
				}
			}
		}
	};

	void drawCursors() const {
		list<ofxTuioCursor*>::const_iterator tit;
		for (tit=cursorList.begin(); tit != cursorList.end(); tit++) {
			ofxTuioCursor *blob = (*tit);

			glColor3f(1.0,1.0,1.0);
			ofEllipse(blob->getX()*ofGetWidth(), blob->getY()*ofGetHeight(), 10.0, 10.0);
			string str = "SessionId: "+ofToString((int)(blob->getSessionId()));
			ofDrawBitmapString(str, blob->getX()*ofGetWidth()-10.0, blob->getY()*ofGetHeight()+25.0);
		}
	};

	void drawObjects() const {

		list<ofxTuioObject*>::const_iterator tobj;
		for (tobj=objectList.begin(); tobj != objectList.end(); tobj++) {
			ofxTuioObject *blob = (*tobj);
			glColor3f(1.0,0.0,0.0);
			glPushMatrix();
			glTranslatef(blob->getX()*ofGetWidth(), blob->getY()*ofGetHeight(), 0.0);
			glRotatef(blob->getAngleDegrees(), 0.0, 0.0, 1.0);
			ofRect(-10.0, -10.0, 20.0, 20.0);
			glColor3f(1.0,1.0,1.0);
			ofLine(0, 0, 0, 10);
			glPopMatrix();
			string str = "FiducialId: "+ofToString((int)(blob->getFiducialId()));
			ofDrawBitmapString(str, blob->getX()*ofGetWidth()-10.0, blob->getY()*ofGetHeight()+25.0);
			str = "SessionId: "+ofToString((int)(blob->getSessionId()));
			ofDrawBitmapString(str, blob->getX()*ofGetWidth()-10.0, blob->getY()*ofGetHeight()+40.0);
		}
	};

	list<ofxTuioObject*> getTuioObjects() const {
		return objectList;
	};

	list<ofxTuioCursor*> getTuioCursors() const {
		return cursorList;
	};

	ofEvent<ofxTuioCursor> cursorRemoved;
	ofEvent<ofxTuioCursor> cursorAdded;
	ofEvent<ofxTuioCursor> cursorUpdated;

	ofEvent<ofxTuioObject> objectAdded;
	ofEvent<ofxTuioObject> objectRemoved;
	ofEvent<ofxTuioObject> objectUpdated;

	ofPoint accelData;
	int remoteOrientation, remoteHomeButton;
	float lerpSpeed;
private:

	ofxOscReceiver	receiver;

	list<ofxTuioCursor*> cursorList;
	list<ofxTuioCursor*> freeCursorList;
	list<ofxTuioCursor*> freeCursorBuffer;
	list<ofxTuioObject*> objectList;
	list<long> aliveObjectList;
	list<long> newObjectList;
	list<long> aliveCursorList;
	list<long> newCursorList;

	int currentFrame, lastFrame, maxFingerID;
};
