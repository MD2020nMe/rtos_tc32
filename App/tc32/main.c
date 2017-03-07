/*
r   FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

	bra bra bra

*/


/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Demo application includes. */
#include "flash.h"
#include "PollQ.h"
#include "dynamic.h"
#include "BlockQ.h"
#include "blocktim.h"
#include "semtest.h"
#include "EventGroupsDemo.h"

// TC32 related setup
#include "debug.h"
#include "uprintf.h"
#include "bsp.h"
#include "led.h"

/* Priorities for the demo application tasks. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainMEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )

/* The rate at which the on board LED will toggle when there is/is not an
error. */
#define mainNO_ERROR_FLASH_PERIOD	( ( TickType_t ) 3000 / portTICK_PERIOD_MS  )
#define mainERROR_FLASH_PERIOD		( ( TickType_t ) 500 / portTICK_PERIOD_MS  )
#define mainON_BOARD_LED_BIT		( ( unsigned long ) 0x80 )

/* Constants used by the vMemCheckTask() task. */
#define mainCOUNT_INITIAL_VALUE		( ( unsigned long ) 0 )
#define mainNO_TASK					( 0 )

/* The size of the memory blocks allocated by the vMemCheckTask() task. */
#define mainMEM_CHECK_SIZE_1		( ( size_t ) 51 )
#define mainMEM_CHECK_SIZE_2		( ( size_t ) 52 )
#define mainMEM_CHECK_SIZE_3		( ( size_t ) 151 )

// Some place holder for the firmware version etc.
unsigned long firmwareVersion = 0x00100200;

// This is the Heap.  Can do it at the link file level also
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

extern  struct tskTaskControlBlock  * pxCurrentTCB;

extern void vCreateAbortDelayTasks(void);

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */

const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
static long prvCheckOtherTasksAreStillRunning();
static uint32_t ulParameterCheck = 0;

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning().  See the description at the top
 * of the file.
 */
// task handle
static portTASK_FUNCTION_PROTO( vBlinkTask, pvParameters );
static portTASK_FUNCTION_PROTO( vErrorCheckTask, pvParameters );
static portTASK_FUNCTION_PROTO( vMemCheckTask, pvParameters );
static unsigned long ulMemCheckTaskCount = 9;

// LED status
#define LED_ERRORCHECK  	1
#define LED_BLINKTEST    	2
#define LED_MEMTEST      	3
#define LED_HEARTBEAT 		4

// Hooks
void vApplicationMallocFailedHook( void );

void vApplicationMallocFailedHook( void )
{
	vPortPRINTF("Memory Allocation failed");
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	vPortPRINTF("Stack overflow Task %04x [%s]\n", (void *)xTask, pcTaskName);
}

void vPortUserIRQHandler(void );
void vPortUserIRQHandler(void ) 
{
#ifdef USER_DEFINED_HANDLER	
	static volatile int tick_cnt = 0;

  	portDEBUG_PRINTF("User defined IRQ\n");

   
   if (portIRQ_IS_TICK()) {
		portDEBUG_PRINTF("Tick:%4x\n",tick_cnt++);
		portCLEAR_TIMER0();
   	}
#endif // USER
   
}


// Quick and dirty test tasks
//
static portTASK_FUNCTION( vBlinkTask, pvParameters )
{
    uint32_t  task2_cnt = 0;
	TickType_t xDelayPeriod = 10; 

	while(1) {
			
		if (task2_cnt > 5) {
			vPortPRINTF("BlinkTask is alive\n");
			vToggleLED(LED_BLINKTEST);
			task2_cnt = 0;
			vTaskDelay( xDelayPeriod);
		}
	}

	(void) pvParameters;


}


static portTASK_FUNCTION( vMemCheckTask, pvParameters )
{

	unsigned long *pulMemCheckTaskRunningCounter = NULL;
	void *pvMem1, *pvMem2, *pvMem3;
	static long lErrorOccurred = pdFALSE;

	TickType_t xDelayPeriod = 10;

	if (pvParameters != NULL) {
		pulMemCheckTaskRunningCounter = ( unsigned long * ) pvParameters;
	}

	while(1) {
			
		// still have bugs here!

		if( lErrorOccurred == pdFALSE )
		{
			portENTER_CRITICAL();
			/* We have never seen an error so increment the counter. */
			if (pulMemCheckTaskRunningCounter != NULL) {
				( *pulMemCheckTaskRunningCounter )++;
				vPortPRINTF("MemCheck is alive (%04x)\n",*pulMemCheckTaskRunningCounter);
			}
			portEXIT_CRITICAL();
		}

		vTaskSuspendAll();
		{
			portDEBUG_PRINTF("Allocating SIZE 1\n");
			pvMem1 = pvPortMalloc( mainMEM_CHECK_SIZE_1 );
			if( pvMem1 == NULL )
			{
				lErrorOccurred = pdTRUE;
			}
			else
			{
    			portDEBUG_PRINTF("Free SIZE 1\n");
				memset( pvMem1, 0xaa, mainMEM_CHECK_SIZE_1 );
				vPortFree( pvMem1 );
			}
		}
		xTaskResumeAll();

		/* Again - with a different size block. */
		vTaskSuspendAll();
		{
			portDEBUG_PRINTF("Allocating SIZE 2\n");
			pvMem2 = pvPortMalloc( mainMEM_CHECK_SIZE_2 );
			if( pvMem2 == NULL )
			{
				lErrorOccurred = pdTRUE;
			}
			else
			{
				portDEBUG_PRINTF("Free SIZE 2\n");
				memset( pvMem2, 0xaa, mainMEM_CHECK_SIZE_2 );
				vPortFree( pvMem2 );
			}
		}
		xTaskResumeAll();

		/* Again - with a different size block. */
		vTaskSuspendAll();
		{
			portDEBUG_PRINTF("Allocating SIZE 3\n");
			pvMem3 = pvPortMalloc( mainMEM_CHECK_SIZE_3 );
			if( pvMem3 == NULL )
			{
				lErrorOccurred = pdTRUE;
			}
			else
			{
				portDEBUG_PRINTF("Free SIZE 3\n");
				memset( pvMem3, 0xaa, mainMEM_CHECK_SIZE_3 );
				vPortFree( pvMem3 );
			}
		}
		xTaskResumeAll();


		vToggleLED(LED_MEMTEST);
		vTaskDelay( xDelayPeriod );

	}

}

