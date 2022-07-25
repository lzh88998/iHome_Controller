#include "eeprom.h"
#include <stc12c5a60s2.h>
#include <intrins.h>

#include "../global.h"

sfr ISP_DATA  = 0xC2;
sfr ISP_ADDRH = 0xC3;
sfr ISP_ADDRL = 0xC4;
sfr ISP_CMD   = 0xC5;
sfr ISP_TRIG  = 0xC6;
sfr ISP_CONTR = 0xC7;

//sfr ISP_CMD   = 0xC5;
#define		ISP_STANDBY()	ISP_CMD = 0
#define		ISP_READ()		ISP_CMD = 1
#define		ISP_WRITE()		ISP_CMD = 2
#define		ISP_ERASE()		ISP_CMD = 3

//sfr ISP_TRIG  = 0xC6;
#define 	ISP_TRIG()	ISP_TRIG = 0x5A,	ISP_TRIG = 0xA5

//							  7    6    5      4    3    2    1     0    Reset Value
//sfr IAP_CONTR = 0xC7;		IAPEN SWBS SWRST CFAIL  -   WT2  WT1   WT0   0000,x000	//IAP Control Register
#define ISP_EN			0x80
#define ISP_SWBS		0x40
#define ISP_SWRST		0x20
#define ISP_CMD_FAIL	0x10
#define ISP_WAIT_1MHZ	7
#define ISP_WAIT_2MHZ	6
#define ISP_WAIT_3MHZ	5
#define ISP_WAIT_6MHZ	4
#define ISP_WAIT_12MHZ	3
#define ISP_WAIT_20MHZ	2
#define ISP_WAIT_24MHZ	1
#define ISP_WAIT_30MHZ	0

#define	ISP_Fosc	MAIN_FOSC
#if (ISP_Fosc >= 24000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_30MHZ
#elif (ISP_Fosc >= 20000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_24MHZ
#elif (ISP_Fosc >= 12000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_20MHZ
#elif (ISP_Fosc >= 6000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_12MHZ
#elif (ISP_Fosc >= 3000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_6MHZ
#elif (ISP_Fosc >= 2000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_3MHZ
#elif (ISP_Fosc >= 1000000L)
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_2MHZ
#else
	#define		ISP_WAIT_FREQUENCY	ISP_WAIT_1MHZ
#endif

#define		ISP_ENABLE()	ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY)
#define		ISP_DISABLE()	ISP_CONTR = 0; ISP_CMD = 0; ISP_TRIG = 0; ISP_ADDRH = 0xff; ISP_ADDRL = 0xff

#define ETHERNET_EEPROM_START				0x0000

unsigned char eeprom[ETHERNET_EEPROM_MAX_SIZE] = 	{ 
																										0xAA,
																										0xC0, 0xA8, 0x64, 0x01,	// Gateway
																										0xFF, 0xFF, 0xFF, 0x00, // Subnet
																										0x0C, 0x29, 0xAB, 0x7D, 0x01, 0xA0, // MAC
																										0xC0, 0xA8, 0x64, 0x64, // IP
																										1, // Port Count
																										0x13, 0x88 // Port Number for Socket 1 5000
																															 // Other ports up to 8 ports and total 36 bytes in eeprom
																									};

#define EEPROM_READ(EE_address, dat)				EA=0;\
																						ISP_ENABLE();\
																						ISP_READ();\
																						ISP_ADDRH=EE_address>>8;\
																						ISP_ADDRL=EE_address&0xFF;\
																						ISP_TRIG();\
																						_nop_();\
																						dat = ISP_DATA;\
																						ISP_DISABLE();\
																						EA = 1;

#define EEPROM_ERASE_SECTOR(EE_address)			EA=0;\
																						ISP_ADDRH=EE_address>>8;\
																						ISP_ADDRL=EE_address&0xFF;\
																						ISP_ENABLE();\
																						ISP_ERASE();\
																						ISP_TRIG();\
																						ISP_DISABLE();\
																						EA = 1;

#define EEPROM_WRITE(EE_address, dat)				EA = 0;\
																						ISP_ENABLE();\
																						ISP_WRITE();\
																						ISP_ADDRH=EE_address>>8;\
																						ISP_ADDRL=EE_address&0xFF;\
																						ISP_DATA=dat;\
																						ISP_TRIG();\
																						_nop_();\
																						ISP_DISABLE();\
																						EA = 1;

void EEPROM_Load() {
	unsigned char i;
	EEPROM_READ(ETHERNET_EEPROM_START, i);
	if(0xAA == i) {
		// Valid signarture in EEPROM, load all EEPROM into memory
		DEBUG_PRINT("EEPROM load from flash\r\n");
		for(i = 0; i < ETHERNET_EEPROM_MAX_SIZE; i++) {
			EEPROM_READ(ETHERNET_EEPROM_START + i, eeprom[i]);
		}
	} else {
		DEBUG_PRINT("EEPROM using defaults\r\n");
	}
}

void EEPROM_Save() {
	unsigned char i;
	DEBUG_PRINT("EEPROM save to flash\r\n");
	EEPROM_ERASE_SECTOR(ETHERNET_EEPROM_START);
	for(i = 0; i < ETHERNET_EEPROM_MAX_SIZE; i++) {
		EEPROM_WRITE(ETHERNET_EEPROM_START + i, eeprom[i]);
	}
}

void EEPROM_Clear() {
	EEPROM_ERASE_SECTOR(ETHERNET_EEPROM_START);
}
