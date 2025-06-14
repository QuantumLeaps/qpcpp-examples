##############################################################################
# Product: Makefile for QP/C++ on NUCLEO-H743ZI, embOS RTOS, GNU-ARM
# Last Updated for Version: 7.0.1
# Date of the Last Update:  2022-05-23
#
#                    Q u a n t u m  L e a P s
#                    ------------------------
#                    Modern Embedded Software
#
# Copyright (C) 2005-2021 Quantum Leaps, LLC. All rights reserved.
#
# This program is open source software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, this program may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GNU General Public License and are specifically designed for
# licensees interested in retaining the proprietary status of their code.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses>.
#
# Contact information:
# <www.state-machine.com/licensing>
# <info@state-machine.com>
##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default), Release, and Spy
# make
# make CONF=rel
# make CONF=spy
#
# cleaning configurations: Debug (default), Release, and Spy
# make clean
# make CONF=rel clean
# make CONF=spy clean
#
# NOTE:
# To use this Makefile on Windows, you will need the GNU make utility, which
# is included in the Qtools collection for Windows, see:
#    https://sourceforge.net/projects/qpc/files/QTools/
#

#-----------------------------------------------------------------------------
# project name
#
PROJECT     := dpp

#-----------------------------------------------------------------------------
# project directories
#

# location of the QP/C++ framework (if not provided in an environment var.)
ifeq ($(QP),)
QP := ../../../../..
endif

# QP port used in this project
QP_PORT_DIR := $(QP)/ports/embos

# list of all source directories used by this project
VPATH = \
	SEGGER \
	Setup \
	.. \
	../.. \
	$(QP)/src/qf \
	$(QP)/src/qs \
	$(QP_PORT_DIR) \
	$(QP)/3rd_party/STM32CubeH7/nucleo-h743zi/gnu \
	$(QP)/3rd_party/STM32CubeH7/Drivers/BSP/STM32H7xx_Nucleo_144 \
	$(QP)/3rd_party/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src \
	$(QP)/3rd_party/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates

# list of all include directories needed by this project
INCLUDES  = \
	-I.. \
	-I../.. \
	-ISEGGER \
	-I$(QP)/include \
	-I$(QP_PORT_DIR) \
	-I$(QP)/3rd_party/embOS-gnu/Start/Inc \
	-I$(QP)/3rd_party/CMSIS/Include \
	-I$(QP)/3rd_party/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
	-I$(QP)/3rd_party/STM32CubeH7/Drivers/BSP/STM32H7xx_Nucleo_144 \
	-I$(QP)/3rd_party/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc

#-----------------------------------------------------------------------------
# files
#

# assembler source files
ASM_SRCS :=

# C source files
C_SRCS := \
	JLINKMEM_Process.c \
	OS_Error.c \
	RTOSInit_STM32H7xx.c \
	startup_stm32h743xx.c \
	system_stm32h7xx.c \
	stm32h7xx_nucleo_144.c \
	stm32h7xx_hal.c \
	stm32h7xx_hal_cortex.c \
	stm32h7xx_hal_gpio.c \
	stm32h7xx_hal_pwr_ex.c \
	stm32h7xx_hal_rcc.c \
	stm32h7xx_hal_rcc_ex.c \
	stm32h7xx_hal_msp.c \
	stm32h7xx_hal_uart.c

# C++ source files
CPP_SRCS := \
	bsp.cpp \
	main.cpp \
	philo.cpp \
	table.cpp

OUTPUT    := $(PROJECT)
LD_SCRIPT := $(PROJECT).ld

QP_SRCS := \
	qep_hsm.cpp \
	qep_msm.cpp \
	qf_act.cpp \
	qf_defer.cpp \
	qf_dyn.cpp \
	qf_mem.cpp \
	qf_ps.cpp \
	qf_qact.cpp \
	qf_qeq.cpp \
	qf_qmact.cpp \
	qf_time.cpp \
	qf_port.cpp

QP_ASMS :=


QS_SRCS := \
	qs.cpp \
	qs_rx.cpp \
	qs_fp.cpp

LIB_DIRS  := -L$(QP)\3rd_party\embOS-gnu\Start\Lib
LIBS      := -losT7VLR

# defines
DEFINES   := -DSTM32H743xx -DUSE_HAL_DRIVER -DUSE_STM32H7XX_NUCLEO_144

# ARM CPU, ARCH, FPU, and Float-ABI types...
# ARM_CPU:   [cortex-m0 | cortex-m0plus | cortex-m1 | cortex-m3 | cortex-m4]
# ARM_FPU:   [ | vfp]
# FLOAT_ABI: [ | soft | softfp | hard]
#
ARM_CPU   := -mcpu=cortex-m7
ARM_FPU   := -mfpu=fpv5-d16
FLOAT_ABI := -mfloat-abi=softfp

