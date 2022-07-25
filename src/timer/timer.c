#include "timer.h"
#include "..\global.h"
#include <stc12c5a60s2.h>

unsigned char t1_counter = 0;
extern bit main_int_timer;

void Timer_Init() {
	AUXR &= 0x7F;	
	TMOD &= 0xF0;	
	TMOD |= 0x01;	
	TH0 = 0x28;
	TL0 = 0x00;
	TR0 = 1;

	ET0 = 1;
	EA = 1;
}

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1 using 1
{
	TH0 = 0x28;
	TL0 = 0x00;
	
	t1_counter++;
	if(10 == t1_counter) {
		t1_counter = 0;
		main_int_timer = 1;
	}
}
