######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Wed Apr 20 16:26:41 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Wed May  4 09:49:40 2011
##           n     
## Description:   for signal library
## Copyright (C) 2010-2011,  Bright Pan
######################################################################

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
LD = arm-none-eabi-ld

CCLD = $(LD)
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libbsp-signal.la
libbsp_signal_la_SOURCES = signal.c
libbsp_signal_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libbsp_signal_la_LDFLAGS = -all-static -static-libtool-libs 

