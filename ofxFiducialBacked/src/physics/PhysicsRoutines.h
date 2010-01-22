#include "physics_types.h"

namespace ofxFiducialBacked {
	namespace physics {
		class Routines
		{
			friend class Engine;
			
			Routines(ofxFiducialBacked::types::fiducials_t& _fiducialsList);
		protected:
			ofxFiducialBacked::types::fiducials_t&				fiducialsList;
			
			ofxFiducialBacked::types::physics::functions_map_t	functions_map;
			ofxFiducialBacked::types::physics::shapes_map_t		shapes;
			
			virtual vector<ofPoint> getShape(ofxFiducial* fiducial);
			virtual vector<pair<ofPoint, double> > getOutputRays(ofxFiducial* fiducial);

		#ifdef NOTIFY_FIDUCIAL_INTERSECTION
			queue<ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs>
			findIntersectionsRay		(ofxFiducial* fiducial);
			queue<ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs>
			findIntersectionsMultiRay	(ofxFiducial* fiducial);
			queue<ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs>
			findIntersectionsTri		(ofxFiducial* fiducial);
			queue<ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs>
			findIntersectionsQuad		(ofxFiducial* fiducial);
			queue<ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs>
			findIntersectionsPrism		(ofxFiducial* fiducial);
		#endif

		private:
			ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs
			findMinIntersectionRay(ofxFiducial* fiducial, ofPoint _origin, double angle);

			ofxPoint2f intersects_window_edge(const ofxPoint2f& origin, const ofxPoint2f& endpoint);
			
		#ifdef NOTIFY_FIDUCIAL_INTERSECTION
			ofxPoint2f lineSegmentTest(const ofxPoint2f& origin, const ofxPoint2f& endpoint,
									   const vector<ofPoint>& corners);
		#endif
		};
	}
}
