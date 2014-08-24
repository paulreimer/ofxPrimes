#pragma once

#include "config.h"

#include "ofxFiducial.h"

namespace ofxFiducialBacked {
	namespace types {
		typedef int fiducialIndex;
		//typedef ofxFiducial* fiducialIndex;

		typedef list<ofxFiducial>			fiducials_t;
		typedef fiducials_t::iterator		fiducial_iter;

#ifdef USE_FINGER_TRACKING
		typedef list<ofxFinger>				fingers_t;
		typedef fingers_t::iterator			finger_iter;
#endif

		typedef list<int>					fiducial_category_t;
		typedef map<int, pair<int, int> >	fiducial_ranges_flags_t;
	}
}
