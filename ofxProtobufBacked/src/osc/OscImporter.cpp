#include "OscImporter.h"

#ifndef DEFAULT_OSC_IN_PORT
#define DEFAULT_OSC_IN_PORT 3333
#endif

namespace ofxProtobufBacked {
namespace osc {

//--------------------------------------------------------------
OscImporter::OscImporter()
{
	port = DEFAULT_OSC_IN_PORT;
}

//--------------------------------------------------------------
void OscImporter::setup()
{
	receiver.setup(port);
}

//--------------------------------------------------------------
void OscImporter::update()
{
	ofxOscMessage m;

	while( receiver.hasWaitingMessages() )
	{
		receiver.getNextMessage( &m );
		recv(m);
	}
}

} // osc
} // ofxProtobufBacked
