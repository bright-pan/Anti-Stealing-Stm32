######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue Apr 19 17:08:35 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Tue Apr 19 17:22:01 2011
##                
## Description:   for led function library
## Copyright (C) 2010-2011,  Bright Pan
######################################################################

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
LD = arm-none-eabi-ld

CCLD = $(LD)

lib_LTLIBRARIES = libbsp-led.la
libbsp_led_la_SOURCES = led.c
libbsp_led_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(srcdir)/../inc \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/bsp/system/inc \
	-I$(top_srcdir)/lib/bsp/init/inc \
	-I$(top_srcdir)/lib/bsp/interrupt/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/ucos/core/inc \
	-I$(top_srcdir)/lib/std_periph/inc
libbsp_led_la_LDFLAGS = -all-static -static-libtool-libs 

