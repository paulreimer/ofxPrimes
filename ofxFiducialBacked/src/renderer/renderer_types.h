#pragma once

#include "../config.h"
#include "ofTypes.h"

namespace ofxFiducialBacked {
	namespace types {
		namespace actors {
			typedef unsigned char actor_type_t;
			enum actor_types {
				ACTOR_TYPE_NULL,
				ACTOR_TYPE_RGB,
				ACTOR_TYPE_GRAY,
			};
			
			typedef unsigned char arrow_type_t;
			enum arrow_types {
				ARROW_TYPE_NONE,
				ARROW_TYPE_START,
				ARROW_TYPE_END,
				ARROW_TYPE_BOTH,
			};
			
			struct lineSegment {
				ofPoint			from;
				ofPoint			to;
				actor_type_t	from_type;
				actor_type_t	to_type;
				
				lineSegment() {
					from_type	= ACTOR_TYPE_NULL;
					to_type		= ACTOR_TYPE_NULL;
				}
			};
		}
	}
}