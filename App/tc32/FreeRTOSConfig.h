/*
   FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
   All rights reserved

   1 tab == 4 spoons!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <tlsr8269.h>


/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/



// Tick
#define configUSE_16_BIT_TICKS		0
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( 32000000UL )	/* =32MHz */
#define configTICK_RATE_HZ			( ( TickType_t ) 20 )      // PS run at 50ms Tick for now

// Tasks
#define configUSE_PREEMPTION		1
#define configMAX_PRIORITIES		(5)
#define configMAX_TASK_NAME_LEN		(16)
#define configIDLE_SHOULD_YIELD		1
#define configUSE_NEWLIB_REENTRANT	0

#define configUSE_APPLICATION_TASK_TAG	1
#define configGENERATE_RUN_TIME_STATS	0      // this requires timers to work
#define configUSE_TASK_NOTIFICATIONS	1

#define configUSE_STATS_FORMATTING_FUNCTIONS	0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS	0


// Thread
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0


// Queue
#define configUSE_QUEUE_SETS		1
#define configQUEUE_REGISTRY_SIZE	1

// Memory
#define configUSE_MALLOC_FAILED_HOOK 		1
#define configAPPLICATION_ALLOCATED_HEAP 	1
#define configSUPPORT_DYNAMIC_ALLOCATION 	1
#define configCHECK_FOR_STACK_OVERFLOW		1
#define configMINIMAL_STACK_SIZE			( ( unsigned short ) 256)
#define configTOTAL_HEAP_SIZE				( ( size_t ) ( 18  * 1024 ) ) 

// Mutex
#define configUSE_MUTEXES			1
#define configUSE_RECURSIVE_MUTEXES	1

// Semaphore
#define configUSE_COUNTING_SEMAPHORES	0

// Trace
#define configUSE_TRACE_FACILITY	0

// Assert 

// Error Checking
#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

// Timer
#define configUSE_TIMERS			1
#define configTIMER_TASK_PRIORITY	1
#define configTIMER_QUEUE_LENGTH 	5
#define configTIMER_TASK_STACK_DEPTH ( configMINIMAL_STACK_SIZE * 2 )

/* Set the following definitions to 1 to include the API function, or zero
   to exclude the API function. */
#define INCLUDE_eTaskGetState			1
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_vTaskAbortDelay			1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_xTaskGetHandle          1
#define INCLUDE_xTaskGetSchedulerState  1
#define INCLUDE_xTimerPendFunctionCall 	1
#define INCLUDE_xTaskAbortDelay			1
#define INCLUDE_xTaskResumeFromISR		1
#define INCLUDE_uxTaskGetStackHighWaterMark 	1

#define USE_STDIO						1

#if    (configTRACE_TASK_PRIORITY_SET == 1)
#	define traceTASK_PRIORITY_SET( pxTCB, uxNewPriority ) \
		portDEBUG_PRINTF("traceTASK_PRIORITY_SET pxTCB %04x, newPriority %d\n", pxTCB, uxNewPriority) 
#else
#	define traceTASK_PRIORITY_SET( pxTCB, uxNewPriority ) 
#endif

#if (0) // (TDEBUG == 1)
#	define traceMALLOC(pvReturn, xWantedSize)  vPortTraceMALLOC(pvReturn, xWantedSize)
#	define traceFREE( pv, blockSize ) vPortTraceFree(pv, blocksize)
#else
#	define traceMALLOC(pvReturn, xWantedSize)  
#	define traceFREE( pv, blockSize ) 
#endif

#define traceEVENT_GROUP_CREATE(xEventGroup ) \
		portDEBUG_PRINTF("traceEVENT_GROUP_CREATE Group %04x\n", xEventGroup) 

#define traceEVENT_GROUP_CREATE_FAILED() \
	    portDEBUG_PRINTF("traceEVENT_GROUP_CREATE failed\n")     

#if (TDEBUG == 1)
#	define assert(EX) (void)((EX) || (portDEBUG_PRINTF("Assertion failed %s\n", #EX),0))
#else
#	define configASSERT(EX) ((void)0)
#endif


// Debugger PRINTF
//
#define USE_STDIO						1
#define IO_DEBUG_MODE  					1
#define IO_THRESHOLD  					4
#define IO_BLOCK_MODE  					1
#define IO_CRITICAL    					1



#endif /* FREERTOS_CONFIG_H */
