######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue May 24 09:20:34 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Tue May 24 10:15:32 2011
##                
## Description:   for rs485 compile
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

lib_LTLIBRARIES = libbsp-rs485.la
libbsp_rs485_la_SOURCES = rs485.c
libbsp_rs485_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libbsp_rs485_la_LDFLAGS = -all-static -static-libtool-libs 
