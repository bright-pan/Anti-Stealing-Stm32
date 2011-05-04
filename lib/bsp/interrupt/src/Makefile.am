######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue Apr 19 17:42:07 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Wed May  4 09:46:25 2011
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
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libbsp-interrput.la
libbsp_interrput_la_SOURCES = stm32f10x_it.c
libbsp_interrput_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libbsp_interrput_la_LDFLAGS = -all-static -static-libtool-libs

