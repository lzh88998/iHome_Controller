#include "..\global.h"
#include "w5500_registers.h"
#include "ethernet.h"

#include "callback.h"
#include "..\eeprom\eeprom.h"
#include "..\buffer\buffer.h"

#ifdef SERIAL_OUTPUT
#include "..\serial\serial.h"
#endif

#ifndef W5500_ON_DATA
#define W5500_ON_DATA(s, dat)				DEBUG_PRINT("Recv on sock %bd, 0x%bx\r\n", s, dat);
#endif

//#define S_RX_SIZE									2		/*定义Socket接收缓冲区的大小，可以根据W5500_RMSR的设置修改 */
#define S_TX_SIZE										2  	/*定义Socket发送缓冲区的大小，可以根据W5500_TMSR的设置修改 */

static unsigned ethernet_enabled_port_cnt = 0;

unsigned char Ethernet_Socket_Close(SOCKET socket)
{
	unsigned char i, dat;
	DEBUG_PRINT("Closing socket %bd!\r\n", socket);
	
	W5500_WRITE_SOCKET_BYTE(socket, Sn_CR, CLOSE);

	DEBUG_PRINT("Socket %bd command sent!\r\n", socket);

	for(i = 0; i < 10; i++)
	{
		W5500_READ_SOCKET_BYTE(socket, Sn_SR, dat);
		if(dat == SOCK_CLOSED)
			break;
	}

	if(i == 10) {
		DEBUG_PRINT("Socket %bd failed to close!\r\n", socket);
		return 0;
	}

	DEBUG_PRINT("Socket %bd closed!\r\n", socket);
	return 1;
}

unsigned char Ethernet_Socket_Listen(SOCKET socket)
{
	unsigned char i, dat;
	
	W5500_WRITE_SOCKET_BYTE(socket, Sn_MR, MR_TCP);
	W5500_WRITE_SOCKET_BYTE(socket, Sn_CR, OPEN);

	DEBUG_PRINT("Opening socket %bd!\r\n", socket);
	for(i = 0; i < 10; i++)
	{ // Wait for socket to become init status
		W5500_READ_SOCKET_BYTE(socket, Sn_SR, dat);
		if(dat == SOCK_INIT)
			break;
	}

	if(i == 10) {
		DEBUG_PRINT("Opening socket %bd failed!\r\n", socket);
		Ethernet_Socket_Close(socket);
		return 0;
	}

	DEBUG_PRINT("Listening socket %bd!\r\n", socket);
	W5500_WRITE_SOCKET_BYTE(socket, Sn_CR, LISTEN);

	for(i = 0; i < 10; i++)
	{
		W5500_READ_SOCKET_BYTE(socket, Sn_SR, dat);
		if(dat == SOCK_LISTEN)
			break;
	}

	if(i == 10) {
		DEBUG_PRINT("Listening socket %bd failed!\r\n", socket);
		Ethernet_Socket_Close(socket);
		return 0;
	}

	DEBUG_PRINT("Listening socket %bd succeed!\r\n", socket);

	return 1;
}

