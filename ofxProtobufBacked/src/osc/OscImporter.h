#pragma once

#include "ofxMSAInteractiveObject.h"
#include "ofxOsc.h"

namespace ofxProtobufBacked {
    namespace osc {

        class OscImporter : public ofxMSAInteractiveObject
        {
        public:
            virtual void recv(ofxOscMessage& m) {};

        protected:
            OscImporter();

            void setup();
            void update();

            ofxOscReceiver receiver;
            unsigned short port;
        };
    }
}
