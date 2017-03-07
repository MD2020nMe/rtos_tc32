GCCPATH = /opt/tc32/bin/tc32
AS      = $(GCCPATH)-elf-as
CC      = $(GCCPATH)-elf-gcc
LD      = $(GCCPATH)-elf-ld
NM      = $(GCCPATH)-elf-nm
OBJCOPY = $(GCCPATH)-elf-objcopy
OBJDUMP = $(GCCPATH)-elf-objdump
AR 		= $(GCCPATH)-elf-ar

TARGET = 8269_drv

CFLAGS_T       = -c  -O2
CFLAGS_T1       = -c  -O0
NMFLAGS		= -n
ASMFLAGS	= 

OBJS= bsp.o adc.o audio.c pm.o pwm.o led.o div_mod.o 
LIBS    = 

all:$(TARGET).a

clean:
	rm -rf *.mot *.abs *.map *.lst *.o *.bin  *.txt *.s *.ll *.bc *.a

$(TARGET).a:$(OBJS)
	$(AR)  -r $@ $(OBJS)

%.o: %.c
	@echo Compiling $<
	$(CC) $(CFLAGS_T1) -c $< -o $@    
	$(OBJDUMP) -x -D -l -S $*.o > $*.lst

div_mod.o: div_mod.S
	$(CC) $< $(CFLAGS_T) -o $*.o
	$(OBJDUMP) -x -D -l -S $*.o > $*.lst
	


