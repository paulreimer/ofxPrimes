#pragma once

#include "../config.h"
#include "../event_types.h"
#include <queue>

// forward decl.
class ofxFiducialPhysicsRoutines;

namespace ofxFiducialBacked {
	namespace types {			
		namespace physics {
			//typedef int fiducialIndex;
			//typedef ofxFiducial* fiducialIndex;

			//typedef list<ofxFiducial>			fiducials_t;
			//typedef fiducials_t::iterator		fiducial_iter;

#ifdef NOTIFY_FIDUCIAL_STATUS
			typedef map<int, int>				fiducial_states_t;
			typedef fiducial_states_t::iterator	fiducial_state_iter;
#endif
			
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
			typedef map<int, int>				fiducial_edges_t;
			typedef fiducial_edges_t::iterator	fiducial_edge_iter;
			
			typedef map<int, int>				fiducial_physics_types_t;
			typedef fiducial_physics_types_t::iterator physics_type_iter;

			enum FiducialPhysicsTypes
			{
				FIDUCIAL_PHYSICS_DEFAULT,
				FIDUCIAL_PHYSICS_RAY,
				FIDUCIAL_PHYSICS_MULTI_RAY,
				FIDUCIAL_PHYSICS_TRI,
				FIDUCIAL_PHYSICS_QUAD,
				FIDUCIAL_PHYSICS_PRISM,
			};

			enum PhysicsShapes
			{
				SHAPE_WINDOW,
			};

			typedef queue<ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs>
					(ofxFiducialPhysicsRoutines::*routine)(ofxFiducial* fiducial);

			typedef map<int, routine>			routines_map_t;
			typedef routines_map_t::iterator	routines_map_iter;

			typedef map<int, vector<ofPoint> >	shapes_map_t;
			typedef shapes_map_t::iterator		shapes_map_iter;

#endif
		}
	}
}
