#pragma once

//#include "ofxFiducialPhysics.h"
#include "gui_types.h"
#include "../event_types.h"

#include "GuiPage.h"
#include "ofxMSAInteractiveObject.h"

namespace ofxFiducialBacked {
	namespace gui {
		class GuiGraph : public types::gui::fiducial_graph_t, public ofxMSAInteractiveObject
		{
		public:
			GuiGraph();
			void setup();
			void update();
			void draw();

			GuiPage* add(int fiducialId);
			GuiPage* add(int fiducialId, GuiPage* newPage);

			void remove(int fiducialId);

		#ifdef NOTIFY_FIDUCIAL_STATUS
			void fiducialFound	(types::events::fiducialEvtArgs &args);
			void fiducialLost	(types::events::fiducialEvtArgs &args);
			void fiducialUpdated(types::events::fiducialEvtArgs &args);
		#endif

		#ifdef NOTIFY_FIDUCIAL_INTERSECTION
			void fiducialRayTermination			(types::events::fiducialRayIntersectionEvtArgs &args);
/*
			void fiducialRayIntersectionFound	(types::events::fiducialRayIntersectionEvtArgs &args);
			void fiducialRayIntersectionLost	(types::events::fiducialRayIntersectionEvtArgs &args);
			void fiducialRayIntersectionUpdated	(types::events::fiducialRayIntersectionEvtArgs &args);
			list<pair<ofxFiducial*, ofxFiducial*> > connections;
*/
			types::gui::fiducial_rays_t segments;
		#endif
			bool bUseFiducials;
			float scaleFac;
		};
	}
}