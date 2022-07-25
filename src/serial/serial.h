#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "..\global.h"

#include "stdio.h"

#define SERIAL_BAUD_RATE	9600L
#define RELOAD (256 - (MAIN_FOSC/SERIAL_BAUD_RATE/32/(AUXR & 0x04 ? 1 : 12)*(PCON & 0x80 ? 2 : 1)))

#define SERIAL_INIT()					SCON |= 0x50;\
															AUXR |= 0x15;\
															PCON |= 0x80;\
															BRT = RELOAD;\
															EA = 1;\
															SBUF=' ';
															
#define SERIAL_SEND_DATA(dat)	while(0 == TI);TI = 0;SBUF = dat;  
#define SERIAL_SEND_STRING(s)	while(*s) { SERIAL_SEND_DATA(*s++); };

#endif