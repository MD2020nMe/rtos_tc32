
#include "led.h"
#include "gpio.h"


void vToggleLED(int led)
{
	static int led1 = 0;
	static int led2 = 0;
	static int led3 = 0;
	static int led4 = 0;

	extern void vPortEnterCritical();
	extern void vPortExitCritical();

	vPortEnterCritical();

	switch(led) {
		case 1:
			GPIO_SetGPIOEnable(GPIOC_GP0, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP0, Bit_SET);
			led1 ? GPIO_SetBit(GPIOC_GP0) : GPIO_ResetBit(GPIOC_GP0);
			led1 = !led1;
			break;
		case 2:
			GPIO_SetGPIOEnable(GPIOC_GP1, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP1, Bit_SET);
			GPIO_SetBit(GPIOC_GP1);
			led2 ? GPIO_SetBit(GPIOC_GP1) : GPIO_ResetBit(GPIOC_GP1);
			led2 = !led2;
			break;
		case 3:
			GPIO_SetGPIOEnable(GPIOC_GP2, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP2, Bit_SET);
			led3 ? GPIO_SetBit(GPIOC_GP2) : GPIO_ResetBit(GPIOC_GP2);
			led3 = !led3;
			break;
		case 4:
			GPIO_SetGPIOEnable(GPIOC_GP3, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP3, Bit_SET);
			led4 ? GPIO_SetBit(GPIOC_GP3) : GPIO_ResetBit(GPIOC_GP3);
			led4 = !led4;
			break;
	}

	vPortExitCritical();

}

void vTurnLED(int led, int onoff)
{

	switch(led) {
		case 1:
			GPIO_SetGPIOEnable(GPIOC_GP0, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP0, Bit_SET);
			onoff ? GPIO_ResetBit(GPIOC_GP0) : GPIO_SetBit(GPIOC_GP0);
			break;
		case 2:
			GPIO_SetGPIOEnable(GPIOC_GP1, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP1, Bit_SET);
			GPIO_SetBit(GPIOC_GP1);
			onoff ? GPIO_ResetBit(GPIOC_GP1) : GPIO_SetBit(GPIOC_GP1);
			break;
		case 3:
			GPIO_SetGPIOEnable(GPIOC_GP2, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP2, Bit_SET);
			onoff ? GPIO_ResetBit(GPIOC_GP2) : GPIO_SetBit(GPIOC_GP2);
			break;
		case 4:
			GPIO_SetGPIOEnable(GPIOC_GP3, Bit_SET);
			GPIO_SetOutputEnable(GPIOC_GP3, Bit_SET);
			onoff ? GPIO_ResetBit(GPIOC_GP3) : GPIO_SetBit(GPIOC_GP3);
			break;
	}

}


