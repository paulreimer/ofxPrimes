#include "ofxFiducialBackedGuiGraph.h"
#include "../renderer/ofxFiducialBackedRenderer.h"

using namespace ofxFiducialBacked::renderer::primitives;
using namespace ofxFiducialBacked::types;
using namespace ofxFiducialBacked::types::gui;
using namespace ofxFiducialBacked::types::events;
using namespace ofxFiducialBacked::types::actors;

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::setup()
{
	fiducial_graph_t::iterator gui;
	for (gui = begin(); gui != end(); gui++)
		gui->second->enabled = false;

	// Static function, gotta call it somewhere.
	renderer::style::setup();	
}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::update()
{}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::draw()
{
	fiducial_rays_t::iterator ray_it;
	if (!segments.empty())
		for (ray_it = segments.begin(); ray_it != segments.end(); ray_it++)
			render::arrow(ray_it->second.first, ray_it->second.second, types::actors::ACTOR_TYPE_GRAY);
}

//--------------------------------------------------------------
ofxFiducialBackedGui*
ofxFiducialBackedGuiGraph::add(int fiducialId)
{
	fiducial_graph_t::iterator existingGui = this->find(fiducialId);
	if (existingGui == this->end())
	{
		this->insert(make_pair(fiducialId, new ofxFiducialBackedGui(string("Fiducial "+fiducialId))));
	}
	
	return (*this)[fiducialId];
}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::remove(int fiducialId)
{
	fiducial_graph_t::iterator existingGui = this->find(fiducialId);
	if (existingGui != this->end())
		this->erase(existingGui);
}

#ifdef NOTIFY_FIDUCIAL_STATUS
//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::fiducialFound(fiducialEvtArgs &args) 
{
	ofxFiducial		&fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();
//	cout << "Fiducial " << fiducialId << " found\t\t+(" << fiducial.life << ")" << endl;	
	
	fiducial_graph_t::iterator chk_gui;

	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
	{
		fiducialUpdated(args); // set position, angle, size
		chk_gui->second->activate(true);
	}
}

//--------------------------------------------------------------
void ofxFiducialBackedGuiGraph::fiducialLost(fiducialEvtArgs &args) 
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();
	
	fiducial_graph_t::iterator chk_gui;
	
//	cout << "Fiducial " << fiducialId << " lost\t\t-(" << fiducial.life << ")" << endl;
	
	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
		chk_gui->second->activate(false);
	
	fiducial_rays_t::iterator old_intersection_it = segments.find(fiducialId);
	if (old_intersection_it != segments.end())
		segments.erase(old_intersection_it);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::fiducialUpdated(fiducialEvtArgs &args)
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();
	
//	cout << "Fiducial " << fiducialId << " updated\t.(" << fiducial.life << ")" << endl;
	
	fiducial_graph_t::iterator chk_gui;
	
	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
	{
		chk_gui->second->setPos(fiducial.getX(), fiducial.getY());
		
		chk_gui->second->size	= fiducial.getRootSize();
		chk_gui->second->angle	= fiducial.getAngle();
	}
}
#endif

//#ifdef NOTIFY_FIDUCIAL_INTERSECTION
//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::fiducialRayTermination(fiducialRayIntersectionEvtArgs &args)
{
	if (args.from == NULL)
		return;

	float dist = args.from_pt.distance(args.to_pt);
	float trimPct = 16.0/dist;

	ofxPoint2f draw_from = args.from_pt.getInterpolated(args.to_pt, trimPct); // 10*0.05/dist);
	ofxPoint2f draw_to;

	if (args.to != NULL)
		draw_to	= args.from_pt.getInterpolated(args.to_pt, 1-trimPct); // 10*0.95/dist);
	else
		draw_to = args.to_pt;

	segments[args.from->getId()] = make_pair(draw_from, draw_to);
}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args)
{
/*
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f from_pt = args.from_pt;
	ofxPoint2f to_pt = args.to_pt;
*/	
//	cout << "Found intersection" << endl;
}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{
/*	
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f from_pt = args.from_pt;
	ofxPoint2f to_pt = args.to_pt;
*/
//	cout << "Lost intersection" << endl;
}

//--------------------------------------------------------------
void
ofxFiducialBackedGuiGraph::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{
/*
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f from_pt = args.from_pt;
	ofxPoint2f to_pt = args.to_pt;
*/
//	cout << "Updated intersection" << endl;
}
