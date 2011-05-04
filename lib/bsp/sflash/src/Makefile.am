######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue Apr 19 17:45:49 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Wed May  4 09:48:12 2011
##                
## Description:   for sflash library
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

lib_LTLIBRARIES = libbsp-sflash.la
libbsp_sflash_la_SOURCES = sflash.c
libbsp_sflash_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libbsp_sflash_la_LDFLAGS = -all-static -static-libtool-libs 

