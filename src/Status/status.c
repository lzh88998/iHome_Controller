#include "status.h"
#include "custom.h"
#include "..\EEPROM\eeprom.h"

#define EEPROM_CUSTOME 0x0200

unsigned long PINSTATUS;
unsigned char KEEPSTATUS;

void Status_Init()
{
	unsigned char i = 0;
	PINSTATUS = 0;
	KEEPSTATUS = 0;

	do
	{
		PINSTATUS <<= 8;
		PINSTATUS |= EEPROM_read(EEPROM_CUSTOME + i);
		i++;
	}
	while(i < 4);

	for(i = 0; i < 32; i++)
		  Update_Status(i, 0x01 &  (PINSTATUS >> i));

	KEEPSTATUS = EEPROM_read(EEPROM_CUSTOME + i);
}

unsigned char Get_Keep_Status()
{
	return KEEPSTATUS;
} 

void Keep_Status(unsigned char keep)
{
	KEEPSTATUS = keep;

	if(!KEEPSTATUS)
		PINSTATUS = 0;

	Save_Status();
}

void Update_Status(unsigned char pin, unsigned char status)
{
	switch(pin)
	{
		case 0:
			CP0 = (status != 0);
			break;
		case 1:
			CP1 = (status != 0);
			break;
		case 2:
			CP2 = (status != 0);
			break;
		case 3:
			CP3 = (status != 0);
			break;
		case 4:
			CP4 = (status != 0);
			break;
		case 5:
			CP5 = (status != 0);
			break;
		case 6:
			CP6 = (status != 0);
			break;
		case 7:
			CP7 = (status != 0);
			break;
		case 8:
			CP8 = (status != 0);
			break;
		case 9:
			CP9 = (status != 0);
			break;
		case 10:
			CP10 = (status != 0);
			break;
		case 11:
			CP11 = (status != 0);
			break;
		case 12:
			CP12 = (status != 0);
			break;
		case 13:
			CP13 = (status != 0);
			break;
		case 14:
			CP14 = (status != 0);
			break;
		case 15:
			CP15 = (status != 0);
			break;
		case 16:
			CP16 = (status != 0);
			break;
		case 17:
			CP17 = (status != 0);
			break;
		case 18:
			CP18 = (status != 0);
			break;
		case 19:
			CP19 = (status != 0);
			break;
		case 20:
			CP20 = (status != 0);
			break;
		case 21:
			CP21 = (status != 0);
			break;
		case 22:
			CP22 = (status != 0);
			break;
		case 23:
			CP23 = (status != 0);
			break;
		case 24:
			CP24 = (status != 0);
			break;
		case 25:
			CP25 = (status != 0);
			break;
		case 26:
			CP26 = (status != 0);
			break;
		case 27:
			CP27 = (status != 0);
			break;
		case 28:
			CP28 = (status != 0);
			break;
		case 29:
			CP29 = (status != 0);
			break;
		case 30:
			CP30 = (status != 0);
			break;
		case 31:
			CP31 = (status != 0);
			break;
	}

	if(status != 0)
		PINSTATUS |= ((unsigned long)1 << pin);
	else
		PINSTATUS &= ~((unsigned long)1 << pin); 

	if(KEEPSTATUS)
		Save_Status();
}

void Save_Status()
{
	int i = 0;

	EEPROM_SectorErase(EEPROM_CUSTOME);
	
	do
	{
		EEPROM_write(EEPROM_CUSTOME + i, 0xFF & (PINSTATUS >> ((3-i)*8)));
		i++;
	}
	while(i < 4);

	// Keep status
	EEPROM_write(EEPROM_CUSTOME + i, KEEPSTATUS);
}

unsigned char Get_Status(unsigned char pin)
{
	return (PINSTATUS >> pin) & 0x00000001L;
}
