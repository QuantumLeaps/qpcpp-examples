##############################################################################
# Makefile for QUTEST-QP/C++ on the host (Windows/Linux/macOS)
#
# Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
#
#                   Q u a n t u m  L e a P s
#                   ------------------------
#                   Modern Embedded Software
#
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
#
# The QP/C software is dual-licensed under the terms of the open-source GNU
# General Public License (GPL) or under the terms of one of the closed-
# source Quantum Leaps commercial licenses.
#
# Redistributions in source code must retain this top-level comment block.
# Plagiarizing this software to sidestep the license obligations is illegal.
#
# NOTE:
# The GPL (see <www.gnu.org/licenses/gpl-3.0>) does NOT permit the
# incorporation of the QP/C software into proprietary programs. Please
# contact Quantum Leaps for commercial licensing options, which expressly
# supersede the GPL and are designed explicitly for licensees interested
# in using QP/C in closed-source proprietary applications.
#
# Quantum Leaps contact information:
# <www.state-machine.com/licensing>
# <info@state-machine.com>
##############################################################################
#
# examples of invoking this Makefile:
# make         # make and run the Python tests in the current directory
# make TESTS=test*.py  # make and run the selected tests in the curr. dir.
# make norun   # only make but not run the tests
# make clean   # cleanup the build
# make debug   # only run tests in DEBUG mode
#
# NOTE:
# To use this Makefile on Windows, you will need the GNU make utility, which
# is included in the QTools collection for Windows, see:
#    https://github.com/QuantumLeaps/qtools

#-----------------------------------------------------------------------------
# project:
#
PROJECT := test_qutest

VPATH := . \
	../src

INCLUDES := -I. \
	-I../src

C_SRCS :=

CPP_SRCS :=

DEFINES  :=

LIB_DIRS :=
LIBS     :=

OUTPUT   := $(PROJECT)

#-----------------------------------------------------------------------------
# target
#
TARGET   := host

DEFINES += -DQ_SPY -DQ_UTEST \
	-DQ_HOST

BIN_DIR := build_$(TARGET)

#-----------------------------------------------------------------------------
# QP framework
#

# location of the QP/C++ framework (if not provided in an env. variable)
ifeq ($(QPCPP),)
QPCPP := ../../../..
endif

ifeq ($(OS),Windows_NT)

QP_PORT_DIR := $(QPCPP)/ports/win32-qutest
LIBS += -lws2_32

else

QP_PORT_DIR := $(QPCPP)/ports/posix-qutest
#QP_PORT_DIR := $(QPCPP)/ports/posix
LIBS += -lpthread

endif

VPATH += \
	$(QPCPP)/src/qs \
	$(QP_PORT_DIR)

INCLUDES += \
	-I$(QPCPP)/include \
	-I$(QP_PORT_DIR)

#-----------------------------------------------------------------------------
# QP/C++ framework sources
#
QP_SRCS := \
	qep_hsm.cpp \
	qep_msm.cpp \
	qf_act.cpp \
	qf_actq.cpp \
	qf_defer.cpp \
	qf_dyn.cpp \
	qf_mem.cpp \
	qf_ps.cpp \
	qf_qact.cpp \
	qf_qeq.cpp \
	qf_qmact.cpp \
	qf_time.cpp \
	qs.cpp \
	qs_64bit.cpp \
	qs_rx.cpp \
	qs_fp.cpp \
	qutest.cpp \
	qutest_port.cpp

VPATH += \
	$(QPCPP)/src/qf \
	$(QPCPP)/src/qs $(QP_PORT_DIR)

INCLUDES += \
	-I$(QPCPP)/include \
	-I$(QP_PORT_DIR)

# add the QP sources to the build
CPP_SRCS += $(QP_SRCS)

#-----------------------------------------------------------------------------
# GNU toolset:
# NOTE:  GNU toolset (MinGW) is included in the QTools collection for Windows, see:
#     https://www.state-machine.com/qtools
# It is assumed that %QTOOLS%\bin directory is added to the PATH
#
CC    := gcc
CPP   := g++
#LINK  := gcc    # for C programs
LINK  := g++   # for C++ programs
COV   := gcov -f

