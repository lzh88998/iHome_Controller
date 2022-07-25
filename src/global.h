#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef TEST_ENV
#define MAIN_FOSC			11059200
#else
#define MAIN_FOSC			33177600	//define main clock
#endif

#ifdef SERIAL_OUTPUT
#include "stdio.h"
#define DEBUG_PRINT						printf
#else
#define DEBUG_PRINT						/##/
#endif

#endif