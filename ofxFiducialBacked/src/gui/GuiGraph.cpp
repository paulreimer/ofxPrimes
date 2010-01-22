#include "GuiGraph.h"
#include "../renderer/Renderer.h"

using namespace ofxFiducialBacked::renderer::primitives;
using namespace ofxFiducialBacked::types;
using namespace ofxFiducialBacked::types::gui;
using namespace ofxFiducialBacked::types::events;
using namespace ofxFiducialBacked::types::actors;

namespace ofxFiducialBacked {
namespace gui {
//--------------------------------------------------------------
GuiGraph::GuiGraph()
{
	bUseFiducials = true;
	scaleFac = DEFAULT_FIDUCIAL_SCALE_FACTOR;
}

//--------------------------------------------------------------
void
GuiGraph::setup()
{
	if (bUseFiducials)
	{
		fiducial_graph_t::iterator gui_it;
		for (gui_it = begin(); gui_it != end(); gui_it++)
			gui_it->second->enabled = false;
	}

	// Idempotent static function, gotta call it somewhere.
//	ofxFiducialBacked::renderer::style::setup();
	ofxFiducialBacked::renderer::primitives::render::setup();
}

//--------------------------------------------------------------
void
GuiGraph::update()
{}

//--------------------------------------------------------------
void
GuiGraph::draw()
{
#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

	fiducial_rays_t::iterator ray_it;
	if (!segments.empty())
		for (ray_it = segments.begin(); ray_it != segments.end(); ray_it++)
			render::arrow(ray_it->second.first, ray_it->second.second, types::actors::ACTOR_TYPE_RGB);
	
#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
}

//--------------------------------------------------------------
GuiPage*
GuiGraph::add(int fiducialId)
{
	fiducial_graph_t::iterator existingGui = this->find(fiducialId);
	if (existingGui == this->end())
	{
		this->insert(make_pair(fiducialId, new GuiPage(string("Fiducial "+fiducialId))));
	}
	
	return (*this)[fiducialId];
}

//--------------------------------------------------------------
GuiPage*
GuiGraph::add(int fiducialId, GuiPage* newPage)
{
	this->insert(make_pair(fiducialId, newPage));

	return (*this)[fiducialId];
}
	
//--------------------------------------------------------------
void
GuiGraph::remove(int fiducialId)
{
	fiducial_graph_t::iterator existingGui = this->find(fiducialId);
	if (existingGui != this->end())
		this->erase(existingGui);
}

#ifdef NOTIFY_FIDUCIAL_STATUS
//--------------------------------------------------------------
void
GuiGraph::fiducialFound(fiducialEvtArgs &args) 
{
	ofxFiducial		&fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();
//	cout << "(GuiGraph)Fiducial " << fiducialId << " found\t\t+(" << fiducial.life << ")" << endl;	
	
	fiducial_graph_t::iterator chk_gui;

	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
	{
		fiducialUpdated(args); // set position, angle, size
		chk_gui->second->activate(true);
	}
}

//--------------------------------------------------------------
void
GuiGraph::fiducialLost(fiducialEvtArgs &args) 
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
GuiGraph::fiducialUpdated(fiducialEvtArgs &args)
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();

//	cout << "Fiducial " << fiducialId << " updated\t.(" << fiducial.life << ")" << endl;

	fiducial_graph_t::iterator chk_gui;

	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
	{
		chk_gui->second->setPos(fiducial.getX() * args.to_window.x,
								fiducial.getY() * args.to_window.y);
		
		float rootSize = (fiducial.getRootSize()/2) * args.to_window.x;

		chk_gui->second->setScale(rootSize*scaleFac);
		chk_gui->second->setAngle(fiducial.getAngle()+FIDUCIAL_ANGLE_CORRECTION_RADIANS);
		chk_gui->second->setOffset(ofPoint(-rootSize, 2*rootSize));
	}
}
#endif

//#ifdef NOTIFY_FIDUCIAL_INTERSECTION
//--------------------------------------------------------------
void
GuiGraph::fiducialRayTermination(fiducialRayIntersectionEvtArgs &args)
{
	if (args.from == NULL)
		return;

	ofxPoint2f	from_pt	= args.from_pt	* args.to_window,
				to_pt	= args.to_pt	* args.to_window;

	float dist = from_pt.distance(to_pt);
	float trimPct = (float)TRIM_RAY_PIXELS/dist;

	ofxPoint2f draw_from = from_pt.getInterpolated(to_pt, trimPct);
	ofxPoint2f draw_to;

	if (args.to != NULL)
		draw_to	= from_pt.getInterpolated(to_pt, 1-trimPct);
	else
		draw_to = to_pt;

	segments[args.from->getId()] = make_pair(draw_from, draw_to);
}
/*
//--------------------------------------------------------------
void
GuiGraph::fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args)
{
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f from_pt = args.from_pt;
	ofxPoint2f to_pt = args.to_pt;
	cout << "Found intersection" << endl;
}

//--------------------------------------------------------------
void
GuiGraph::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f from_pt = args.from_pt;
	ofxPoint2f to_pt = args.to_pt;
	cout << "Lost intersection" << endl;
}

//--------------------------------------------------------------
void
GuiGraph::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f from_pt = args.from_pt;
	ofxPoint2f to_pt = args.to_pt;
	cout << "Updated intersection" << endl;
}
*/
} //gui
} //ofxFiducialBacked
