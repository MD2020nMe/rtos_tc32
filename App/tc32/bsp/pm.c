#include "pm.h"

//restore analog Regs' value as default
static const TBLCMDSET  tbl_cpu_wakeup_init[] = {
	{0x01, 0x4e, 0},
	{0x01, 0x4e, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x2c, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x81, 0xc0, TCMD_UNDER_BOTH | TCMD_WAREG},	//xtal cap off to quick settle osc
	{0x20, 0xdb, TCMD_UNDER_BOTH | TCMD_WAREG},  //wakeup reset time
	{0x2d, 0x0f, TCMD_UNDER_BOTH | TCMD_WAREG},	//quick settle: 200 us
	{0x81, 0xc0, TCMD_UNDER_BOTH | TCMD_WAREG},	//xtal cap off to quick settle osc
	{0x2f, 0xcc, TCMD_UNDER_BOTH | TCMD_WAREG},	//cal wait len
	{0x2f, 0xcd, TCMD_UNDER_BOTH | TCMD_WAREG},	//cal wait len
	{0x05, 0x62, TCMD_UNDER_BOTH | TCMD_WAREG},	//turn off crystal pad,	bit7: bbpll	(8267)
	{0x88, 0x0f, TCMD_UNDER_BOTH | TCMD_WAREG},	//bit[1:0]: 192M CLOCK to digital (8267)
	{0x21, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x22, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x23, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x24, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x25, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x27, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},   
	{0x28, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x29, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x2a, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
	{0x2b, 0x00, TCMD_UNDER_BOTH | TCMD_WAREG},
};

static void sleep_start(void) 
{
      write_reg8(0x80006f,0x81);
      volatile unsigned int i;
      for(i=0; i<0x30; i++);
}

static void suspend_start(void)
{
	volatile unsigned int i;
	//REG_ADDR8(0x0d) = 0;
	//REG_ADDR8(0x0c) = 0xb9;
	// for(i=0; i<2; i++); //1440ns when 32M clk
	//REG_ADDR8(0x0d) = 1;
	write_reg8(0x80006f,0x81);
	for(i=0; i<0x30; i++);
	//REG_ADDR8 (0x0d) = 0;
	//REG_ADDR8 (0x0c) = 0xab;
	// for(i=0; i<2; i++); //1440ns when 32M clk
	//REG_ADDR8 (0x0d) = 1;
}

void PM_GPIOSet(Pin_TypeDef pin, int pol, int en)    //only surport from suspend
{
    if ((pin&0xfff) == 0) {
    	return;
    }
    
    unsigned char bit = pin & 0xff;
    unsigned long reg_en = 0x800000 + 0x587 + ((pin>>8)<<3);
    unsigned long reg_pol = 0x800000 + 0x584 + ((pin>>8)<<3);

    if (en) {                                         //set gpio pin wakeup enable
    	write_reg8(reg_en,read_reg8(reg_en)|bit);   
    }
    else {
    	write_reg8(reg_en,read_reg8(reg_en)&(~bit));
    }

    if (pol) {                                        //set gpio pin wakeup polarity     
		write_reg8(reg_pol,read_reg8(reg_pol)&(~bit));
	}
	else {
		write_reg8(reg_pol,read_reg8(reg_pol)|bit);
	} 

    write_reg8(0x8005b5,read_reg8(0x8005b5)|0x04);   //0x5b5[2] gpio wakeup enable 
}

void PM_WakeupInit(void)
{	
	LoadTblCmdSet(tbl_cpu_wakeup_init, sizeof (tbl_cpu_wakeup_init)/sizeof (TBLCMDSET));
    // WriteAnalogReg(0x01,ReadAnalogReg(0x01)&0xfb); //clr afe3v01<2>
    // WriteAnalogReg(0x01,ReadAnalogReg(0x01)|0x08); //set afe3v01<3>
    // WriteAnalogReg(0x00,ReadAnalogReg(0x00)|0x80); //set afe3v00<7>
	write_reg16(0x800750, 0x0001);	//set 32KTimer cal Result reg a special value
	write_reg8(0x80074f, 0x01);	//enable system timer and 32KTimer's cal
	while (read_reg16(0x800750) == 0x0001); //wait for 32KTimer's cal completed
}

/*************************************************************
** wakeup en: 	0x27 - 0x2b(pa - pe)
** pol:			0x21 - 0x25(pa - pe)
** 0x2b[3:0]: pe_wakeup_en
** 0x25[3:0]: pe_pol
** 0x2b[7:4]: reserved
** 0x25[7:4]: reserved
************************************************************/
void PM_PadSet(Pin_TypeDef pin, int pol, int en)
{
    unsigned char mask = pin & 0xff;
	unsigned char areg;
	unsigned char val;

	if(!mask)
		return;

	/***set pad pin's polarity***/
	areg = ((pin>>8) + 0x21);
	val = ReadAnalogReg(areg);
	if (pol) {
		val &= ~mask;
	}
	else {
		val |= mask;
	}
	WriteAnalogReg (areg, val);	

	/***store enabled pad pin***/
	areg = ((pin>>8) + 0x27);
	val = ReadAnalogReg(areg);
	if (en) {
		val |= mask;
	}
	else {
		val &= ~mask;
	}
	WriteAnalogReg (areg, val);	
}

int  PM_LowPwrEnter(int DeepSleep, int WakeupSrc, unsigned long WakeupTick)
{
    int system_tick_enable = WakeupSrc & WAKEUP_SRC_TIMER;
    unsigned long  span = WakeupTick - read_reg32(0x800740);
    unsigned char i = 0;
    unsigned char qdec_wakeup_en = 0;

	if ((system_tick_enable) && (span<0)) {
		return  ReadAnalogReg(0x44) & 0x0f;
	}

    /***if sleep time is less than 2ms, then delay instead of enter sleep***/
    if ((system_tick_enable) && (span<(2000*tick_per_us))) {
		unsigned long t = read_reg32(0x800740);
		WriteAnalogReg(0x44, 0x0f);			//clear all status
		unsigned char st;
		do {
			st = ReadAnalogReg (0x44) & 0x0f;
		} while ((read_reg32(0x800740) - t < span) && !st);
		return st;
	}

    WriteAnalogReg(0x2c, 0x00);

	/******set wakeup source ******/
	WriteAnalogReg (0x26, (WakeupSrc&0xff));

	unsigned char anareg01 = ReadAnalogReg(0x01);		
	unsigned char anareg2c = ReadAnalogReg(0x2c);		//power down control
	unsigned char anareg2d = ReadAnalogReg(0x2d);		//power down control
	unsigned char anareg80 = ReadAnalogReg(0x80);		//xtal cap
	unsigned char anareg81 = ReadAnalogReg(0x81);		//xtal cap

    write_reg8(0x80006e,0x00);
    if (WakeupSrc >> 8) {                               //usb/digital_gpio/qdec wakeup
        write_reg8(0x80006e,(WakeupSrc >> 8));
        if ((WakeupSrc>>8) & 0x10) {                    //if enable qdec wankeup, set 0xd4[0] to 1
            write_reg8(0x8000d4, read_reg8(0x8000d4)|0x01);
            qdec_wakeup_en = 1;
        }
    }
	WriteAnalogReg (0x44, 0x0f);				//clear all flag
	
	/****** set wakeup tick ******/
	if (system_tick_enable) {
		anareg2c &= 0xfe;						//32K RC need be enabled
		
        //write setting tick value 
        write_reg32(0x800748, WakeupTick-2000*tick_per_us);

        //prepare suspend/deepsleep
        write_reg8(0x80074f,0x02);
		while(0x2 == (read_reg8(0x80074f) & 0x2) ){
			for(i=0;i <16;i++);
		}
	} 
	else {
		write_reg8(0x80074c,0x20);
		anareg2c |= BIT(0);
	}

	if (qdec_wakeup_en) {                       //32K RC need be enabled
		anareg2c &= 0xfe;
	}

    /****** change to 32M RC clock before suspend ******/
	unsigned char reg66 = read_reg8(0x800066);			
	write_reg8(0x800066, 0);				//change to 32M rc clock

	/******set DeepSleep flag******/
	if (DeepSleep) {
		WriteAnalogReg(0x3f, ReadAnalogReg(0x3f) | 0x01);
	} 
	else {
	    WriteAnalogReg(0x3f, ReadAnalogReg(0x3f) & 0xfe);
    }

	WriteAnalogReg(0x2c, (DeepSleep ? 0xfe : 0x5e) | anareg2c);
	WriteAnalogReg(0x01, anareg01 | BIT(3));		//floating R to reduce DVDD leakage
	WriteAnalogReg (0x81, 0xc0);					//turn off xtal cap
	// 0x80: bit7-aac off; bit4-pd_dig; bit3-pd_rfpll
	WriteAnalogReg (0x80, 0xa1);					//turn off xtal 4.6p cap

	WriteAnalogReg(0x2d, anareg2d & 0xdf);  // write 3.3v register 0x2d【5】 to 0 before enter suspend 
	if (DeepSleep) {
		sleep_start();
	}
	else {
		suspend_start();    
	}
	
    /*suspend recover setting*/
	WriteAnalogReg(0x01, anareg01);
	WriteAnalogReg(0x2c, anareg2c);
	//WriteAnalogReg(0x2d, anareg2d | 0x20);  // write 3.3v register 0x2d【5】 to 1 after exit from suspend 

	WriteAnalogReg (0x80, anareg80);		//restore xtal cap setting
	WriteAnalogReg (0x81, anareg81);		//restore xtal cap setting

    /***clear "qdec_irq" irq source flag***/
	if (read_reg8(0x8000d5) & 0x01) {                     
        write_reg8(0x8000d5, read_reg8(0x8000d5)|0x01);      
        while(read_reg8(0x8000d5) & 0x01);
    }

	/****** clear GPIO pad wake-up configuration******/
	// WriteAnalogReg(0x26, ReadAnalogReg(0x26)&0xef); //clear 0x26[4] to disable wakeup from pad

	/******system clock has to be on******/
	write_reg8 (0x800066, reg66);			//restore system clock

	if (system_tick_enable) {
		write_reg8(0x80074f, 0x01);
	 	while (!(read_reg8(0x80074f) & 0x02));
	}

	write_reg8(0x80074c, 0x90);				//auto mode
	write_reg8(0x80074f, 0x01);				//enable system timer

	unsigned long now,delta;
	if (ReadAnalogReg(0x44) & BIT(1)) {		//wakeup from timer
		do {
            now = read_reg32(0x800740);
            if (now > WakeupTick) {
            	delta = now - WakeupTick;
            }
            else {
            	delta = WakeupTick - now;
            }
		} while (delta > 150);
		// while ((read_reg32(0x800740) - WakeupTick) > BIT(30));
	}

	return ReadAnalogReg(0x44);
}


	

