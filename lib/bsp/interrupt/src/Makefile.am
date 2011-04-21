######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue Apr 19 17:42:07 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Thu Apr 21 10:12:39 2011
##                
## Description:   for interrupt library
## Copyright (C) 2010-2011,  Bright Pan
######################################################################

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = arm-none-eabi-ld

lib_LTLIBRARIES = libbsp-interrput.la
libbsp_interrput_la_SOURCES = stm32f10x_it.c
libbsp_interrput_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(srcdir)/../inc \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/std_periph/inc \
	-I$(top_srcdir)/lib/bsp/system/inc \
	-I$(top_srcdir)/lib/ucos/core/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/bsp/init/inc \
	-I$(top_srcdir)/lib/bsp/led/inc \
	-I$(top_srcdir)/lib/bsp/calender/inc
libbsp_interrput_la_LDFLAGS = -all-static -static-libtool-libs

