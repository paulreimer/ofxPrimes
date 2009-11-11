#pragma once

extern "C" {
#include <btstack/utils.h>
}

#include "ofMain.h"

class ofxBtStack
{
public:
	ofxBtStack();
//	virtual ~ofxBtStack();
	void start();
	void stop();
	
	bd_addr_t addr;

	void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

	ofEvent<uint8_t*>	startup;
	ofEvent<uint8_t*>	startup_failure;
	ofEvent<uint8_t*>	pin_request;

	ofEvent<uint8_t*>	hci_connection_closed;
	ofEvent<uint8_t*>	hci_command_complete;

	ofEvent<uint8_t*>	l2cap_data_packet;
	ofEvent<uint8_t*>	l2cap_channel_opened;
	ofEvent<uint8_t*>	l2cap_channel_failed;

	ofEvent<char*>		new_state;
	
private:
	bool bRunLoopEnabled;
};
