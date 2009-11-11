#pragma once

#include "../config.h"

#include "ofxFiducial.h"
#ifdef USE_FINGER_TRACKING
#include "ofxFinger.h"
#endif

class ofxFiducialBackedGui;

namespace ofxFiducialBacked {
	namespace types {	
		namespace gui {
			typedef map<int, ofxFiducialBackedGui*> fiducial_graph_t;
			typedef map<int, pair<ofPoint, ofPoint> > fiducial_rays_t;
		}
	}
}