#-----------------------------------------------------------------------------
# GNU-ARM toolset (NOTE: You need to adjust to your machine)
# see https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
#
ifeq ($(GNU_ARM),)
GNU_ARM := $(QTOOLS)/gnu_arm-none-eabi
endif

# make sure that the GNU-ARM toolset exists...
ifeq ("$(wildcard $(GNU_ARM))","")
$(error GNU_ARM toolset not found. Please adjust the Makefile)
endif

CC    := $(GNU_ARM)/bin/arm-none-eabi-gcc
CPP   := $(GNU_ARM)/bin/arm-none-eabi-g++
AS    := $(GNU_ARM)/bin/arm-none-eabi-as
LINK  := $(GNU_ARM)/bin/arm-none-eabi-g++
BIN   := $(GNU_ARM)/bin/arm-none-eabi-objcopy


##############################################################################
# Typically, you should not need to change anything below this line

# basic utilities (included in QTools for Windows), see:
#     https://www.state-machine.com/qtools

MKDIR := mkdir
RM    := rm

#-----------------------------------------------------------------------------
# build options for various configurations for ARM Cortex-M
#

# combine all the sources...
CPP_SRCS += $(QP_SRCS)
C_SRCS   += syscalls.c
ASM_SRCS += $(QP_ASMS)

ifeq (rel, $(CONF)) # Release configuration ..................................

BIN_DIR := rel

ASFLAGS = $(ARM_CPU) $(ARM_FPU) $(ASM_CPU) $(ASM_FPU)

CFLAGS = -c $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb -Wall \
	-ffunction-sections -fdata-sections \
	-O1 $(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -c $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb -Wall \
	-ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
	-O1 $(INCLUDES) $(DEFINES) -DNDEBUG

else ifeq (spy, $(CONF))  # Spy configuration ................................

BIN_DIR := spy

CPP_SRCS += $(QS_SRCS)

ASFLAGS = -g $(ARM_CPU) $(ARM_FPU) $(ASM_CPU) $(ASM_FPU)

CFLAGS = -c -g $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb -Wall \
	-ffunction-sections -fdata-sections \
	-O $(INCLUDES) $(DEFINES) -DQ_SPY

CPPFLAGS = -c -g $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb -Wall \
	-ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
	-O $(INCLUDES) $(DEFINES) -DQ_SPY

else # default Debug configuration ..........................................

BIN_DIR := dbg

ASFLAGS = -g $(ARM_CPU) $(ARM_FPU) $(ASM_CPU) $(ASM_FPU)

CFLAGS = -c -g $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb -Wall \
	-ffunction-sections -fdata-sections \
	-O $(INCLUDES) $(DEFINES)

CPPFLAGS = -c -g $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb -Wall \
	-ffunction-sections -fdata-sections -fno-rtti -fno-exceptions \
	-O $(INCLUDES) $(DEFINES)

endif # ......................................................................


LINKFLAGS = -T$(LD_SCRIPT) $(ARM_CPU) $(ARM_FPU) $(FLOAT_ABI) -mthumb \
	-specs=nosys.specs -specs=nano.specs \
	-Wl,-Map,$(BIN_DIR)/$(OUTPUT).map,--cref,--gc-sections $(LIB_DIRS)


ASM_OBJS     := $(patsubst %.s,%.o,  $(notdir $(ASM_SRCS)))
C_OBJS       := $(patsubst %.c,%.o,  $(notdir $(C_SRCS)))
CPP_OBJS     := $(patsubst %.cpp,%.o,$(notdir $(CPP_SRCS)))

TARGET_BIN   := $(BIN_DIR)/$(OUTPUT).bin
TARGET_ELF   := $(BIN_DIR)/$(OUTPUT).elf
ASM_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o, %.d, $(CPP_OBJS_EXT))

# create $(BIN_DIR) if it does not exist
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif

#-----------------------------------------------------------------------------
# rules
#

all: $(TARGET_BIN)
#all: $(TARGET_ELF)

$(TARGET_BIN): $(TARGET_ELF)
	$(BIN) -O binary $< $@

$(TARGET_ELF) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(CPP) $(CPPFLAGS) $(QP)/src/qs/qstamp.cpp -o $(BIN_DIR)/qstamp.o
	$(LINK) $(LINKFLAGS) -o $@ $^ $(BIN_DIR)/qstamp.o $(LIBS)

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

# include dependency files only if our goal depends on their existence
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
  endif
endif


.PHONY : clean
clean:
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/*.bin \
	$(BIN_DIR)/*.elf \
	$(BIN_DIR)/*.map
	
show:
	@echo PROJECT = $(PROJECT)
	@echo CONF = $(CONF)
	@echo DEFINES = $(DEFINES)
	@echo ASM_FPU = $(ASM_FPU)
	@echo ASM_SRCS = $(ASM_SRCS)
	@echo C_SRCS = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo TARGET_ELF = $(TARGET_ELF)
