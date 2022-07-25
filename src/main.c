//#include <intrins.h>
#include <stc12c5a60s2.h>

#include "global.h"

#ifdef SERIAL_OUTPUT
#include "Serial\serial.h"
#endif

#include "timer\timer.h"
#include "SPI\spi.h"
#include "Ethernet\ethernet.h"

bit main_int_timer = 0;
bit main_int_net = 0;
bit main_int_reset = 0;
bit main_network_cnt = 0;

void exint0() interrupt 0 using 2
{
	main_int_net = 1;
}

void main()
{
	
	P4SW |= 0x70; // put all P4 pin in IO mode

	IT0 = 0;                        //set INT0 int type (1:Falling 0:Low level)
	EX0 = 1;                        //enable INT0 interrupt
	EA = 1;                         //open global interrupt switch

#ifdef SERIAL_OUTPUT
	SERIAL_INIT();
#endif

	Timer_Init();
	DEBUG_PRINT("Timer Initialized!\r\n");
	
	SPI_INIT();
	DEBUG_PRINT("SPI Initialized!\r\n");

	Ethernet_Init();
	DEBUG_PRINT("Ethernet Initialized!\r\n");
		
	while(1) {
		if(main_int_timer) {
			DEBUG_PRINT("Timer\r\n");
			if(0 == main_network_cnt){ // for 1 timer period ~600ms @ 11M
				if(Ethernet_IsReset() || Ethernet_IsLinkDown()) {
					DEBUG_PRINT("W5500 reset or link down!\r\n");
					Ethernet_Init();
				}
			}
			main_network_cnt = 0;
			main_int_timer = 0;
		}
		
		if(main_int_net) {
			DEBUG_PRINT("W5500 INT!\r\n");
			main_network_cnt = 1;
			Ethernet_Process_Int();
			main_int_net = 0;
		}
		
		if(main_int_reset) {
			DEBUG_PRINT("W5500 reset due to cmd!\r\n");
			Ethernet_Init();
			main_int_reset = 0;
		}
	}
}
