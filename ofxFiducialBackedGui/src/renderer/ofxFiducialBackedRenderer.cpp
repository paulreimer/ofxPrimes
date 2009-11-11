#pragma once

#include "ofxFiducialBackedRenderer.h"

namespace ofxFiducialBacked {
	namespace renderer {
		namespace style {
			void setup() {
				if (bSetup)
					return;

				lineHeight = 6;
				arrowheadRadius = 3;

				grayLineTex.allocate(1,3, GL_LUMINANCE_ALPHA);
				unsigned char gray_line_data[3*2] = {
					255,0,	255,255,	255,0
					//	DARK	BRIGHT		DARK
				};
				grayLineTex.loadData(gray_line_data, 1,3, GL_LUMINANCE_ALPHA); 
				
				rgbLineTex.allocate(1,7, GL_RGBA);
				unsigned char rgb_line_data[7*4] = {
					255,0,0,0,	255,0,0,255,
					//r	DARK		BRIGHT
					0,255,0,0,	0,255,0,255,
					//g	DARK		BRIGHT
					0,0,255,0,	0,0,255,255,
					//b	DARK		BRIGHT
					0,0,255,0
					//	DARK
				};
				rgbLineTex.loadData(rgb_line_data, 1,7, GL_RGBA);

				bSetup = true;
			}
		}	
	}
}
