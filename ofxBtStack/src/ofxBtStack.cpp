#include "ofxBtStack.h"
/*
 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <strings.h>
 */
extern "C" {
	#include <btstack/btstack.h>
	#include <btstack/run_loop.h>
	#include <btstack/hci_cmds.h>
}

// Global state... ewwww
ofxBtStack* _btstack = new ofxBtStack();

static void _packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
	_btstack->packet_handler(packet_type, channel, packet, size);
}

ofxBtStack::ofxBtStack()
{
	bRunLoopEnabled = false;
}

void ofxBtStack::start()
{
	if (!bRunLoopEnabled)
	{
		run_loop_init(RUN_LOOP_COCOA);
		bRunLoopEnabled = true;
	}

	bt_open();
	bt_register_packet_handler(_packet_handler);

	char *bt_started = "BT started";
	ofNotifyEvent(this->new_state, bt_started);

	bt_send_cmd(&btstack_set_power_mode, HCI_POWER_ON );
}

void ofxBtStack::stop()
{
	bt_close();
}

void ofxBtStack::packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
	switch (packet_type) {

		case L2CAP_DATA_PACKET:
			ofNotifyEvent(this->l2cap_data_packet, packet);
			break;

		case HCI_EVENT_PACKET:

			switch (packet[0]) {

				case BTSTACK_EVENT_POWERON_FAILED:
					ofNotifyEvent(this->startup_failure, packet);
					break;

				case BTSTACK_EVENT_STATE:
					if (packet[2] == HCI_STATE_WORKING)
						ofNotifyEvent(this->startup, packet);
					break;

				case HCI_EVENT_PIN_CODE_REQUEST:
					ofNotifyEvent(this->pin_request, packet);
					break;

				case L2CAP_EVENT_CHANNEL_OPENED:
					if (packet[2] == 0)
						ofNotifyEvent(this->l2cap_channel_opened, packet);
					else
						ofNotifyEvent(this->l2cap_channel_failed, packet);

					break;

				case HCI_EVENT_DISCONNECTION_COMPLETE:
					ofNotifyEvent(this->hci_connection_closed, packet);
					break;

				case HCI_EVENT_COMMAND_COMPLETE:
					ofNotifyEvent(this->hci_command_complete, packet);
					break;

				default:
					// other event
					break;
			}
			break;

		default:
			// other packet type
			break;
	}
}

