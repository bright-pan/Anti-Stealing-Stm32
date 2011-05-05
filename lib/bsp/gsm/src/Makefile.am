######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue May  3 09:41:36 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Thu May  5 13:41:00 2011
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
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libbsp-gsm.la
libbsp_gsm_la_SOURCES = gsm.c sms.c
libbsp_gsm_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libbsp_gsm_la_LDFLAGS = -all-static -static-libtool-libs 
