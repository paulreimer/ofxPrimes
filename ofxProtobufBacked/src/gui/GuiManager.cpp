#include "GuiManager.h"

#include <iostream>
#include "ofEvents.h"

using namespace google::protobuf;
using namespace ofxFiducialBacked::types::events;
using namespace ofxFiducialBacked::gui;
using namespace ofxProtobufBacked::protobuf;


#ifdef USE_LIBMEMCACHED
using namespace memcache;
#endif

namespace ofxProtobufBacked {
namespace gui {

GuiManager::GuiManager()
{
	baseName = "/";
}

void
GuiManager::setup()
{
#ifdef USE_LIBMEMCACHED
	for (int i=0; i<memcachedSockets.size(); i++)
		memc.addServer(memcachedSockets[i].host, memcachedSockets[i].port);
#endif

#ifdef USE_OSC_IN
	oscImporters.resize(oscInSockets.size());
	for (int i=0; i<oscImporters.size(); i++)
	{
		oscImporters[i].port = oscInSockets[i].port;
		oscImporters.setup();
		oscImporters[i].enableAppEvents();
	}
#endif

#ifdef USE_OSC_OUT
	oscExporters.resize(oscOutSockets.size());
	for (int i=0; i<oscExporters.size(); i++)
	{
		oscExporters[i].port = oscOutSockets[i].port;
		oscExporters[i].setup();
		oscExporters[i].enableAppEvents();
	}
#endif
}

#ifdef USE_OSC_IN
//--------------------------------------------------------------
Gui*
GuiManager::get(ofxOscMessage& m)
{
	string path;

	path = m.getAddress();

	switch (m.getArgType(0))
	{
		case OFXOSC_TYPE_BLOB:
			osc::Blob binary;
			Gui* gui_pb = new Gui();

			binary = m.getArgAsBlob(0);

			if (binary.size>0)
			{
				gui_pb->ParseFromArray(binary.data, binary.size);
				cout << "osc://ofxFiducialBacked::types::protobuf::Gui" << endl;
				return gui_pb;
			}
			break;
		default:
			break;
	}
	return NULL;
}
#endif

#ifdef USE_OSC_OUT
//--------------------------------------------------------------
void
GuiManager::controlValueChanged(controlEvtArgs& args)
{
	ofxSimpleGuiControl* control = args;

	if (oscExporters.empty())
		return;

	// TODO: this sucks
//	if (args->key[0] != '/')
//		return;

	ofxOscMessage m;
	m.setAddress(control->key);

	if		(args->controlType=="SliderString")
	{
		ofxSimpleGuiSliderString* control = dynamic_cast<ofxSimpleGuiSliderString*>(args);
		m.addIntArg(*control->value);

		map<int, string>::iterator chk = control->strs.find(*control->value);
		if (chk != control->strs.end())
			m.addStringArg(chk->second);
	}
	else if (control->controlType=="SliderFloat")
	{
		ofxSimpleGuiSliderFloat* control = dynamic_cast<ofxSimpleGuiSliderFloat*>(args);
		m.addFloatArg(*control->value);
	}
	else if (control->controlType=="SliderDouble")
	{
		ofxSimpleGuiSliderDouble* control = dynamic_cast<ofxSimpleGuiSliderDouble*>(args);
		m.addFloatArg((float)*control->value);
	}
	else if (control->controlType=="SliderByte")
	{
		ofxSimpleGuiSliderByte* control = dynamic_cast<ofxSimpleGuiSliderByte*>(args);
		m.addIntArg((int)*control->value);
	}
	else if (control->controlType=="SliderInt")
	{
		ofxSimpleGuiSliderInt* control = dynamic_cast<ofxSimpleGuiSliderInt*>(args);
		m.addIntArg(*control->value);
	}

	else if (control->controlType=="Button")
	{
		ofxSimpleGuiButton* control = dynamic_cast<ofxSimpleGuiButton*>(args);
		m.addIntArg((int)(*control->value)? 1:0);
	}
	else if (control->controlType=="Title")
	{
		ofxSimpleGuiTitle* control = dynamic_cast<ofxSimpleGuiTitle*>(args);
		m.addIntArg((int)(*control->value)? 1:0);
	}
	else if (control->controlType=="Toggle")
	{
		ofxSimpleGuiToggle* control = dynamic_cast<ofxSimpleGuiToggle*>(args);
		m.addIntArg((int)(*control->value)? 1:0);
	}

	else if (control->controlType=="Slider2D")
	{
		ofxSimpleGuiSlider2d* control = dynamic_cast<ofxSimpleGuiSlider2d*>(args);
		ofPoint value2d(*control->value);
		m.addFloatArg(value2d.x);
		m.addFloatArg(value2d.y);
	}
/*
	else if (control->controlType=="QuadWarp")
	{}
	else if (control->controlType=="MovieSlider")
	{}
	else if (control->controlType=="Content")
	{}
	else if (control->controlType=="SliderContent")
	{}
*/
	for (int i=0; i<oscExporters.size(); i++)
		oscExporters[i].send(m);
}
#endif

#ifdef USE_LIBMEMCACHED
static void populate_vector(vector<char> &vec, const string &str)
{
	vec.reserve(str.length());
	vec.assign(str.begin(), str.end());
}

//--------------------------------------------------------------
Gui*
GuiManager::get(string key)
{
	vector<char> buffer;

//	cout << "get:" << key << endl;
	memc.get(key, buffer);
	if (buffer.size() > 0)
	{
		Gui* gui_pb = new Gui();
		gui_pb->ParseFromArray(&buffer[0], buffer.size());
		return gui_pb;
	}
	return NULL;
}
#endif


//--------------------------------------------------------------
GuiPage*
GuiManager::recv(Gui& gui_pb)
{
	if (!gui_pb.IsInitialized())
		return NULL;

	using google::protobuf::FieldDescriptorProto;

	GuiPage* new_gui = new GuiPage(gui_pb.name());
	new_gui->addTitle(gui_pb.name());

	ofxSimpleGuiControl* new_control;
	for (int i=0; i<gui_pb.controls_size(); i++) {
		const Gui::Control& control = gui_pb.controls(i);

		switch (control.value_type())
		{
			case FieldDescriptorProto::TYPE_BOOL:
				new_control = &new_gui->addToggle(control.name(), *(new bool));
				break;
			case FieldDescriptorProto::TYPE_STRING:
				new_control = &new_gui->addTitle(control.name());
				break;

			case FieldDescriptorProto::TYPE_DOUBLE:
				//new_control = new_gui->addSlider(control.name(), *(new double), control.min(), control.max());
				//break;
			case FieldDescriptorProto::TYPE_FLOAT:
				new_control = &new_gui->addSlider(control.name(), *(new float), control.min(), control.max());
				break;

			case FieldDescriptorProto::TYPE_INT32:
			case FieldDescriptorProto::TYPE_INT64:
			case FieldDescriptorProto::TYPE_SINT32:
			case FieldDescriptorProto::TYPE_SINT64:
			case FieldDescriptorProto::TYPE_FIXED32:
			case FieldDescriptorProto::TYPE_FIXED64:
			case FieldDescriptorProto::TYPE_SFIXED32:
			case FieldDescriptorProto::TYPE_SFIXED64:
			case FieldDescriptorProto::TYPE_UINT32:
			case FieldDescriptorProto::TYPE_UINT64:
				new_control = &new_gui->addSlider(control.name(), *(new int), control.min(), control.max());
				break;
/*
			case FieldDescriptorProto::TYPE_BYTES:
			break;
			case FieldDescriptorProto::TYPE_GROUP:
			break;
			case FieldDescriptorProto::TYPE_MESSAGE:
			break;
*/
			default:
				break;
		}
		if (control.has_key())
			new_control->setKey(control.key());

#ifdef USE_OSC_OUT
		ofxSimpleGuiControl& _control(*new_control);
		ofAddListener(_control.valueChangedEvt, this, &GuiManager::controlValueChanged);
#endif
	}
	return new_gui;
}

#ifdef NOTIFY_FIDUCIAL_STATUS
//--------------------------------------------------------------
void
GuiManager::fiducialFound(fiducialEvtArgs &args)
{
	ofxFiducial		&fiducial	= *args.fiducial;
	int				fiducialId	= fiducial.getId();

	if (graph.find(fiducialId) == graph.end())
	{
		Gui* gui_pb = get(baseName + "/fiducials/" + ofToString(fiducialId));
		if (gui_pb != NULL)
		{
			GuiPage* add_gui = recv(*gui_pb);
			if (add_gui != NULL)
				graph.add(fiducialId, add_gui);
		}
	}

	graph.fiducialFound(args);
}

//--------------------------------------------------------------
void
GuiManager::fiducialLost(fiducialEvtArgs &args)
{
	graph.fiducialLost(args);
}

//--------------------------------------------------------------
void
GuiManager::fiducialUpdated(fiducialEvtArgs &args)
{
	graph.fiducialUpdated(args);
}
#endif

#ifdef NOTIFY_FIDUCIAL_INTERSECTION
//--------------------------------------------------------------
void
GuiManager::fiducialRayTermination(fiducialRayIntersectionEvtArgs &args)
{
	//	graph.fiducialRayTermination(args);
}

//--------------------------------------------------------------
void
GuiManager::fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args)
{}

//--------------------------------------------------------------
void
GuiManager::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{}

//--------------------------------------------------------------
void
GuiManager::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{}
#endif

} // gui
} // ofxProtobufBacked
