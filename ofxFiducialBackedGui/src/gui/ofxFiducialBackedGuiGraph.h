//#include "ofxFiducialPhysics.h"
#include "gui_types.h"
#include "../event_types.h"

#include "ofxFiducialBackedGui.h"
#include "ofxMSAInteractiveObject.h"

using namespace ofxFiducialBacked;

class ofxFiducialBackedGuiGraph : types::gui::fiducial_graph_t, public ofxMSAInteractiveObject
{
public:
	void setup();
	void update();
	void draw();

	ofxFiducialBackedGui* add(int fiducialId);
	void remove(int fiducialId);

#ifdef NOTIFY_FIDUCIAL_STATUS
	void fiducialFound	(types::events::fiducialEvtArgs &args);
	void fiducialLost	(types::events::fiducialEvtArgs &args);
	void fiducialUpdated(types::events::fiducialEvtArgs &args);
#endif
	
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
	void fiducialRayTermination			(types::events::fiducialRayIntersectionEvtArgs &args);

	void fiducialRayIntersectionFound	(types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionLost	(types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionUpdated	(types::events::fiducialRayIntersectionEvtArgs &args);
/*	
	list<pair<ofxFiducial*, ofxFiducial*> > connections;
*/
	types::gui::fiducial_rays_t segments;
#endif
};
