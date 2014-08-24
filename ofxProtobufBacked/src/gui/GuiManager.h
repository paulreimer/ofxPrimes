#pragma once

#define	USE_LIBMEMCACHED
#define	USE_OSC_OUT
#undef	USE_OSC_IN

#include "event_types.h"

#include "GuiGraph.h"
#include "ofxProtobufBackedGui.pb.h"

#ifdef USE_OSC_IN
#include "../osc/OscImporter.h"
#endif

#ifdef USE_OSC_OUT
#include "../osc/OscExporter.h"
#endif

#ifdef USE_LIBMEMCACHED
#include "memcached.hpp"
#endif

namespace ofxProtobufBacked {
    namespace gui {

        struct socket_t
        {
            std::string	host;
            int			port;
        };

        class GuiManager
        {
        public:
            GuiManager();
//			~GuiManager();
            void setup();

            string baseName;

            ofxFiducialBacked::gui::GuiPage* recv(protobuf::Gui& gui_pb);

#ifdef USE_LIBMEMCACHED
            protobuf::Gui* get(string key);

            vector<socket_t> memcachedSockets;
            memcache::Memcache memc;
#endif

#ifdef USE_OSC_IN
            protobuf::Gui* get(ofxOscMessage& m);

            vector<socket_t> oscInSockets;
            vector<osc::OscImporter> oscImporters;
#endif

#ifdef USE_OSC_OUT
            vector<socket_t> oscOutSockets;
            vector<osc::OscExporter> oscExporters;

            void controlValueChanged(controlEvtArgs& args);
#endif

#ifdef NOTIFY_FIDUCIAL_STATUS
            void fiducialFound	(ofxFiducialBacked::types::events::fiducialEvtArgs &args);
            void fiducialLost	(ofxFiducialBacked::types::events::fiducialEvtArgs &args);
            void fiducialUpdated(ofxFiducialBacked::types::events::fiducialEvtArgs &args);
#endif

#ifdef NOTIFY_FIDUCIAL_INTERSECTION
            void fiducialRayTermination			(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);

            void fiducialRayIntersectionFound	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
            void fiducialRayIntersectionLost	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
            void fiducialRayIntersectionUpdated	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
#endif

        private:
            ofxFiducialBacked::gui::GuiGraph graph;
        };
    }
}