static portTASK_FUNCTION( vErrorCheckTask, pvParameters )
{
	TickType_t xDelayPeriod = mainNO_ERROR_FLASH_PERIOD;
	uint32_t task1_cnt = 0;

	/* The parameters are not used in this function. */
	( void ) pvParameters;

	
	while(1)  
	{
		if (task1_cnt++ > 10) {
			task1_cnt = 0;
			vPortPRINTF("ErrorCheck is alive\n");
		}

		/* Check all the standard demo application tasks are executing without
		error.  ulMemCheckTaskRunningCount is checked to ensure it was
		modified by the task just deleted. */
		if( prvCheckOtherTasksAreStillRunning() != pdPASS ) {

			/* An error has been detected in one of the tasks - flash faster. */
			xDelayPeriod = mainERROR_FLASH_PERIOD;
		} else
			xDelayPeriod = mainNO_ERROR_FLASH_PERIOD;

		vTaskDelay( xDelayPeriod );
		vToggleLED(LED_ERRORCHECK);


	}

}


static long prvCheckOtherTasksAreStillRunning()
{
	long lReturn = ( long ) pdPASS;

	if( xArePollingQueuesStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

#if 0
	if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}
#endif


	if( xAreBlockingQueuesStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( ulMemCheckTaskCount == mainCOUNT_INITIAL_VALUE )
	{
		/* The vMemCheckTask did not increment the counter - it must
		have failed. */
		lReturn = ( long ) pdFAIL;
	}

	return lReturn;
}



int main( void )
{


	/* Setup the hardware for use with the Telink EVK  board. */
	/* Currently 8267 and 8269 are assumed for theset test cases */
	vPortSetupHardware();
	delay(50*1000); //wait usb setup.

	vPortPRINTF("****RTOS INITIALING*****\n");
	vPortPRINTF("size of char %d\n", sizeof(char));
	vPortPRINTF("size of short %d\n", sizeof(short));
	vPortPRINTF("size of int %d\n", sizeof(int));
	vPortPRINTF("size of long %d\n", sizeof(long));
	vPortPRINTF("size of long long %d\n", sizeof(long long));
	vPortPRINTF("size of double %d\n", sizeof(double));
	vPortGetSPValue();

	vStartLEDFlashTasks(mainLED_TASK_PRIORITY);  	// verified, this is served as visual heart beat
	vStartRecursiveMutexTasks();					// verified with stack size 256 

//
// yes they are verified but you need to adjust the heap and stack size to make sure they 
// fits into the SRAM
//
#ifdef VERIFIED
	vStartTimerDemoTask(10); 						// verified 1227
	vStartQueuePeekTasks();  						// verified 1227, LED 1
	vStartEventGroupTasks();  						// verified	
	vCreateSuicidalTasks(mainCHECK_TASK_PRIORITY);  // verified
	vStartQueueSetTasks();     						// verified
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY ); // verified
	vStartQueueSetPollingTask(); 					// Verified
	vCreateBlockTimeTasks();   						// verified under 69
	xTaskCreate( vErrorCheckTask, "ErrorCheck", configMINIMAL_STACK_SIZE, NULL,  mainCHECK_TASK_PRIORITY, NULL);     // verified
	xTaskCreate( vMemCheckTask, "MemCheck", configMINIMAL_STACK_SIZE, NULL, mainMEM_TEST_PRIORITY, NULL);  // verified
	xTaskCreate( vBlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, mainLED_TASK_PRIORITY, NULL);  // verified
	vStartDynamicPriorityTasks();  					// verified needs 23k heap and 128 byte size stack.  Just heap thirsty
	vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );  // verified.  stack size 128 bytes 
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY ); // verified with stack size 128
	vCreateAbortDelayTasks();   					// verified,  128 bytes stack
#endif

	// To be verified 
#ifdef NOT_VERIFIED
	vStartTaskNotifyTask();
#endif 

	// Now all the tasks have been started - start the scheduler.
	vPortPRINTF("Starting Scheduler\n");
	vTaskStartScheduler();
	
    while (1) {
	
	}

}
