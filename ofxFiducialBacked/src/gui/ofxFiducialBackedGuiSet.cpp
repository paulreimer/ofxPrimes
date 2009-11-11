#include "ofxFiducialBackedGuiSet.h"

ofxFiducialBackedGuiSet::ofxFiducialBackedGuiSet()
{
	enableAllEvents();
}

void
ofxFiducialBackedGuiSet::setup()
{
	fiducial_set_t::iterator gui;
	for (gui = begin(); gui != end(); gui++)
		gui->second->enabled = false;		
}

ofxFiducialBackedGui* ofxFiducialBackedGuiSet::add(int fiducialId)
{
	fiducial_set_t::iterator existingGui = this->find(fiducialId);
	if (existingGui == this->end())
	{
		this->insert(make_pair(fiducialId, new ofxFiducialBackedGui(string("Fiducial "+fiducialId))));
	}
	
	return (*this)[fiducialId];
}

void
ofxFiducialBackedGuiSet::remove(int fiducialId)
{
	fiducial_set_t::iterator existingGui = this->find(fiducialId);
	if (existingGui != this->end())
		this->erase(existingGui);
}

void
ofxFiducialBackedGuiSet::update()
{
	if (tracker == NULL)
		return;
	
	list<ofxFiducial>::iterator fiducial;
	fiducial_set_t::iterator gui, chk_gui;
	
	for (gui = begin(); gui != end(); gui++)
		gui->second->activate(false);
	
	for (fiducial = tracker->fiducialsList.begin();
		 fiducial != tracker->fiducialsList.end();
		 fiducial++)
	{
		chk_gui = find(fiducial->getId());
		if (chk_gui != end())
		{
			//				ofPoint &origin = fiducial->cornerPoints[FIDUCIAL_ORIGIN_CORNER];
			
			chk_gui->second->setPos(fiducial->getX(), fiducial->getY());
			
			chk_gui->second->size	= fiducial->getRootSize();
			chk_gui->second->angle	= fiducial->getAngle();
			
			chk_gui->second->activate(true);
		}
	}
}
