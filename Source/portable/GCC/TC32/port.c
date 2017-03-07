/*
   FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
   All rights reserved

   VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

   This file is part of the FreeRTOS distribution.

   FreeRTOS is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License (version 2) as published by the
   Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

 ***************************************************************************
 >>!   NOTE: The modification to the GPL is included to allow you to     !<<
 >>!   distribute a combined work that includes FreeRTOS without being   !<<
 >>!   obliged to provide the source code for proprietary components     !<<
 >>!   outside of the FreeRTOS kernel.                                   !<<
 ***************************************************************************

 1 tab == 4 spoons!
 */


/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "led.h"

/* Constants required to setup the task context. */
#define portINSTRUCTION_SIZE			(( StackType_t ) 2)
#define portNO_CRITICAL_SECTION_NESTING	(( StackType_t ) 0)
#define portINITIAL_XPSR                (0x01000000) 


uint32_t * pxStackPointer = 0;

/*-----------------------------------------------------------*/


extern void vPortISRStartFirstTask(void);
static void prvTaskExitError(void);
static void vPortStartFirstTask(void);

#ifdef confTRACE_MALLOC 
void vPortTraceMALLOC(void * pvReturn, size_t  xWantedSize)
{
	vPortPRINTF("traceMALLOC pvReturn=%04x xWantedSize=%d\n", pvReturn, xWantedSize);
} 
void vPortTraceFREE(void * pvReturn, size_t  xWantedSize)
{
	vPortPRINTF("traceFREE pvReturn=%04x xWantedSize=%d\n", pvReturn, xWantedSize);
} 
#endif // TRACE


/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description. 
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, 
		TaskFunction_t pxCode, void *pvParameters )
{

	// XD  : we need only save little contex
	//pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
	//*pxTopOfStack = portINITIAL_XPSR;   /* xPSR */
	//pxTopOfStack--;
	*pxTopOfStack-- = ( StackType_t ) pxCode; /* PC    /LR/  */ 
	*pxTopOfStack-- = 0;  //r7
	*pxTopOfStack-- = 0;  //r6
	*pxTopOfStack-- = 0;  //r5
	*pxTopOfStack-- = 0;  //r4
	*pxTopOfStack-- = 0;  //r3
	*pxTopOfStack-- = 0;  //r2
	*pxTopOfStack-- = 0;  //r1
	*pxTopOfStack-- = ( StackType_t ) pvParameters; // R0

	*pxTopOfStack-- = 0;  //r8
	*pxTopOfStack-- = 0;  //r9
	*pxTopOfStack-- = 0;  //r10
	*pxTopOfStack-- = 0;  //r11
	*pxTopOfStack-- = 0;  //r12

	*pxTopOfStack-- = 0x13; //CPSR

	*pxTopOfStack = 0; // ulCriticalNesting 




	return pxTopOfStack;
}

/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	//configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}



extern  struct tskTaskControlBlock  * pxCurrentTCB;

void vPortStartFirstTask( void )
{

	portENABLE_INTERRUPTS();
	portDEBUG_PRINTF("Starting first Task = %4x\n",pxCurrentTCB);
	portRESTORE_CONTEXT();
}

/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	   here already. */
	vPortSetupTimerInterrupt();

	/* Start the first task. */
	vPortStartFirstTask();	

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the TC32 port will require this function as there
	   is nothing to return to.  */
}
/*-----------------------------------------------------------*/

/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */

/*-----------------------------------------------------------*/
void vPortSetupTimerInterrupt(void)
{
	/*Set initial Tick value of Timer0*/
	write_reg32(0x800630, 0);

	/*Set Capture value of Timer0(SysClk: 32MHz SysTick: 100Hz Cycle: 50ms)*/
	write_reg32(0x800624, (1000 / configTICK_RATE_HZ) *32*1024); // 50ms

	/*Set Timer0 to Mode 0 and enable Timer0*/
	write_reg8(0x800620, 0x01);

	/*enable Timer0's irq*/
	//write_reg8(0x800640,read_reg8(0x800640)|0x01); // enable software irq
	portENABLE_TIMER0();
}


// Read SP 
//			"tjex lr;"
void vPortGetSPValue()
{
	__asm volatile (
			"tpush     {r0};" 
			"tpush     {r1};" 
			"tloadr r0,=pxStackPointer;"
			"tmov  r1,r13;"
			"tstorer r1, [r0];"
			"tpop  {r1};"
			"tpop  {r0};"
			);

	portDEBUG_PRINTF("Stack Pointer %04x\n");

}


