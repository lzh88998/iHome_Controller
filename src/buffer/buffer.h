#ifndef _BUFFER_H_
#define _BUFFER_H_

#define BUFFER_PARTITION_SIZE		256
#define BUFFER_PARTITION_CNT		1

#define BUFFER_ADD(c)						if(buffer_partition_idx < BUFFER_PARTITION_CNT){\
																	buffer[buffer_partition_idx][buffer_item_idx]=c;\
																	buffer_item_idx++;\
																	if(0==buffer_item_idx){\
																			buffer_partition_idx++;\
																	}\
																}

#define BUFFER_RESET()					buffer_item_idx=0;\
																buffer_partition_idx=0;

extern unsigned char buffer_partition_idx;
extern unsigned char buffer_item_idx;
extern xdata unsigned char buffer[BUFFER_PARTITION_CNT][BUFFER_PARTITION_SIZE];

#endif