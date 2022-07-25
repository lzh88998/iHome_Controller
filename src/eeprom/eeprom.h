#ifndef _EEPROM_H_
#define _EEPROM_H_

#define ETHERNET_EEPROM_GATEWAY			1
#define ETHERNET_EEPROM_SUBNET			5
#define ETHERNET_EEPROM_MAC_ADDR		9
#define ETHERNET_EEPROM_IP_ADDR			15
#define ETHERNET_EEPROM_PORT_COUNT	19
#define ETHERNET_EEPROT_PORT_START	20

#define ETHERNET_EEPROM_MAX_SIZE		22	// Should be 36 for 8 ports, 22 only support 1 ports

void			EEPROM_Load();
void 			EEPROM_Save();
void			EEPROM_Clear();

extern unsigned char eeprom[ETHERNET_EEPROM_MAX_SIZE];
#endif