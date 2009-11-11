#pragma once

#include "config.h"

#include "fiducial_types.h"
#include "ofxPoint2f.h"

namespace ofxFiducialBacked {
	namespace types {			
#ifdef OF_USING_POCO
		namespace events {

			struct fiducialEvtArgs {
				ofxFiducial* fiducial;
				
				fiducialEvtArgs() {
					fiducial = NULL;
				}
			};

			struct fiducialRayIntersectionEvtArgs {
				ofxFiducial* from;
				ofxFiducial* to;
				ofxPoint2f from_pt;
				ofxPoint2f to_pt;
				
				fiducialRayIntersectionEvtArgs() {
					from = NULL;
					to = NULL;
					from_pt = ofxPoint2f(0,0);
					to_pt = ofxPoint2f(0,0);
				}
			};
		}		
#endif
	}
}
