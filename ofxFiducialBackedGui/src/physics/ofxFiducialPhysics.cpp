#include "ofxFiducialPhysics.h"
#include "../renderer/renderer_types.h"

using namespace ofxFiducialBacked::types;
using namespace ofxFiducialBacked::types::physics;
using namespace ofxFiducialBacked::types::events;

class fiducial_list_by_id {
public:
	const int id;
	fiducial_list_by_id(const int _id) : id(_id) {}
	bool operator()(ofxFiducial& fiducial) {
		return (fiducial.getId()==id);
	}
};

//--------------------------------------------------------------
ofxFiducialPhysics::ofxFiducialPhysics()
{
	tracker = NULL;
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
	bForceFiducialCorners = true;
	
	dummyFiducial = NULL;
	
	engine = NULL;
#endif
}

//--------------------------------------------------------------
ofxFiducialPhysics::~ofxFiducialPhysics()
{
	destroy();
}

//--------------------------------------------------------------
void
ofxFiducialPhysics::setup()
{
	if (tracker != NULL)
	{
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
		x_scale=(float)ofGetWidth()/width;
		y_scale=(float)ofGetHeight()/height;
		
		cout << "Initialize with videoSize: (" << width << "," << height << ")" << endl;
#endif
	}
	
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
	if (dummyFiducial == NULL)
		dummyFiducial = new ofxFiducial();
#endif
	
	if (!engine)
		engine = new ofxFiducialPhysicsRoutines(tracker->fiducialsList);

	window_corners.resize(4);
	window_corners[0].set(0,	0);
	window_corners[1].set(width,0);
	window_corners[2].set(width,height);
	window_corners[3].set(0,	height);
	
	engine->shapes[SHAPE_WINDOW] = window_corners;
}

//--------------------------------------------------------------
void
ofxFiducialPhysics::destroy()
{
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
	if (dummyFiducial != NULL)
		delete dummyFiducial;

	if (engine != NULL)
		delete engine;
#endif
}

