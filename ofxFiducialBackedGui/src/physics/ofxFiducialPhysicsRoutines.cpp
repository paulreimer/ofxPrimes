#include "ofxFiducialPhysics.h"
#include <float.h>

using namespace ofxFiducialBacked::types;
using namespace ofxFiducialBacked::types::physics;
using namespace ofxFiducialBacked::types::events;

//--------------------------------------------------------------
ofxFiducialPhysicsRoutines::ofxFiducialPhysicsRoutines(fiducials_t& _fiducialsList)
: fiducialsList(_fiducialsList)
{
	routines[FIDUCIAL_PHYSICS_RAY]		= &ofxFiducialPhysicsRoutines::findIntersectionsRay;
	routines[FIDUCIAL_PHYSICS_MULTI_RAY]= &ofxFiducialPhysicsRoutines::findIntersectionsMultiRay;
	routines[FIDUCIAL_PHYSICS_TRI]		= &ofxFiducialPhysicsRoutines::findIntersectionsTri;
	routines[FIDUCIAL_PHYSICS_QUAD]		= &ofxFiducialPhysicsRoutines::findIntersectionsQuad;
	routines[FIDUCIAL_PHYSICS_PRISM]	= &ofxFiducialPhysicsRoutines::findIntersectionsPrism;
	routines[FIDUCIAL_PHYSICS_DEFAULT]	= routines[FIDUCIAL_PHYSICS_RAY];	
}

//--------------------------------------------------------------
fiducialRayIntersectionEvtArgs
ofxFiducialPhysicsRoutines::findMinIntersectionRay(ofxFiducial* fiducial, ofPoint _origin, double angle)
{
	fiducial_iter	chk_fiducial_it;
	ofxFiducial		*chk_fiducial, *hit_fiducial;
	
	ofxPoint2f		first_hit_point, edge_hit_point, hit_point;
	ofxPoint2f		origin(_origin), endpoint;

	float dist, min_dist;	

	endpoint.set(origin.x + MAX_INTERSECTION_DIST, origin.y);
	endpoint.rotateRad(angle, origin);

	first_hit_point = edge_hit_point = intersects_window_edge(origin, endpoint);

	min_dist = origin.distance(first_hit_point);

	hit_fiducial = NULL;

	// Check if this candidate fiducial intersects any others, and choose the one
	// with minimum distance intersection
	for (chk_fiducial_it = fiducialsList.begin();
		 chk_fiducial_it != fiducialsList.end();
		 chk_fiducial_it++)
	{
		chk_fiducial = &(*chk_fiducial_it);
		
		if (chk_fiducial == fiducial)
			continue;
		
		vector<ofPoint> chk_shape = getShape(chk_fiducial);
		
		hit_point = lineSegmentTest(origin, first_hit_point, chk_shape);
		
		if (hit_point.x >= 0 && hit_point.y >= 0)
		{
			dist = origin.distance(hit_point);
			if (dist < min_dist)
			{
				min_dist = dist;
				first_hit_point = hit_point;
				
				hit_fiducial = chk_fiducial;
			}
		}
	}
	
	fiducialRayIntersectionEvtArgs args;
	// Generate intersection event
	args.to		= hit_fiducial;
	args.to_pt	= first_hit_point;
	return args;
}

//--------------------------------------------------------------
queue<fiducialRayIntersectionEvtArgs>
ofxFiducialPhysicsRoutines::findIntersectionsRay(ofxFiducial* fiducial)
{
	queue<fiducialRayIntersectionEvtArgs> intersections;
	fiducialRayIntersectionEvtArgs args;

	ofxPoint2f	origin	= ofxPoint2f(fiducial->cornerPoints[0])
							.getMiddle(ofxPoint2f(fiducial->cornerPoints[3]));
	float		angle	= fiducial->getAngle();

	args = findMinIntersectionRay(fiducial, origin, angle);
	
	args.from	= fiducial;
	args.from_pt= ofxPoint2f(fiducial->cornerPoints[0])
					.getMiddle(ofxPoint2f(fiducial->cornerPoints[3]));

	intersections.push(args);

	return intersections;
}

