#include "ofxFiducialBackedGui.h"
#include "ofxMSAInteractiveObject.h"

typedef map<int, ofxFiducialBackedGui*> fiducial_set_t;

class ofxFiducialBackedGuiSet : fiducial_set_t, public ofxMSAInteractiveObject
{
public:
	ofxFiducialBackedGuiSet();
	
	void setup();
	void update();

	ofxFiducialBackedGui* add(int fiducialId);
	void remove(int fiducialId);

	ofxFiducialTracker *tracker;
};
