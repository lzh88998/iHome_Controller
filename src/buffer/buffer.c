#include "buffer.h"

unsigned char buffer_partition_idx = 0;
unsigned char buffer_item_idx = 0;
xdata unsigned char buffer[BUFFER_PARTITION_CNT][BUFFER_PARTITION_SIZE];
