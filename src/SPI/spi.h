#ifndef _SPI_H_
#define _SPI_H_

#include "stc12c5a60s2.h"

enum SPI_CLK_Mode	{ 
										SPI_MODE_IDLE_HIGH_REAR_SAMPLE 	= 0x0C,
										SPI_MODE_IDLE_HIGH_FRONT_SAMPLE = 0x08,
										SPI_MODE_IDLE_LOW_REAR_SAMPLE		= 0x04,
										SPI_MODE_IDLE_LOW_FRONT_SAMPLE	= 0x00
									};

enum SPI_Speed_Mode {
											SPI_SPEED_MODE_LL		= 0x03,
											SPI_SPEED_MODE_L		= 0x02,
											SPI_SPEED_MODE_H		= 0x01,
											SPI_SPEED_MODE_HH		= 0x00
										};

#define SPIF        0x80        //SPSTAT.7
#define WCOL        0x40        //SPSTAT.6

#define SSIG        0x80        //SPCTL.7
#define SPEN        0x40        //SPCTL.6
#define DORD        0x20        //SPCTL.5
#define MSTR        0x10        //SPCTL.4
#define CPOL        0x08        //SPCTL.3
#define CPHA        0x04        //SPCTL.2
#define SPDHH       0x00        //CPU_CLK/4
#define SPDH        0x01        //CPU_CLK/16
#define SPDL        0x02        //CPU_CLK/64
#define SPDLL       0x03        //CPU_CLK/128

#define SPI_INIT()							SPDAT=0;SPSTAT=SPIF|WCOL;SPCTL=SPEN|MSTR|SSIG;
#define SPI_SET_SPEED(speed)		SPCTL=(SPCTL & 0xFC)|(speed & 0x03);
#define SPI_SET_MODE(mode)			SPCTL=(SPCTL & 0xF3)|(mode & 0x0C);

#define SPI_SEND_BYTE(dat)			SPDAT=dat;while(!(SPSTAT&SPIF));SPSTAT=SPIF|WCOL;
#define SPI_RECV_BYTE(dat)			SPDAT=0xFF;while(!(SPSTAT&SPIF));SPSTAT=SPIF|WCOL;dat=SPDAT;
#endif