#include "FreeRTOSConfig.h"
#include <stdlib.h>
#include <string.h>
#include "bsp.h"

#if defined(IO_DEBUG_MODE)

#define   USBFIFO    0x80013d

#if  defined(IO_BLOCK_MODE)
	#define BLOCK_MODE IO_BLOCK_MODE
#else 
	#define   BLOCK_MODE   1
#endif

#if  defined(IO_THREAHOLD)
	#define FIFOTHRESHOLD IO_THREAHOLD
#else 
	#define FIFOTHRESHOLD 4
#endif


void uart_putc (char c)
{
#if(BLOCK_MODE)
	while (read_reg8(USBFIFO) & BIT1);
#endif
	write_reg8(EDPS_DAT, c);
}

void printb(unsigned char c) {
	unsigned char nib = c >> 4;
	if (nib > 9)	nib = nib + 87;
	else		nib = nib + 48;
	uart_putc (nib);

	nib = c & 15;
	if (nib > 9)	nib = nib + 87;
	else		nib = nib + 48;
	uart_putc (nib);
}

#define vaStart(list, param) list = (char*)((int)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]

void sysPutNumber(unsigned int w,int len) {
	int i;
	int c = w;
	//int len_adj = (len>4) || (len<1) ? 4 : len;
	              
	for(i=len-1;i>=0;i--)
	{
		c = w >>(i*8);
		printb(c);
	}
}

void sysPutInt(unsigned int w)
{
	unsigned char buf[12],tmp,*p;
	int u;
	//buffer[11] = 0;
	p = buf + 11;
	*p = '\0';
	u = w;
	do{ // at least one time..
        tmp = u % 10;
		*--p = tmp + '0';
		u /= 10;
	}while(u);
	while(*p){
        uart_putc(*p);
        p++;
    }
	
}

void  sysPutString(char * str)
{
    char *s;
	s = str;
	if(s == NULL){
        s = "(null)";
    }
	while(*s){
           uart_putc(*s);
           s++;
	}
}


static char *FormatItem(char *f, int a)
{		char c;
		int    fieldwidth = 0;
		int    flag = 0;
		
		while ((c = *f++) != 0)
		{
			if (c >= '0' && c <= '9')
			{
				fieldwidth = (fieldwidth * 10) + (c - '0');
			}
			else
			{
				switch (c)
				{
					case 'x': 
						flag = 16;
						break;
					case 'd':
						flag = 10;
						break;
					case 's':
						flag = 99;
						break;
					default:
						uart_putc('*');
						flag = -1;
						break;
				}
			}
			if(flag!=0)
			{
				if(fieldwidth==0)  fieldwidth=8;
				break;
			}
		}
		if(flag == 16)
			sysPutNumber(a,fieldwidth);
		else if(flag == 10)
			sysPutInt(a);
		else if(flag==99)
			sysPutString((char *)a);
		return f;
		
}

void my_printf(const char *format, ...) {
	//va_list args;
	char *  args;
	char *  pcStr = (char *)format;
	static int first_time = 1;
	if(first_time==1)
	{
		write_reg8(0x80013c,FIFOTHRESHOLD);
	}
	
	vaStart( args, format);
	
	while (*pcStr)
	{                       /* this works because args are all ints */
	    	if (*pcStr == '%')
	        	pcStr = FormatItem(pcStr + 1, vaArg(args, int));
	    	else
	        	uart_putc(*pcStr++);
	}
	
}

void vPrintDisplayMessage( const char * pcMessageToSend )
{
	portDEBUG_PRINTF(pcMessageToSend);
}


#endif // IO_DEBUG_MODE
