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

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	unsigned long
#define portBASE_TYPE	portLONG

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			8
#define portNOP()					__asm volatile ( "tnop" );
/*-----------------------------------------------------------*/

#define OS_CPU_CM3_NVIC_PRIO_MIN   	0xFF     /* Min handler prio.          */
#define portSWI_IS_ENABLED() 		(read_reg8(0x800649) & 0x40)
#define portSWI_ENABLE() 			write_reg8(0x800641,read_reg8(0x800641)|0x40)
#define portSWI_SET()   			write_reg8(0x800022,0x10)
#define portSWI_CLR()   			write_reg8(0x800022,0x01)



/* Scheduler utilities. */

/*
 */

#define portSAVE_CONTEXT()                do {                  	        \
    __asm volatile ("tpush  {r14}");         					 			\
	__asm volatile ("tpush  {r0-r7}");     									\
	__asm volatile ("tmrcs   r0");     									    \
	__asm volatile ("tmov	   r1, r8");     								\
	__asm volatile ("tmov	   r2, r9");     								\
	__asm volatile ("tmov	   r3, r10");     								\
	__asm volatile ("tmov	   r4, r11");     								\
	__asm volatile ("tmov	   r5, r12");     								\
	__asm volatile ("tpush  {r0-r5}");     									\
	__asm volatile ("tloadr r2, =ulCriticalNesting");						\
	__asm volatile ("tloadr r1, [r2]");										\
	__asm volatile ("tpush {r1}");											\
	__asm volatile ("tloadr r0, =pxCurrentTCB");							\
	__asm volatile ("tloadr r0, [r0]");                            	       \
	__asm volatile ("tmov     r1, r13 ");									\
	__asm volatile ("tstorer r1, [r0] ");								    \
}while(0)


#define portRESTORE_CONTEXT()     do                {   	             \
    __asm volatile ("tloadr   r1, =pxCurrentTCB");   						\
	__asm volatile ("tloadr   r1, [r1] ");									\
	__asm volatile ("tloadr   r0, [r1] ");									\
	__asm volatile ("tmov     r13, r0 ");									\
	__asm volatile ("tloadr   r0, =ulCriticalNesting");						\
	__asm volatile ("tpop     {r1} ");										\
	__asm volatile ("tstorer   r1, [r0] ");									\
	__asm volatile ("tpop		{r0-r5}");        					        \
	__asm volatile ("tmov	   r8, r1");     								\
	__asm volatile ("tmov	   r9, r2");     								\
	__asm volatile ("tmov	   r10, r3");     								\
	__asm volatile ("tmov	   r11, r4");     								\
	__asm volatile ("tmov	   r12, r5");     								\
	__asm volatile ("tmcsr     r0");        					            \
	__asm volatile ("tpop		{r0-r7}");        					        \
	__asm volatile ("tpop		{r15}");        							\
} while(0)


#define portYIELD_FROM_ISR()		vTaskSwitchContext()
#define portYIELD()					vPortYieldProcessor()

/* Critical section management. */
#define portDISABLE_INTERRUPTS() write_reg8(0x800643,0x00)
#define portENABLE_INTERRUPTS()	 write_reg8(0x800643,0x01)

#define portENABLE_TIMER0()	 	 write_reg8(0x800640,read_reg8(0x800640)|0x01) 
#define portENABLE_TIMER1()	 	 write_reg8(0x800640,read_reg8(0x800640)|0x02) 
#define portENABLE_TIMER2()	 	 write_reg8(0x800640,read_reg8(0x800640)|0x04) 
#define portENABLE_TIMER3()	 	 write_reg8(0x800640,read_reg8(0x800640)|0x08) 

#define portCLEAR_TIMER0() 		 write_reg8(0x800623, 0x01)
#define portCLEAR_TIMER1() 		 write_reg8(0x800623, 0x02)
#define portCLEAR_TIMER2() 		 write_reg8(0x800623, 0x04)
#define portCLEAR_TIMER3() 		 write_reg8(0x800623, 0x08)
#define portCLEAR_DOG() 		 write_reg8(0x800623, 0x08)
#define portIRQ_SRC() 			read_reg16(0x800648)
#define portIRQ_IS_TICK()       (read_reg16(0x800648) & 0x0001)

#define portENTER_CRITICAL()     vPortEnterCritical()                                                             
#define portEXIT_CRITICAL()      vPortExitCritical()

void vPortSetupTimerInterrupt(void);
void vPortIRQHandler(void);
void vPortEnterCritical( void );
void vPortExitCritical( void );
void vPortYieldProcessor(void)  __attribute__((naked)); ;
void vPortTraceMALLOC(void * pvReturn, size_t  xWantedSize);
void vPortGetSPValue();
			

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )




#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

