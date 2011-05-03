######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue May  3 09:41:36 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Tue May  3 13:45:43 2011
##                
## Description:   for temperature library
## Copyright (C) 2010-2011,  Bright Pan
######################################################################

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
LD = arm-none-eabi-ld

CCLD = $(LD)

lib_LTLIBRARIES = libbsp-temperature.la
libbsp_temperature_la_SOURCES = temperature.c
libbsp_temperature_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(srcdir)/../inc \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/bsp/system/inc \
	-I$(top_srcdir)/lib/bsp/init/inc \
	-I$(top_srcdir)/lib/bsp/led/inc \
	-I$(top_srcdir)/lib/bsp/calender/inc \
	-I$(top_srcdir)/lib/bsp/signal/inc \
	-I$(top_srcdir)/lib/bsp/sflash/inc \
	-I$(top_srcdir)/lib/bsp/interrupt/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/ucos/core/inc \
	-I$(top_srcdir)/lib/std_periph/inc
libbsp_temperature_la_LDFLAGS = -all-static -static-libtool-libs 

