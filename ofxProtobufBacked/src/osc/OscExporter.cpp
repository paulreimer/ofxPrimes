#include "OscExporter.h"

#ifndef DEFAULT_OSC_OUT_PORT
#define DEFAULT_OSC_OUT_PORT 3334
#endif

#ifndef DEFAULT_OSC_HOST
#define DEFAULT_OSC_HOST "localhost"
#endif

namespace ofxProtobufBacked {
namespace osc {

//--------------------------------------------------------------
OscExporter::OscExporter()
{
	host = DEFAULT_OSC_HOST;
	port = DEFAULT_OSC_OUT_PORT;
}

//--------------------------------------------------------------
void OscExporter::setup()
{
	sender.setup( host, port );
}

//--------------------------------------------------------------
void OscExporter::send(ofxOscMessage& m)
{
	cout << "Send OSC message to " << m.getAddress() << endl;
	sender.sendMessage(m);
}

} // osc
} // ofxProtobufBacked
