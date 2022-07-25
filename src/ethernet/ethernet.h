#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include "socket.h"
#include "w5500.h"

void Ethernet_Init();

unsigned char Ethernet_IsReset();

unsigned char Ethernet_IsLinkDown();

void Ethernet_Process_Int();

void Ethernet_Send_Data(SOCKET socket);

#endif