//--------------------------------------------------------------
void
ofxFiducialPhysics::update()
{
	if (tracker == NULL)
		return;

	fiducial_iter	fiducial_it;
	ofxFiducial		*fiducial;
	fiducialIndex	fiducialId;
	
	fiducial_state_iter fiducial_state_it;
	int fiducialStatePrev;
	fiducials_t	&fiducialsList	= tracker->fiducialsList;
#ifdef USE_FINGER_TRACKING
	fingers_t	&fingersList	= tracker->fingersList;
#endif

#ifdef NOTIFY_FIDUCIAL_STATUS
	fiducialEvtArgs fid_args;
	
	// Scan for new fiducials
	for (fiducial_it =	fiducialsList.begin();
		 fiducial_it !=	fiducialsList.end();
		 fiducial_it++)
	{
		fiducial = &(*fiducial_it);
		fiducialId = fiducial->getId();

		if (find(ignoreFiducialIds.begin(), ignoreFiducialIds.end(), fiducialId)
			!= ignoreFiducialIds.end())
			continue;

		if (bForceFiducialCorners && !fiducial->getCornerUpdateStatus())
		{
			fiducial->computeCorners();
			fiducial->setUpdateCorners(true);
		}

		fid_args.fiducial = fiducial;

		fiducial_state_it = fiducialStates.find(fiducialId);
		
		if (fiducial_state_it == fiducialStates.end())
			fiducialStates[fiducialId] = FIDUCIAL_INVALID;

		fiducialStatePrev = fiducialStates[fiducialId];

		if (fiducial->state == FIDUCIAL_FOUND)
			if (fiducialStatePrev == FIDUCIAL_FOUND)
				ofNotifyEvent(this->fiducialUpdatedEvt, fid_args);
			else
				ofNotifyEvent(this->fiducialFoundEvt, fid_args);

		else if (fiducial->state == FIDUCIAL_LOST)
			if (fiducialStatePrev != FIDUCIAL_LOST)
				ofNotifyEvent(this->fiducialLostEvt, fid_args);

		fiducialStates[fiducialId] = fiducial->state;
	}
#endif
	
#ifdef NOTIFY_FIDUCIAL_INTERSECTION
	fiducialRayIntersectionEvtArgs args;	
	
	fiducial_edge_iter	fiducialEdgePrev;
	fiducial_iter		hit_fiducial_prev_it;
	ofxFiducial			*hit_fiducial, *hit_fiducial_prev;
	fiducialIndex		hit_fiducialId;

#ifndef NOTIFY_FIDUCIAL_INTERSECTION_ALWAYS
	if (fiducialsList.size() == 1)
		return;
#endif

	routine				physics_fun;
	routines_map_iter	physics_fun_it;
	physics_type_iter	physics_type_it;
	
	std::queue<fiducialRayIntersectionEvtArgs> fid_intersection_evts;

	for (fiducial_it = fiducialsList.begin();
		 fiducial_it != fiducialsList.end();
		 fiducial_it++)
	{
		fiducial = &(*fiducial_it);
		fiducialId = fiducial->getId();

		if (!validOutputFiducial(fiducial))
			continue;

		physics_type_it = fiducialPhysicsTypes.find(fiducial->getId());
		if (physics_type_it != fiducialPhysicsTypes.end())
			physics_fun_it = engine->routines.find(physics_type_it->second);
		else
			physics_fun_it = engine->routines.find(FIDUCIAL_PHYSICS_DEFAULT);

		if (physics_fun_it != engine->routines.end())
		{
			physics_fun = physics_fun_it->second;
			fid_intersection_evts = (engine->*physics_fun)(fiducial);
		}
		else
			continue;

		//fid_intersection_evts = findIntersections(fiducial);
		
		while (!fid_intersection_evts.empty())
		{
			args = fid_intersection_evts.front();

			hit_fiducial	= args.to;
			hit_fiducialId	= args.to? args.to->getId() : INVALID_FIDUCIAL_ID;

			fiducialEdgePrev = fiducialEdges.find(fiducialId);
			if (hit_fiducial != NULL)
			{
				if (fiducialEdgePrev != fiducialEdges.end()
					&& fiducialEdgePrev->second == hit_fiducialId)
					ofNotifyEvent(this->fiducialRayIntersectionUpdatedEvt, args);
				else {
					ofNotifyEvent(this->fiducialRayIntersectionFoundEvt, args);

					fiducialEdges[fiducialId] = hit_fiducialId;
				}
			}

			if (fiducial->state == FIDUCIAL_FOUND)
				ofNotifyEvent(this->fiducialRayTerminationEvt, args);
			
			if (fiducialEdgePrev != fiducialEdges.end()
				&& (!hit_fiducial || hit_fiducialId != fiducialEdgePrev->second))
			{
				if (!hit_fiducial)
				{
#ifdef NOTIFY_FIDUCIAL_INTERSECTION_STRICT
					break;
#endif
				}
				else if (hit_fiducialId != fiducialEdgePrev->second) // asserts true
				{
					hit_fiducial_prev_it = find_if(fiducialsList.begin(), fiducialsList.begin(),
												   fiducial_list_by_id(fiducialId));

					if (hit_fiducial_prev_it != fiducialsList.end())
						hit_fiducial_prev = &(*hit_fiducial_prev_it);
					else {
						dummyFiducial->fidId= fiducialEdgePrev->second;
						hit_fiducial_prev	= dummyFiducial;
					}
				}

				args.to		= hit_fiducial_prev;
				ofNotifyEvent(this->fiducialRayIntersectionLostEvt, args);

				fiducialEdges.erase(fiducialEdgePrev);
			}

			fid_intersection_evts.pop();
		}
	}
#endif
}

//--------------------------------------------------------------
bool
ofxFiducialPhysics::validFiducial(ofxFiducial* fiducial)
{	
	if (fiducial == NULL)
		return false;
	
	if (	fiducial->getX() == FIDUCIAL_INVALID_COORDS
		&&	fiducial->getY() == FIDUCIAL_INVALID_COORDS)
		return false;
	
#ifdef NOTIFY_FIDUCIAL_STATUS
	// Do not do intersection tests on fiducials which have
	// not sent a status event
	fiducial_state_iter fiducial_state_it = fiducialStates.find(fiducial->getId());
	if (fiducial_state_it == fiducialStates.end())
		return false;
#endif
	
	return true;
}

//--------------------------------------------------------------
bool
ofxFiducialPhysics::validInputFiducial(ofxFiducial* fiducial)
{
	if (!validFiducial(fiducial))
		return false;
	
	if (find(ignoreHitFiducialIds.begin(), ignoreHitFiducialIds.end(), fiducial->getId())
		!= ignoreHitFiducialIds.end())
		return false;
	
	return true;
}

//--------------------------------------------------------------
bool
ofxFiducialPhysics::validOutputFiducial(ofxFiducial* fiducial)
{
	return validFiducial(fiducial);	
}
