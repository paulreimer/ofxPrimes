#include "Renderer.h"
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
				static bool bSetup = false;
				
				static ofTexture grayLineTex;
				static ofTexture rgbLineTex;
				
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
							TexLine(startpoint, endpoint, rgbLineTex);
							break;
						case ACTOR_TYPE_GRAY:
							TexLine(startpoint, endpoint, grayLineTex);
							break;
					}
				}

				void grayLine(ofPoint& startpoint, ofPoint& endpoint) 
				{
					TexLine(startpoint, endpoint, grayLineTex);
				}

				void rgbLine(ofPoint& startpoint, ofPoint& endpoint) 
				{
					TexLine(startpoint, endpoint, rgbLineTex);
				}
				
				void setup()
				{					
					if (bSetup)
						return;
					
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
					cout << "Renderer textures initialized." << endl;
				}				
			}
		}
	}
}

