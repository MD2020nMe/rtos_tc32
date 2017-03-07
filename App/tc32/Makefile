#/*
#    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
#
#	
#*/

# This assumes that you install Telink toolchain for Windows, MacOS or Linux
# Under Windows you will be using MingW/Msys environment
#
GCCPATH = /opt/tc32/bin/tc32
AS      = $(GCCPATH)-elf-as
CC      = $(GCCPATH)-elf-gcc
LD      = $(GCCPATH)-elf-ld
NM      = $(GCCPATH)-elf-nm
OBJCOPY = $(GCCPATH)-elf-objcopy
OBJDUMP = $(GCCPATH)-elf-objdump
ARCH	= $(GCCPATH)-elf-ar
SIZE	= $(GCCPATH)-elf-size

# type of heap library to use
# heap1 and heap2 are supported at this stage
#
HEAP 	= heap2

CRT0=cstartup_8269.S 

WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wunused

OTHER_CFLAGS=-fdata-sections -ffunction-sections

TARGET  = rtos

# This allow you to use portDEBUG_PRINTF()
DEBUG_FLAGS = -DTDEBUG=1 -O2 

# It's unreal for people at this stage and still  using old C standards - PS
CFLAGS=$(WARNINGS) $(DEBUG_FLAGS) $(OTHER_FLAGS) --std=c99  -I. -I./Bsp \
	    -I../../Source/include \
	    -I../../Source/portable/gcc/tc32 \
		-I../Common/include \
		-I../Common/Minimal \
	
CFLAGS_T = -c -O2

LINKER_FLAGS=-Xlinker -o$(TARGET).abs -Xlinker -M -Xlinker -Map=$(TARGET).map

LDFLAGS = -Wl,--gc-sections -T boot.link

RTOS_SOURCE_DIR=../../Source
DEMO_SOURCE_DIR=../Common/Minimal
BSP_SOURCE_DIR=./bsp
UTL_SOURCE_DIR=./util

#$(DEMO_SOURCE_DIR)/comtest_strings.c \

TC32_SRC = \
main.c \
$(BSP_SOURCE_DIR)/bsp.c \
$(BSP_SOURCE_DIR)/adc.c \
$(BSP_SOURCE_DIR)/led.c \
$(BSP_SOURCE_DIR)/rf.c \
$(BSP_SOURCE_DIR)/uprintf.c \
$(BSP_SOURCE_DIR)/uart.c \
$(BSP_SOURCE_DIR)/pm.c \
$(BSP_SOURCE_DIR)/pwm.c \
$(BSP_SOURCE_DIR)/audio.c \
$(UTL_SOURCE_DIR)/memcpy.c \
$(UTL_SOURCE_DIR)/memset.c \
$(DEMO_SOURCE_DIR)/EventGroupsDemo.c \
$(DEMO_SOURCE_DIR)/flash.c \
$(DEMO_SOURCE_DIR)/flash_timer.c \
$(DEMO_SOURCE_DIR)/integer.c \
$(DEMO_SOURCE_DIR)/PollQ.c \
$(DEMO_SOURCE_DIR)/BlockQ.c \
$(DEMO_SOURCE_DIR)/QPeek.c \
$(DEMO_SOURCE_DIR)/TimerDemo.c \
$(DEMO_SOURCE_DIR)/blocktim.c \
$(DEMO_SOURCE_DIR)/GenQTest.c \
$(DEMO_SOURCE_DIR)/QueueSet.c \
$(DEMO_SOURCE_DIR)/QueueSetPolling.c \
$(DEMO_SOURCE_DIR)/QueueOverwrite.c \
$(DEMO_SOURCE_DIR)/StaticAllocation.c \
$(DEMO_SOURCE_DIR)/TaskNotify.c \
$(DEMO_SOURCE_DIR)/countsem.c \
$(DEMO_SOURCE_DIR)/crflash.c \
$(DEMO_SOURCE_DIR)/crhook.c \
$(DEMO_SOURCE_DIR)/death.c \
$(DEMO_SOURCE_DIR)/IntQueue.c \
$(DEMO_SOURCE_DIR)/IntSemTest.c \
$(DEMO_SOURCE_DIR)/recmutex.c \
$(DEMO_SOURCE_DIR)/semtest.c \
$(DEMO_SOURCE_DIR)/dynamic.c \
$(DEMO_SOURCE_DIR)/AbortDelay.c \
$(RTOS_SOURCE_DIR)/tasks.c \
$(RTOS_SOURCE_DIR)/timers.c \
$(RTOS_SOURCE_DIR)/queue.c \
$(RTOS_SOURCE_DIR)/list.c \
$(RTOS_SOURCE_DIR)/event_groups.c \
$(RTOS_SOURCE_DIR)/portable/GCC/TC32/port.c \
$(RTOS_SOURCE_DIR)/portable/GCC/TC32/portISR.c 


ifeq "$(HEAP)" "heap2"
	TC32_SRC += $(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c 
else
	TC32_SRC += $(RTOS_SOURCE_DIR)/portable/MemMang/heap_1.c 
endif 

#
# Define all object files.
#
TC32_OBJ = $(TC32_SRC:.c=.o)

DRV_LIB = 8269_drv.a
DRV_DIR = ./bsp
LIBS = $(DRV_LIB)


all: $(TARGET).elf

$(DRV_LIB): 
	cd $(DRV_DIR);make
	cp $(DRV_DIR)/$(DRV_LIB) $(DRV_LIB)


$(TARGET).elf : $(ASM_OBJ) $(TC32_OBJ) $(CRT0) $(LIBS) Makefile FreeRTOSconfig.h
	$(CC) $(CFLAGS) $(TC32_OBJ) $(LIBS)  -nostartfiles -nostdlib $(CRT0) $(LDFLAGS) $(LINKER_FLAGS)
	$(OBJCOPY) -O binary $(TARGET).abs $(TARGET).bin
	$(OBJCOPY) -O elf32-littletc32  $(TARGET).abs $(TARGET).elf
	$(OBJDUMP) -x -D -l -S $(TARGET).abs > $(TARGET).lst
	$(SIZE) --format=SysV $(TARGET).elf

$(TC32_OBJ) : %.o : %.c $(LDSCRIPT) Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@
	$(OBJDUMP) -x -D -l -S $*.o > $*.lst
	
print-%  : ; @echo $* = $($*)

clean :
	cd $(DRV_DIR);make clean
	rm -rf $(TC32_OBJ) boot.o  *.map *.mot *.elf *.bin *.lst *.a *.abs *.i *.s *.list
	find ../.. -name "*.lst" -exec rm {} \;
	find ../.. -name "*.o" -exec rm {} \;
	find ../.. -name "*.a" -exec rm {} \;
	find ../.. -name "cscope.*" -exec rm {} \;
	find ../.. -name "tags" -exec rm {} \;
	touch Makefile










	