//--------------------------------------------------------------
queue<fiducialRayIntersectionEvtArgs>
ofxFiducialPhysicsRoutines::findIntersectionsMultiRay(ofxFiducial* fiducial)
{
	queue<fiducialRayIntersectionEvtArgs> intersections;
	fiducialRayIntersectionEvtArgs args;

	ofxPoint2f origin;
	float angle;

	vector<pair<ofPoint, double> > rays = getOutputRays(fiducial);
	for (int output_idx=0; output_idx<rays.size(); output_idx++)
	{
		origin	= ofxPoint2f(fiducial->cornerPoints[0])
					.getMiddle(ofxPoint2f(fiducial->cornerPoints[3]));
		angle	= fiducial->getAngle();

		args = findMinIntersectionRay(fiducial, origin, angle);

		args.from	= fiducial;
		args.from_pt= origin;

		intersections.push(args);
	}

	return intersections;
}

//--------------------------------------------------------------
queue<fiducialRayIntersectionEvtArgs>
ofxFiducialPhysicsRoutines::findIntersectionsTri(ofxFiducial* fiducial)
{}

//--------------------------------------------------------------
queue<fiducialRayIntersectionEvtArgs>
ofxFiducialPhysicsRoutines::findIntersectionsQuad(ofxFiducial* fiducial)
{}

//--------------------------------------------------------------
queue<fiducialRayIntersectionEvtArgs>
ofxFiducialPhysicsRoutines::findIntersectionsPrism(ofxFiducial* fiducial)
{}

//--------------------------------------------------------------
vector<pair<ofPoint, double> >
ofxFiducialPhysicsRoutines::getOutputRays(ofxFiducial* fiducial)
{
	vector<pair<ofPoint,double> > single_output_port;
	ofPoint output_point = ofxPoint2f(fiducial->cornerPoints[0]).getMiddle(ofxPoint2f(fiducial->cornerPoints[3]));
	single_output_port.push_back(make_pair(output_point, fiducial->getAngle()));
	
	return single_output_port;
}

//--------------------------------------------------------------
vector<ofPoint>
ofxFiducialPhysicsRoutines::getShape(ofxFiducial* fiducial)
{
#ifndef NOTIFY_FIDUCIAL_STATUS
	if (!fiducial->getCornerUpdateStatus())
		fiducial->computeCorners();
#endif
	return fiducial->cornerPoints;
}

#ifdef NOTIFY_FIDUCIAL_INTERSECTION
//--------------------------------------------------------------
ofxPoint2f
ofxFiducialPhysicsRoutines::intersects_window_edge(const ofxPoint2f& origin, const ofxPoint2f& endpoint)
{
	shapes_map_iter window_corners = shapes.find(SHAPE_WINDOW);
	if (window_corners != shapes.end())
		return lineSegmentTest(origin, endpoint, window_corners->second);
	else
		return ofxPoint2f(FIDUCIAL_INVALID_COORDS,FIDUCIAL_INVALID_COORDS);
}

//--------------------------------------------------------------
//http://newsgroups.archived.at/comp/graphics.algorithms/200603/12/0603122551.html
ofxPoint2f
ofxFiducialPhysicsRoutines::lineSegmentTest(const ofxPoint2f& origin, const ofxPoint2f& endpoint,
		   const vector<ofPoint>& corners)
{
	ofxPoint2f hit_point(endpoint);
	ofxPoint2f first_hit_point(FIDUCIAL_INVALID_COORDS,FIDUCIAL_INVALID_COORDS);
	
	double min_dist = origin.distance(endpoint);
	double d, la, lb, dist;
	int num_corners = corners.size();
	for (int c=0,next_c=1; c<num_corners; c++, next_c++)
	{
		if (next_c==num_corners)
			next_c=0;
		
		ofxPoint2f dp(corners[c].x - origin.x, corners[c].y - origin.y);
		ofxPoint2f qa(endpoint.x - origin.x, endpoint.y - origin.y);
		ofxPoint2f qb(corners[next_c].x - corners[c].x, corners[next_c].y - corners[c].y);
		
		d  = qa.y * qb.x - qb.y * qa.x;
		la = (qb.x * dp.y - qb.y * dp.x)/d;
		lb = (qa.x * dp.y - qa.y * dp.x)/d;
		
		if (0 <= la && la <= 1 && 0 <= lb && lb <= 1)
		{
			hit_point.set(origin.x + la * qa.x,
						  origin.y + la * qa.y);
			dist = origin.distance(hit_point);
			
			if (dist < min_dist)
			{
				min_dist = dist;
				first_hit_point = hit_point;
			}
		}
	}
	return first_hit_point;
}
#endif
