#include "ofxFiducialBackedRenderer.h"
#include "ActorPrimitives.h"

#include "ofMath.h"
#include "ofGraphics.h"
#include "ofTexture.h"

using namespace ofxFiducialBacked::types::actors;
using namespace ofxFiducialBacked::renderer;

namespace ofxFiducialBacked {
	namespace renderer {
		namespace primitives {			
			namespace render {
				void TexLine(ofPoint& startpoint, ofPoint& endpoint, ofTexture& tex) 
				{
					float dist = ofDist(startpoint.x, startpoint.y, endpoint.x, endpoint.y);
					float angle = atan2(endpoint.y-startpoint.y,
										endpoint.x-startpoint.x);;
					
					glPushMatrix();
					
					glTranslatef(startpoint.x, startpoint.y, 0.0);
					glRotatef(angle*180.0/PI, 0, 0, 1.0);
					
					tex.draw(style::lineHeight/2,	   -style::lineHeight/2,
							 dist - style::lineHeight,	style::lineHeight);
					
					glPopMatrix();
				}

				void arrow(lineSegment segment,
						   arrow_type_t arrowtype)
				{
					line(segment);
					
					if (arrowtype == ARROW_TYPE_BOTH || arrowtype == ARROW_TYPE_START)
						ofCircle(segment.from.x, segment.from.y, style::arrowheadRadius);
					
					if (arrowtype == ARROW_TYPE_BOTH || arrowtype == ARROW_TYPE_END)
						ofCircle(segment.to.x, segment.to.y, style::arrowheadRadius);
				}

				void arrow(ofPoint& startpoint, ofPoint& endpoint,
						   actor_type_t starttype, actor_type_t endtype,
						   arrow_type_t arrowtype)
				{
					line(startpoint, endpoint, starttype, endtype);
					
					if (arrowtype == ARROW_TYPE_BOTH || arrowtype == ARROW_TYPE_START)
						ofCircle(startpoint.x, startpoint.y, style::arrowheadRadius);
					
					if (arrowtype == ARROW_TYPE_BOTH || arrowtype == ARROW_TYPE_END)
						ofCircle(endpoint.x, endpoint.y, style::arrowheadRadius);
				}

				void line(lineSegment segment)
				{
					line(segment.from, segment.to, segment.from_type, segment.to_type);
				}

				void line(ofPoint& startpoint, ofPoint& endpoint,
						  actor_type_t starttype, actor_type_t endtype)
				{
					switch (starttype)
					{
						case ACTOR_TYPE_RGB:
							TexLine(startpoint, endpoint, style::rgbLineTex);
							break;
						case ACTOR_TYPE_GRAY:
							TexLine(startpoint, endpoint, style::grayLineTex);
							break;
					}
				}

				void grayLine(ofPoint& startpoint, ofPoint& endpoint) 
				{
					TexLine(startpoint, endpoint, style::grayLineTex);
				}

				void rgbLine(ofPoint& startpoint, ofPoint& endpoint) 
				{
					TexLine(startpoint, endpoint, style::rgbLineTex);
				}
			}
		}
	}
}

