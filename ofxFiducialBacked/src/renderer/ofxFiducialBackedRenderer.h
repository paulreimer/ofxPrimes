#pragma once

#include "ofTexture.h"

#include "ActorPrimitives.h"
#include "ActorUIWebView.h"

namespace ofxFiducialBacked {
	namespace renderer {
		namespace style {
			static bool bSetup	= false;

			static ofTexture grayLineTex;
			static ofTexture rgbLineTex;
			static int lineHeight;
			static int arrowheadRadius;
		
			void setup();
		}
	}
}