void Ethernet_Init()
{
	unsigned char i, temp, simr=0;
	
#ifdef TEST_ENV
	P1M0 |= 0x10;	// P1^4 for BTL (W5500_RST)
	P1M1 &= 0xEF;
	if(0 == P20)
#else
	P4M0 |= 0x08; // P4^3 for BTL (W5500_RST)
	P4M1 &= 0xF7;
	if(0 == P30)
#endif
		EEPROM_Clear();
	
	EEPROM_Load();

	DEBUG_PRINT("Executing Hardware Reset!\r\n");
	// Hard reset
	W5500_HARD_RESET();

	DEBUG_PRINT("Executing Software Reset!\r\n");
	// Soft reset for chip
	W5500_WRITE_COMMON_REG_BYTE(MR, RST)

	DEBUG_PRINT("Waiting Reset Complete!\r\n");

	// Wait for reset done
	do
	{
		W5500_READ_COMMON_REG_BYTE(MR, temp);
	}
	while(temp & RST);

	DEBUG_PRINT("Waiting PHY Ready!\r\n");

	// Wait for PHY ready
	do
	{
		W5500_READ_COMMON_REG_BYTE(PHYCFGR, temp);
	}
	while(!(temp & RST_PHY) || !(temp & LINK)); // wait for hardware initialized

	DEBUG_PRINT("PHY Ready!\r\n");

	// Set TX/RX buffer size
	for(i = 0; i < 8; i++)
	{ 	
		// RX
		W5500_WRITE_SOCKET_BYTE(i, Sn_RXBUF_SIZE, 0 == i ? 16 : 0);
		// TX
		W5500_WRITE_SOCKET_BYTE(i, Sn_TXBUF_SIZE, S_TX_SIZE);
	}

	// send retry count (0x07D0 is 200 ms. Unit is 100us)
	W5500_WRITE_COMMON_REG_SHORT(RTR, 0x07, 0xD0);

	// send retry count
	W5500_WRITE_COMMON_REG_BYTE(RCR, 3);
	
	// Set Gateway, Subnet, MAC, Local IP
	// Load Gateway 	0x0001-0x0004
	// Load Subnet		0x0005-0x0008
	// Load MAC			0x0009-0x000E
	// Load Local IP	0x000F-0x0012
	W5500_LOCATE_REGISTER(GAR, COMMON_R, W5500_WRITE);
	for(i = 1; i < ETHERNET_EEPROM_PORT_COUNT; i++)
	{
		temp = eeprom[i];

		W5500_WRITE_DATA(temp);
	}
	W5500_FINISH_READ_WRITE();

	// Initialize ports
	// Read port count from EEPROM
	ethernet_enabled_port_cnt = eeprom[ETHERNET_EEPROM_PORT_COUNT];
	
	temp = 0;
	simr = 0;
	while(temp < ethernet_enabled_port_cnt)
	{
 //		W5500_WRITE_SOCKET_SHORT(temp, Sn_MSSR, 0x04, 0x1C);
		// set port number
		W5500_LOCATE_SOCKET_REGISTER(temp, Sn_PORT, W5500_WRITE); 
		W5500_WRITE_DATA(eeprom[ETHERNET_EEPROT_PORT_START+(temp * 2)]); // every port have 2 bytes so * 2
		W5500_WRITE_DATA(eeprom[ETHERNET_EEPROT_PORT_START+(temp * 2) + 1]);

		W5500_FINISH_READ_WRITE();

		// Set interrupt events note only enable RECEIVE event for first socket
		W5500_WRITE_SOCKET_BYTE(temp, Sn_IMR, IR_CON|IR_DISCON|IR_TIMEOUT| (0 == temp ? IR_RECV : 0x00));

		// Set socket auto keep alive (5s period)
		W5500_WRITE_SOCKET_BYTE(temp, Sn_KPALVTR, 1);

		// Set socket for listening
		DEBUG_PRINT("Initializing socket %bd!\r\n", temp);
		Ethernet_Socket_Listen(temp);

		simr *= 2;
		simr |= 1;

		temp ++;
	}
	
	W5500_WRITE_COMMON_REG_BYTE(SIMR, simr);
}



unsigned char Ethernet_IsLinkDown() {
	unsigned char dat;
	W5500_READ_COMMON_REG_BYTE(PHYCFGR, dat);
	return (dat & LINK) == 0;
}

unsigned char Ethernet_IsReset()
{
	unsigned char dat;
	W5500_READ_COMMON_REG_BYTE(SIPR, dat);
	return dat == 0x00;
}

