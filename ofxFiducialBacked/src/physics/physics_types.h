#pragma once

#include "../config.h"
#include "../event_types.h"
#include <queue>

// forward decl.
namespace ofxFiducialBacked {
	namespace physics {
		class Routines;
	}
}

namespace ofxFiducialBacked {
	namespace types {			
		namespace physics {

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
					(ofxFiducialBacked::physics::Routines::*routine)(ofxFiducial* fiducial);

			typedef map<int, routine>			functions_map_t;
			typedef functions_map_t::iterator	functions_map_iter;

			typedef map<int, vector<ofPoint> >	shapes_map_t;
			typedef shapes_map_t::iterator		shapes_map_iter;
#endif

			class fiducial_pair_by_id {
			public:
				const int id;
				fiducial_pair_by_id(const int _id) : id(_id) {}
				bool operator()(ofxFiducial& fiducial) {
					return (fiducial.getId()==id);
				}
			};
			
		}
	}
}
