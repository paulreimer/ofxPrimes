#include "GuiSet.h"
#include "physics_types.h"

using namespace ofxFiducialBacked::types;
using namespace ofxFiducialBacked::types::gui;
using namespace ofxFiducialBacked::types::events;
using namespace ofxFiducialBacked::types::physics;

namespace ofxFiducialBacked {
namespace gui {

GuiSet::GuiSet()
{
	bUseFiducials = true;
	scaleFac = DEFAULT_FIDUCIAL_SCALE_FACTOR;

	enableAllEvents();
}

void
GuiSet::setup()
{
	if (bUseFiducials)
	{
		fiducial_set_t::iterator gui;
		for (gui = begin(); gui != end(); gui++)
			gui->second->enabled = false;
	}
}

fiducial_set_t::iterator GuiSet::find(int fiducialId)
{
	return find_if(begin(), end(), fiducial_pair_by_key(fiducialId));
}

GuiPage* GuiSet::add(int fiducialId)
{
	fiducial_set_t::iterator existingGui = this->find(fiducialId);
	if (existingGui == this->end())
	{
		this->push_back(make_pair(fiducialId, new GuiPage(string("Fiducial "+fiducialId))));
	}

//	return (*this)[fiducialId];
	return find(fiducialId)->second;
}

//--------------------------------------------------------------
GuiPage*
GuiSet::add(int fiducialId, GuiPage* newPage)
{
	push_back(make_pair(fiducialId, newPage));
	return newPage;
}

void
GuiSet::remove(int fiducialId)
{
	fiducial_set_t::iterator existingGui = this->find(fiducialId);
	if (existingGui != this->end())
		this->erase(existingGui);
}

void
GuiSet::update()
{}


#ifdef NOTIFY_FIDUCIAL_STATUS
//--------------------------------------------------------------
void
GuiSet::fiducialFound(fiducialEvtArgs &args)
{
	ofxFiducial		&fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();
	//	cout << "Fiducial " << fiducialId << " found\t\t+(" << fiducial.life << ")" << endl;

	fiducial_set_t::iterator chk_gui;

	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
	{
		fiducialUpdated(args); // set position, angle, size
		chk_gui->second->activate(true);
	}
}

//--------------------------------------------------------------
void
GuiSet::fiducialLost(fiducialEvtArgs &args)
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();

	fiducial_set_t::iterator chk_gui;

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
GuiSet::fiducialUpdated(fiducialEvtArgs &args)
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();

	//	cout << "Fiducial " << fiducialId << " updated\t.(" << fiducial.life << ")" << endl;

	fiducial_set_t::iterator chk_gui;

	chk_gui = find(fiducial.getId());
	if (chk_gui != end())
	{
		chk_gui->second->setPos(fiducial.getX(), fiducial.getY());

		chk_gui->second->setScale(fiducial.getRootSize()*scaleFac);
		chk_gui->second->setAngle(fiducial.getAngle()+FIDUCIAL_ANGLE_CORRECTION_RADIANS);
	}
}
#endif

//#ifdef NOTIFY_FIDUCIAL_INTERSECTION
//--------------------------------------------------------------
void
GuiSet::fiducialRayTermination(fiducialRayIntersectionEvtArgs &args)
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

	if (args.to != NULL || bUseFiducials)
		segments[args.from->getId()] = make_pair(draw_from, draw_to);
}

} //gui
} //ofxFiducialBacked
