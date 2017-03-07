#include "bsp.h"
unsigned long	tick_per_us;

// A simple function that makes it handy
//
volatile unsigned char bc;
volatile unsigned int  bc_debug_i,bc_debug_j;

void delay(unsigned int time_10us)
{
	volatile unsigned int i,j;
	
	for(i=0;i<time_10us;i++)
	{
		for(j=0;j<20;j++)
		{
			bc_debug_i = i;
			//bc_debug_j = j;
			//bc++;
		}
	}
	
}

void vPortSetupHardware( void )
{
    struct  S_SYS_CTL  sys_ctl;
    sys_ctl.rst0.all = 0x00;
    sys_ctl.rst1.all = 0x00;
    sys_ctl.rst2.all = 0x00;
    sys_ctl.clk0.all = 0xff;
    sys_ctl.clk1.all = 0xff;
    sys_ctl.clk2.all = 0xff;
    vPortSysInit(&sys_ctl, SYS_CLK_HS_DIV, 6); //set sysclk as 32MHz     

	usb_dp_pullup_en(1); 
}
/*-----------------------------------------------------------*/

//-------------------Clock_init---------------------
void   vPortSysInit(struct  S_SYS_CTL  * p_sys_ctl,enum M_SYSCLK_SEL clock_src,unsigned char clock_divider)
{

	if(p_sys_ctl!=NULL)
	{
		write_reg8(0x800060,p_sys_ctl->rst0.all);
		write_reg8(0x800061,p_sys_ctl->rst1.all);
		write_reg8(0x800062,p_sys_ctl->rst2.all);
		write_reg8(0x800063,p_sys_ctl->clk0.all);
		write_reg8(0x800064,p_sys_ctl->clk1.all);
		write_reg8(0x800065,p_sys_ctl->clk2.all);
	}
	//swires
	write_reg8 (0x800061, read_reg8(0x800061)&(~BIT7));
	write_reg8 (0x800064, read_reg8(0x800064)|BIT7);
	//algm
	write_reg8 (0x800061, read_reg8(0x800061)&(~BIT1));
	write_reg8 (0x800064, read_reg8(0x800064)|BIT1);
	//usb
	write_reg8 (0x800060, read_reg8(0x800060)&(~BIT2));
	write_reg8 (0x800063, read_reg8(0x800063)| BIT2 |BIT3);
	WriteAnalogReg (0x88, 0x0f);
	WriteAnalogReg (0x05, 0x60);
	//system timer
	write_reg8 (0x800061, read_reg8(0x800061)&(~BIT0));
	write_reg8 (0x800064, read_reg8(0x800064)| BIT0);


	write_reg8(0x800073, read_reg8(0x800073)&0x0f);//disable low power mode
	if(SYS_CLK_HS_DIV == clock_src)
	{
		//set FHS select and divider
		write_reg8(0x800070,0x00);
		write_reg8(0x800066,(0x20 | clock_divider));
		SetTickUs(192/clock_divider);
	}
	else if(SYS_CLK_RC == clock_src)
	{
		write_reg8(0x800066,0x00);
		SetTickUs(32);
	}
	write_reg8(0x80074f,0x01);//sys timer beginning to work

	// SWI
    write_reg8(0x800641,read_reg8(0x800641)|0x40); // enable software irq

	// Enable all interrupts
	//write_reg8(0x800643,0x01);  // enable software irq


}


//-------------------Read/Write register---------------------
unsigned char ReadAnalogReg(unsigned char addr){
	write_reg8(0x8000b8,addr);
	write_reg8(0x8000ba,0x40);
	
	while((read_reg8(0x8000ba) & 0x01) != 0x00);
	return read_reg8(0x8000b9);
}
void WriteAnalogReg (unsigned char adr, unsigned char dat)
{

	write_reg8(0x8000b8,adr);
	write_reg8(0x8000b9,dat);
	write_reg8(0x8000ba,0x60);
	while((read_reg8(0x8000ba) & 0x01) != 0x00);
	write_reg8(0x8000ba,0x00);
}

void SetTickUs (unsigned int t)
{
	tick_per_us = t;
}

attribute_ram_code void WaitUs (int microsec)
{
	unsigned int t = read_reg32 (0x800740);
	unsigned int tu = microsec * tick_per_us;

	while(read_reg32(0x800740)-t< tu)
		{}
}

attribute_ram_code void WaitMs (int millisec)
{
    while (millisec) {
        WaitUs(1000);
        millisec--;
    }
}

unsigned long clock_time(void)
{
    unsigned long ret = read_reg32(0x800740);
    return ret;
}

int LoadTblCmdSet (const TBLCMDSET * pt, int size) 
{
	int l=0;
	//int loop;


	while (l<size) {
		unsigned int  cadr = ADR_IO | pt[l].adr;
		unsigned char cdat = pt[l].dat;
		unsigned char ccmd = pt[l].cmd;
		unsigned char cvld =(ccmd & TCMD_UNDER_WR);
		ccmd &= TCMD_MASK;
		if (cvld) {
			if (ccmd == TCMD_WRITE) {
				//if (DEBUG_TBL) printw ("cmdtbl write ", cdat+cadr*256, "\n");
				write_reg8 (cadr, cdat);
			}
			else if (ccmd == TCMD_WAREG) {
				//if (DEBUG_TBL) printw ("cmdtbl analog ", cdat+cadr*256, "\n");
				WriteAnalogReg (cadr, cdat);
			}
			else if (ccmd == TCMD_WAIT) {
				//if (DEBUG_TBL) printw ("cmdtbl wait us  ", cdat+cadr*256, "\n");
				WaitUs (pt[l].adr*256 + cdat);
			}
		}
		l++;
	}
	return size;
}

void usb_dp_pullup_en (int en)
{
	unsigned char dat = ReadAnalogReg(0x08);
	if (en) {
		dat = (dat & 0x3f) | BIT(7);
	}
	else {
		dat = (dat & 0x3f) | BIT(6);
	}

	WriteAnalogReg (0x08, dat);
}



