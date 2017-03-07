#ifndef   DEBUG_H
#define   DEBUG_H

#include <stdlib.h>
#include <string.h>
#include "uprintf.h"

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef TDEBUG


#if defined(IO_CRITICAL) 
	#define ENTER_CRITICAL vPortEnterCritical();
	#define EXIT_CRITICAL vPortExitCritical();
#else
	#define ENTER_CRITICAL 
	#define EXIT_CRITICAL 
#endif 

#define vPortPRINTF(format, ...) do { \
	ENTER_CRITICAL \
	my_printf(format, ##__VA_ARGS__);\
	EXIT_CRITICAL \
} while (0)


#ifdef LONG_PATH
#define    portDEBUG_PRINTF(format, ...) do{ \
	ENTER_CRITICAL \
	my_printf("RTOS:%s line:%d \n \t ",__FILE__,__LINE__); \
	my_printf(format, ##__VA_ARGS__);\
	EXIT_CRITICAL \
} while(0)

#else // LONG_PATH

#define    portDEBUG_PRINTF(format, ...) do{ \
	ENTER_CRITICAL \
	my_printf("RTOS:%s line:%d \t",__FILENAME__,__LINE__); \
	my_printf(format, ##__VA_ARGS__);\
	EXIT_CRITICAL \
} while(0)


#endif // LONG_PATH

#else // TDEBUG

#define    portDEBUG_PRINTF(format, ...) do{ \
} while (0)

#define    vPortPRINTF(format, ...) do{ \
} while (0)


#endif // TDEBUG




#endif // _DEBUG_H