void Ethernet_RecvData(SOCKET socket)
{
	bit more = 1;
	unsigned char temp, size_h, size_l, offset_h, offset_l;

	W5500_READ_SOCKET_SHORT(socket, Sn_RX_RSR, size_h, size_l);
	if(0 == size_h && 0 == size_l)
		return;
	
	if(0x04 <= size_h)
	{
		size_h = 0x04;
		size_l = 0x00;
	}
	
	W5500_READ_SOCKET_SHORT(socket, Sn_RX_RD, offset_h, offset_l);
//	W5500_LOCATE_DATA(offset_h, offset_l, 0x18, W5500_READ);
	W5500_LOCATE_DATA(offset_h, offset_l, (socket*0x20+0x18), W5500_READ);
	
	DEBUG_PRINT("Recv size: 0x%bx 0x%bx 0x%bx 0x%bx\r\n", size_h, size_l, offset_h, offset_l);

	offset_l += size_l;
	offset_h += size_h;
	if(offset_l < size_l)
		offset_h++;

	do
	{
		while(size_l) {
			DEBUG_PRINT("Size: 0x%bx 0x%bx\r\n", size_h, size_l);
			SPDAT = 0xFF;
			size_l--;
			SPSTAT = 0xC0; // SPIF|WCOL
			temp = SPDAT;
			W5500_ON_DATA(socket, temp);
		}
		
		if(size_h) {
			SPDAT = 0xFF;
			size_l--;
			SPSTAT = 0xC0; // SPIF|WCOL
			temp = SPDAT;
			W5500_ON_DATA(socket, temp);
		}
	}
	while(size_h--);
	W5500_FINISH_READ_WRITE();
	
	W5500_WRITE_SOCKET_SHORT(socket, Sn_RX_RD, offset_h, offset_l);
	W5500_WRITE_SOCKET_BYTE(socket, Sn_CR, RECV);
}

void Ethernet_Send_Data(SOCKET socket)
{
	unsigned char offset_h, offset_l, partition_idx = 0, buffer_idx = 0;
	
	if(0 == buffer_partition_idx && 0 == buffer_item_idx)
		return;

	W5500_READ_SOCKET_SHORT(socket, Sn_TX_WR, offset_h, offset_l);
	W5500_LOCATE_DATA(offset_h, offset_l, 0x10 + (socket << 5), W5500_WRITE);

	offset_l += buffer_item_idx;
	offset_h += buffer_partition_idx;
	if(buffer_item_idx > offset_l)
		offset_h++;

	while(partition_idx < buffer_partition_idx) {
		do
		{
			W5500_WRITE_DATA(buffer[partition_idx][buffer_idx]);
			buffer_idx++;
		} while (buffer_idx);
		partition_idx++;
	}
	
	do
	{
		W5500_WRITE_DATA(buffer[partition_idx][buffer_idx]);
		buffer_idx++;
	}
	while(buffer_idx < buffer_item_idx);

	W5500_FINISH_READ_WRITE();
	BUFFER_RESET();

	W5500_WRITE_SOCKET_SHORT(socket, Sn_TX_WR, offset_h, offset_l);
	W5500_WRITE_SOCKET_BYTE(socket, Sn_CR, SEND);
}

void Ethernet_Process_Int()
{
	unsigned char i = 0, intsocket, intcode;
		
	W5500_READ_COMMON_REG_BYTE(SIR, intsocket);
	DEBUG_PRINT("Int socket %bd!\r\n", intsocket);
	W5500_WRITE_COMMON_REG_BYTE(SIR, 0x00);

	for(i = 0; i < ethernet_enabled_port_cnt; i++)
	{
		if((intsocket) & 0x01)
		{
			// socket triggered int
			W5500_READ_SOCKET_BYTE(i, Sn_IR, intcode);
			DEBUG_PRINT("Int code %bd!\r\n", intcode);

			if(intcode & IR_RECV)
			{
				// Data Received
				DEBUG_PRINT("Data received on socket %bd!\r\n", i);
				Ethernet_RecvData(i);
				W5500_WRITE_SOCKET_BYTE(i, Sn_IR, IR_RECV);
				Ethernet_Send_Data(i);
			}

			if((intcode & IR_DISCON) || (intcode & IR_TIMEOUT))
			{
				DEBUG_PRINT("Client disconnected on socket %bd!\r\n", i);
				Ethernet_Socket_Close(i);

				// clear interrupt
				W5500_WRITE_SOCKET_BYTE(i, Sn_IR, IR_DISCON|IR_TIMEOUT);

				Ethernet_Socket_Listen(i);
			}

			if(intcode & IR_CON)
			{
				DEBUG_PRINT("Client connected on socket %bd!\r\n", i);
				
				BUFFER_RESET();
				BUFFER_ADD(i);
				
				W5500_WRITE_SOCKET_BYTE(i, Sn_IR, IR_CON);
				Ethernet_Send_Data(i);
			}
		}

		intsocket >>= 1;
	}
}
