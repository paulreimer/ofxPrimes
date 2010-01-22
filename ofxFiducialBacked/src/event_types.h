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
				
				ofPoint			to_window;
				ofPoint			to_normal;

				fiducialEvtArgs() {
					fiducial = NULL;
				}
			};

			struct fiducialRayIntersectionEvtArgs {
				ofxFiducial*	from;
				ofxFiducial*	to;
				ofxPoint2f		from_pt;
				ofxPoint2f		to_pt;

				ofPoint			to_window;
				ofPoint			to_normal;

				fiducialRayIntersectionEvtArgs() {
					from = NULL;
					to = NULL;
					from_pt = ofxPoint2f(0,0);
					to_pt = ofxPoint2f(0,0);
					
					to_window = ofPoint(1, 1);
					to_normal = ofPoint(1, 1);
				}
			};
		}		
#endif
	}
}
