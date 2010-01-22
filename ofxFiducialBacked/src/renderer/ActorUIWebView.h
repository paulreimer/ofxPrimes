#pragma once

#ifdef TARGET_OF_IPHONE

#include "renderer_types.h"
#include "ofxMSAInteractiveObject.h"

namespace ofxFiducialBacked {
	namespace renderer {
		namespace iPhone {
			namespace render {
				void browser(ofRectangle frame, ofPoint origin=ofPoint(0,0), double angle=0.0, char* newUrl=NULL);
			}
		}
	}
}

#endif