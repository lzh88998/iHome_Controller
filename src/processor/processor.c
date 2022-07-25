#include "processor.h"
#include "stc12c5a60s2.h"
#include "..\global.h"
#include "..\eeprom\eeprom.h"
#include "..\buffer\buffer.h"

enum PROCESSOR_STATUS {
	PROCESSOR_NORMAL = 0,

	PROCESSOR_WAIT_GATEWAY_0 = 1,
	PROCESSOR_WAIT_GATEWAY_1 = 2,
	PROCESSOR_WAIT_GATEWAY_2 = 3,
	PROCESSOR_WAIT_GATEWAY_3 = 4,

	PROCESSOR_WAIT_MASK_0 = 5,
	PROCESSOR_WAIT_MASK_1 = 6,
	PROCESSOR_WAIT_MASK_2 = 7,
	PROCESSOR_WAIT_MASK_3 = 8,

	PROCESSOR_WAIT_MAC_0 = 9,
	PROCESSOR_WAIT_MAC_1 = 10,
	PROCESSOR_WAIT_MAC_2 = 11,
	PROCESSOR_WAIT_MAC_3 = 12,
	PROCESSOR_WAIT_MAC_4 = 13,
	PROCESSOR_WAIT_MAC_5 = 14,

	PROCESSOR_WAIT_IP_0 = 15,
	PROCESSOR_WAIT_IP_1 = 16,
	PROCESSOR_WAIT_IP_2 = 17,
	PROCESSOR_WAIT_IP_3 = 18,
	
	PROCESSOR_WAIT_CHECKSUM = 19,
};

static unsigned char g_processor_status = PROCESSOR_NORMAL;

static code unsigned char g_pin_info[32] = {
																				0x24, 0x23, 0x22, 0x21,
																				0x20, 0x40, 0x37, 0x36, 
																				0x35, 0x34, 0x33, 0x25, 
																				0x26, 0x27, 0x44, 0x45, 
																				
																				0x41, 0x46, 0x07, 0x06, 
																				0x05, 0x04, 0x03, 0x02, 
																				0x01, 0x00, 0x42, 0x10, 
																				0x11, 0x12, 0x13, 0x14, 
																			};

extern bit main_int_reset;

void update_status(unsigned char dat) {
	unsigned char item, v;

//	DEBUG_PRINT("Update_status! 0x%bx\r\n", dat);
	if(PROCESSOR_NORMAL == g_processor_status) {
		if(0x80 & dat){
			// System config change
			DEBUG_PRINT("Update Status!\r\n");
			BUFFER_ADD(dat);
			DEBUG_PRINT("Start Update Item!\r\n");
			g_processor_status = PROCESSOR_WAIT_GATEWAY_0;
		} else if(0x40 & dat) { // Query status
			item = g_pin_info[dat & 0x1F];
			switch(item & 0xF0) {
				case 0x00:
					v = P0;
					break;
				case 0x10:
					v = P1;
					break;
				case 0x20:
					v = P2;
					break;
				case 0x30:
					v = P3;
					break;
				case 0x40:
					v = P4;
					break;
			}
			if(v & (1 << (item & 0x07)))
				dat |= 0x20;
			else
				dat &= 0xDF;
			BUFFER_ADD(dat);
		} else { // set pin status
			DEBUG_PRINT("Set PIN STATUS: %bx\r\n", dat);
			BUFFER_ADD(dat);
			item = g_pin_info[dat & 0x1F];
			DEBUG_PRINT("Set PIN ITEM: %bx\r\n", item);
			v = 1 << (item & 0x07);

			if(dat & 0x20) {
				switch(item & 0xF0) {
					case 0x00:
						P0 |= v;
						break;
					case 0x10:
						P1 |= v;
						break;
					case 0x20:
						P2 |= v;
						break;
					case 0x30:
						P3 |= v;
						break;
					case 0x40:
						P4 |= v;
						break;
				}
			} else {
				switch(item & 0xF0) {
					case 0x00:
						P0 &= ~v;
						break;
					case 0x10:
						P1 &= ~v;
						break;
					case 0x20:
						P2 &= ~v;
						break;
					case 0x30:
						P3 &= ~v;
						break;
					case 0x40:
						P4 &= ~v;
						break;
				}
			}
		}
	} else { // setting ip address related items
		DEBUG_PRINT("Set Item: %bd\r\n", g_processor_status);
		if(PROCESSOR_WAIT_CHECKSUM == g_processor_status) {
			v = 0;
			for(item = ETHERNET_EEPROM_GATEWAY; item < ETHERNET_EEPROM_PORT_COUNT; item++) {
				v += eeprom[item];
			}
			
			if(v == dat) {
				for(item = ETHERNET_EEPROM_GATEWAY; item < ETHERNET_EEPROM_PORT_COUNT; item++)
					BUFFER_ADD(eeprom[item]);

				EEPROM_Save();
				main_int_reset = 1;
			} else {
				// revert eeprom content
				EEPROM_Load();
				BUFFER_ADD(0xFF);
				BUFFER_ADD(v);
				BUFFER_ADD(dat);
				for(item = ETHERNET_EEPROM_GATEWAY; item < ETHERNET_EEPROM_PORT_COUNT; item++) {
					BUFFER_ADD(eeprom[item]);
				}
			}
			g_processor_status = PROCESSOR_NORMAL;
		}
		else {
			eeprom[ETHERNET_EEPROM_GATEWAY + (g_processor_status - PROCESSOR_WAIT_GATEWAY_0)] = dat;
			g_processor_status++;
		}
	}
//	DEBUG_PRINT("Update status finish!\r\n");
}
