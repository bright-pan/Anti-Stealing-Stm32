######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue Apr 19 17:45:49 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Tue May  3 13:39:27 2011
##                
## Description:   for init library
## Copyright (C) 2010-2011,  Bright Pan
######################################################################

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
LD = arm-none-eabi-ld

CCLD = $(LD)

lib_LTLIBRARIES = libbsp-init.la
libbsp_init_la_SOURCES = init.c
libbsp_init_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(srcdir)/../inc \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/bsp/system/inc \
	-I$(top_srcdir)/lib/bsp/init/inc \
	-I$(top_srcdir)/lib/bsp/led/inc \
	-I$(top_srcdir)/lib/bsp/calender/inc \
	-I$(top_srcdir)/lib/bsp/signal/inc \
	-I$(top_srcdir)/lib/bsp/sflash/inc \
	-I$(top_srcdir)/lib/bsp/temperature/inc \
	-I$(top_srcdir)/lib/bsp/interrupt/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/ucos/core/inc \
	-I$(top_srcdir)/lib/std_periph/inc
libbsp_init_la_LDFLAGS = -all-static -static-libtool-libs 

