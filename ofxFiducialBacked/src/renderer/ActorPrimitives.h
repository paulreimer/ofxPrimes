#pragma once

#include "renderer_types.h"
//using namespace ofxFiducialBacked::renderer;

namespace ofxFiducialBacked {
	namespace renderer {
		namespace primitives {
			namespace render {
				void setup();

				void arrow	(ofxFiducialBacked::types::actors::lineSegment segment,
							 ofxFiducialBacked::types::actors::arrow_type_t arrowtype=ofxFiducialBacked::types::actors::ARROW_TYPE_BOTH);
				void line	(ofxFiducialBacked::types::actors::lineSegment segment);
				void line	(ofPoint& startpoint, ofPoint& endpoint,
							 ofxFiducialBacked::types::actors::actor_type_t starttype,
							 ofxFiducialBacked::types::actors::actor_type_t fromtype=ofxFiducialBacked::types::actors::ACTOR_TYPE_NULL);
				void arrow	(ofPoint& startpoint, ofPoint& endpoint,
							 ofxFiducialBacked::types::actors::actor_type_t starttype,
							 ofxFiducialBacked::types::actors::actor_type_t fromtype=ofxFiducialBacked::types::actors::ACTOR_TYPE_NULL,
							 ofxFiducialBacked::types::actors::arrow_type_t arrowtype=ofxFiducialBacked::types::actors::ARROW_TYPE_BOTH);

				void grayLine	(ofPoint& startpoint, ofPoint& endpoint);
				void rgbLine	(ofPoint& startpoint, ofPoint& endpoint);
				void TexLine	(ofPoint& startpoint, ofPoint& endpoint, ofTexture& tex);
			}
		}
	}
}