#pragma once

#include "gui_types.h"
#include "../event_types.h"

#include "GuiPage.h"
#include "ofxMSAInteractiveObject.h"

namespace ofxFiducialBacked {
	namespace gui {
		class GuiSet : public types::gui::fiducial_set_t, public ofxMSAInteractiveObject
		{
		public:
			GuiSet();

			virtual void setup();
			virtual void update();

			GuiPage* add(int fiducialId);
			GuiPage* add(int fiducialId, GuiPage* newPage);

			void remove(int fiducialId);

			types::gui::fiducial_set_t::iterator find(int fiducialId);

#ifdef NOTIFY_FIDUCIAL_STATUS
			void fiducialFound	(types::events::fiducialEvtArgs &args);
			void fiducialLost	(types::events::fiducialEvtArgs &args);
			void fiducialUpdated(types::events::fiducialEvtArgs &args);
#endif

#ifdef NOTIFY_FIDUCIAL_INTERSECTION
			void fiducialRayTermination			(types::events::fiducialRayIntersectionEvtArgs &args);
#endif
			types::gui::fiducial_rays_t segments;
			bool bUseFiducials;
			ofxFiducialTracker *tracker;

			float scaleFac;
		};
	}
}