#-----------------------------------------------------------------------------
# basic utilities (included in QTools for Windows), see:
#     https://www.state-machine.com/qtools
#
ifeq ($(OS),Windows_NT)
	MKDIR      := mkdir
	RM         := rm
	TARGET_EXT := .exe
else ifeq ($(OSTYPE),cygwin)
	MKDIR      := mkdir -p
	RM         := rm -f
	TARGET_EXT := .exe
else
	MKDIR      := mkdir -p
	RM         := rm -f
	TARGET_EXT :=
endif


##############################################################################
# Typically you should not need to change anything below this line

# make sure that QTOOLS env. variable is defined...
ifeq ("$(wildcard $(QTOOLS))","")
$(error QTOOLS not found. Please install QTools and define QTOOLS env. variable)
endif

#-----------------------------------------------------------------------------
# QUTest test script utilities (requires QTOOLS):
#
ifeq ("$(wildcard $(QUTEST))","")
QUTEST := python3 $(QTOOLS)/qutest/qutest.py
endif

#-----------------------------------------------------------------------------
# build options
CFLAGS := -c -g -fno-pie -std=c11 -pedantic -Wall -Wextra -W \
	-O0 $(INCLUDES) $(DEFINES) $(DEF)

CPPFLAGS := -c -g -fno-pie -std=c++11 -pedantic -Wall -Wextra \
	-fno-rtti -fno-exceptions \
	-O0 $(INCLUDES) $(DEFINES) $(DEF)

ifndef GCC_OLD
	LINKFLAGS := -no-pie
endif

ifdef GCOV
	CFLAGS    += -fprofile-arcs -ftest-coverage
	CPPFLAGS  += -fprofile-arcs -ftest-coverage
	LINKFLAGS += -lgcov --coverage
endif

#-----------------------------------------------------------------------------
C_OBJS       := $(patsubst %.c,%.o,   $(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o, $(CPP_SRCS))

TARGET_EXE   := $(BIN_DIR)/$(PROJECT)$(TARGET_EXT)
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

.PHONY : norun debug clean show

ifeq ($(MAKECMDGOALS),norun)
all : $(TARGET_EXE)
norun : all
else
all : $(TARGET_EXE) run
endif

$(TARGET_EXE) : $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(CPP) $(CPPFLAGS) $(QPCPP)/src/qs/qstamp.cpp -o $(BIN_DIR)/qstamp.o
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(BIN_DIR)/qstamp.o $(LIBS)

run : $(TARGET_EXE)
	$(QUTEST) -e$(TARGET_EXE) -q$(QSPY) -l$(LOG) -o$(OPT) -- $(TESTS)
ifdef GCOV
	$(COV) -o $(BIN_DIR) $(PROJECT)
endif

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

# create BIN_DIR and include dependencies only if needed
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
     ifneq ($(MAKECMDGOALS),debug)
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
     endif
  endif
endif

debug :
	$(QUTEST) -edebug -q$(QSPY) -l$(LOG) -o$(OPT) -- $(TESTS)

clean :
	-$(RM) $(BIN_DIR)/*.*

show :
	@echo PROJECT      = $(PROJECT)
	@echo BIN_DIR      = $(BIN_DIR)
	@echo TARGET_EXE   = $(TARGET_EXE)
	@echo MAKECMDGOALS = $(MAKECMDGOALS)
	@echo TESTS        = $(TESTS)
	@echo VPATH        = $(VPATH)
	@echo C_SRCS       = $(C_SRCS)
	@echo CPP_SRCS     = $(CPP_SRCS)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo CPP_OBJS_EXT = $(CPP_OBJS_EXT)
	@echo LIB_DIRS     = $(LIB_DIRS)
	@echo LIBS         = $(LIBS)
	@echo DEFINES      = $(DEFINES)
	@echo QTOOLS       = $(QTOOLS)
	@echo HOST         = $(HOST)
	@echo QUTEST       = $(QUTEST)
	@echo TESTS        = $(TESTS)
