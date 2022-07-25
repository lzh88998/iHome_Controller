#ifndef _W5500_H_
#define _W5500_H_

#include "socket.h"
#include "..\spi\spi.h"
//#include "..\timer\timer.h"

sbit W5500_INT = P3^2;
#ifdef TEST_ENV
sbit W5500_SCS = P1^3; // P3^1 on production P1^3 is only used for DIP40 test
#else
sbit W5500_SCS = P3^1; // P3^1 on production P1^3 is only used for DIP40 test
#endif

#ifdef TEST_ENV
sbit W5500_RST = P1^4; // P4^3 on production P1^4 is only used for DIP40 test
#else
sbit W5500_RST = P4^3; // P4^3 on production P1^4 is only used for DIP40 test
#endif

#define W5500_READ	0
#define W5500_WRITE	1

#define W5500_HARD_RESET()																W5500_RST=0;\
																													for(i = 0; i != 255; i++){for(temp=0; temp !=255; temp++);}\
																													W5500_RST=1;
							
#define W5500_LOCATE_REGISTER(addr, reg_type, rw)					W5500_SCS=0;\
																													SPI_SEND_BYTE(addr>>8);\
																													SPI_SEND_BYTE(addr);\
																													SPI_SEND_BYTE((reg_type)|(rw?RWB_WRITE:RWB_READ)|VDM);
							
#define W5500_WRITE_DATA(dat)															SPI_SEND_BYTE(dat);
							
#define W5500_READ_DATA(dat)															SPI_RECV_BYTE(dat);

#define W5500_FINISH_READ_WRITE()													W5500_SCS=1;

#define W5500_LOCATE_SOCKET_REGISTER(s, addr, is_write)		W5500_LOCATE_REGISTER(addr, ((s << 5)+0x08), is_write);

#define W5500_WRITE_COMMON_REG_BYTE(reg, dat)							W5500_LOCATE_REGISTER(reg, COMMON_R, W5500_WRITE);\
																													W5500_WRITE_DATA(dat);\
																													W5500_FINISH_READ_WRITE();
							
#define W5500_READ_COMMON_REG_BYTE(reg, dat)							W5500_LOCATE_REGISTER(reg, COMMON_R, W5500_READ);\
																													W5500_READ_DATA(dat);\
																													W5500_FINISH_READ_WRITE();
							
#define W5500_WRITE_COMMON_REG_SHORT(reg, high, low)			W5500_LOCATE_REGISTER(reg, COMMON_R, W5500_WRITE);\
																													W5500_WRITE_DATA(high);\
																													W5500_WRITE_DATA(low);\
																													W5500_FINISH_READ_WRITE();

#define W5500_WRITE_SOCKET_BYTE(s, reg, dat)							W5500_LOCATE_SOCKET_REGISTER(s, reg, W5500_WRITE);\
																													W5500_WRITE_DATA(dat);\
																													W5500_FINISH_READ_WRITE();

#define W5500_WRITE_SOCKET_SHORT(s, reg, offset_h, offset_l)	W5500_LOCATE_SOCKET_REGISTER(s, reg, W5500_WRITE);\
																															W5500_WRITE_DATA(offset_h);\
																															W5500_WRITE_DATA(offset_l);\
																															W5500_FINISH_READ_WRITE();

#define W5500_READ_SOCKET_BYTE(s, reg, dat)								W5500_LOCATE_SOCKET_REGISTER(s, reg, W5500_READ);\
																													W5500_READ_DATA(dat);\
																													W5500_FINISH_READ_WRITE();

#define W5500_READ_SOCKET_SHORT(s, reg, dat_high, dat_low)	W5500_LOCATE_SOCKET_REGISTER(s, reg, W5500_READ);\
																														W5500_READ_DATA(dat_high);\
																														W5500_READ_DATA(dat_low);\
																														W5500_FINISH_READ_WRITE();

#define W5500_LOCATE_DATA(offset_h, offset_l, reg_type, rw)	W5500_SCS=0;\
																														SPI_SEND_BYTE(offset_h);\
																														SPI_SEND_BYTE(offset_l);\
																														SPI_SEND_BYTE((reg_type)|(rw?RWB_WRITE:RWB_READ)|VDM);
							


#endif