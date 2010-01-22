#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxOsc.h"

namespace ofxProtobufBacked {
    namespace osc {

        class OscExporter : public ofxMSAInteractiveObject
        {
        public:
            void send(ofxOscMessage& m);

            OscExporter();
            void setup();

			string			host;
            unsigned short	port;

        protected:
            ofxOscSender	sender;
        };
    }
}
