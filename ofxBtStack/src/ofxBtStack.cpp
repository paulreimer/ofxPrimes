#pragma once

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
ofxBtStack* _btstack;// = ofxBtStack();

static void _packet_handler(uint8_t packet_type, uint8_t *packet, uint16_t size)
{
	_btstack->packet_handler(packet_type, packet, size);
}

ofxBtStack::ofxBtStack()
{
}

void ofxBtStack::init()
{
	run_loop_init(RUN_LOOP_COCOA);
	bt_open();
	bt_register_packet_handler(_packet_handler);
	newState("BT started");
	bt_send_cmd(&btstack_set_power_mode, HCI_POWER_ON );
}

/*
ofxBtStack::~ofxBtStack()
{
	destroy();
}

void ofxBtStack::threadedFunction() 
{
	while( isThreadRunning() ) {
		update();
		
		if (fps>0)
			ofSleepMillis((1 * 1000)/fps);
	}
}

void ofxBtStack::destroy() 
{
	stopThread();	
}
*/

void ofxBtStack::packet_handler(uint8_t packet_type, uint8_t *packet, uint16_t size)
{
	bd_addr_t event_addr;
	uint16_t psm;
	uint16_t source_cid;
	
	switch (packet_type) {
			
		case L2CAP_DATA_PACKET:
			// just dump data for now
			// hexdump( packet, size );
			if (packet[8] == 0xa1 && packet[9] == 0x31)
			{
				//				(*data_cb)(packet[12], packet[13], packet[14]);
				newData(packet[12], packet[13], packet[14]);
			}
			break;
			
		case HCI_EVENT_PACKET:
			
			switch (packet[0]){
					
				case BTSTACK_EVENT_STATE:
					// bt stack activated, get started - set local name
					if (packet[2] == HCI_STATE_WORKING) {
						bt_send_cmd(&hci_write_local_name, "ofxBtStack");
						newState("BT running");
					}
					break;
					
				case HCI_EVENT_PIN_CODE_REQUEST:
					// inform about pin code request
					printf("Please enter PIN 1234 on remote device\n");
					break;
					
				case L2CAP_EVENT_CHANNEL_OPENED:
					// inform about new l2cap connection
					bt_flip_addr(event_addr, &packet[2]);
					psm			= READ_BT_16(packet, 10); 
					source_cid	= READ_BT_16(packet, 12); 
					printf("Channel successfully opened: ");
					print_bd_addr(event_addr);
					printf(", handle 0x%02x, psm 0x%02x, source cid 0x%02x, dest cid 0x%02x\n",
						   READ_BT_16(packet, 8), psm, source_cid,  READ_BT_16(packet, 14));
					
					if (psm == 0x13) {
						// interupt channel openedn succesfully, now open control channel, too.
						bt_send_cmd(&l2cap_create_channel, event_addr, 0x11);
					} else {
						// request acceleration data.. probably has to be sent to control channel 0x11 instead of 0x13
//						uint8_t setMode31[] = { 0x52, 0x12, 0x00, 0x31 };
//						bt_send_l2cap( source_cid, setMode31, sizeof(setMode31));

						uint8_t setRumble[] = { 0x52, 0x11, 0x11 };
						bt_send_l2cap( source_cid, setRumble, sizeof(setRumble));

//						uint8_t setLEDs[] = { 0x52, 0x11, 0x10 };
//						bt_send_l2cap( source_cid, setLEDs, sizeof(setLEDs));
						newState("WiiMote connected");
					}
					
				case HCI_EVENT_COMMAND_COMPLETE:
					// use pairing yes/no
					if ( COMMAND_COMPLETE_EVENT(packet, hci_write_local_name) ) {
						bt_send_cmd(&hci_write_authentication_enable, 0);
					}
					
					// connect to HID device (PSM 0x13) at addr
					if ( COMMAND_COMPLETE_EVENT(packet, hci_write_authentication_enable) ) {
						newState("Connecting to WiiMote");
						bt_send_cmd(&l2cap_create_channel, addr, 0x13);
					}
					break;
					
				default:
					break;
			}
			
		default:
			break;
	}
}

