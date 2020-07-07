#
# Makefile for stm32f030 for wavetable_synthesis
#

.PHONY: all clean

#Instruments
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

#Linker script for mcu
LDSCRIPT := devices/stm32f030/mcu/stm32f030_flash.ld

#Project name
TARGET := firmware030

#Build paths
BUILD := build030

#Requared paths
SOURCED := . devices/stm32f030/mcu devices/stm32f030/mcu_funcs wavetable_synthesis devices songs
CMSISD := devices/stm32f030/library/CMSIS

#Preparation
#SOURCED := $(addprefix src/, $(SOURCES))

SOURCEF := $(wildcard $(addsuffix /*.c, $(SOURCED)))
OBJF := $(notdir $(SOURCEF))
OBJF := $(OBJF:.c=.o)
OBJF := $(addprefix $(BUILD)/,$(OBJF))

SUF := $(wildcard $(addsuffix /*.s, $(SOURCED)))
SUOBJF := $(notdir $(SUF))
SUOBJF := $(SUOBJF:.s=.o)
SUOBJF := $(addprefix $(BUILD)/,$(SUOBJF))

#Compile Flags
CFLAGS  := -g -Wall -Wno-missing-braces -std=c99
CFLAGS += -Os
CFLAGS += -flto
CFLAGS += -mthumb -mcpu=cortex-m0
CFLAGS += -mfloat-abi=soft
CFLAGS += -I$(CMSISD)/core -I$(CMSISD)/device $(addprefix -I, $(SOURCED))

#Link Flags
LDFLAGS := -Wl,-Map,$(BUILD)/$(TARGET).map -g -T$(LDSCRIPT) --specs=nano.specs --specs=nosys.specs

#Headers of libraries
SPLD := devices/stm32f030/library/STM32F0xx_StdPeriph_Lib_V1.5.0
CFLAGS += -I$(SPLD)/inc
#used lib source
OBJF += $(BUILD)/stm32f0xx_rcc.o $(BUILD)/stm32f0xx_tim.o $(BUILD)/stm32f0xx_gpio.o

#Paths for compilator
VPATH := $(SOURCED)
#path for lib source
VPATH += $(SPLD)/src

#Rules
all: mkdir $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).hex

mkdir:
	mkdir -p ./$(BUILD)

%.hex: %.elf
	#echo $(CFLAGS)
	$(OBJCOPY) -O ihex $^ $@

$(BUILD)/%.elf: suobj obj
	$(CC) $(CFLAGS) -o $@ $(SUOBJF) $(OBJF) $(LDFLAGS)

suobj: $(SUOBJF)

$(BUILD)/%.o: %.s
	$(CC) -c -MD $(CFLAGS) -o $@ $^

obj: $(OBJF)

$(BUILD)/%.o: %.c
	$(CC) -c -MD $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD)

include $(wildcard *.d)
