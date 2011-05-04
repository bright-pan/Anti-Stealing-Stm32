######################################################################
## Filename:      Makefile.am
## Version:       
##                
## Author:        Bright Pan <loststriker@gmail.com>
## Created at:    Tue Apr 19 17:24:48 2011
##                
##                
## Modified by:   Bright Pan <loststriker@gmail.com>
## Modified at:   Wed May  4 09:52:04 2011
##                
## Description:   for main.elf program, linking some libraries.
## Copyright (C) 2010-2011,  Bright Pan
######################################################################

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CPFLAGS = -Obinary
ODFLAGS	= -S

CCLD = arm-none-eabi-ld

LIBPATH = /usr/local/stm32/arm-none-eabi/lib/thumb

include $(top_srcdir)/src/Makefile.include

bin_PROGRAMS = main.elf
main_elf_SOURCES = main.c
main_elf_LDFLAGS = -T$(srcdir)/stm32f103vct6.gld \
	-nostartfiles -static -static-libtool-libs

main_elf_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
main_elf_LDADD = $(top_builddir)/lib/bsp/init/src/libbsp-init.la \
	$(top_builddir)/lib/bsp/led/src/libbsp-led.la \
	$(top_builddir)/lib/bsp/calender/src/libbsp-calender.la \
	$(top_builddir)/lib/bsp/signal/src/libbsp-signal.la \
	$(top_builddir)/lib/bsp/sflash/src/libbsp-sflash.la \
	$(top_builddir)/lib/bsp/temperature/src/libbsp-temperature.la \
	$(top_builddir)/lib/bsp/interrupt/src/libbsp-interrput.la \
	$(top_builddir)/lib/cm3/src/libcm3.la \
	$(top_builddir)/lib/std_periph/src/libstd_periph.la \
	$(top_builddir)/lib/bsp/system/src/libbsp-system.la \
	$(top_builddir)/lib/ucos/ports/src/libucos-ports.la \
	$(top_builddir)/lib/ucos/core/src/libucos-core.la \
	$(LIBPATH)/libc.a
main_elf_LINK = $(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=link $(CCLD) \
	 $(main_elf_LDFLAGS) $(LDFLAGS) -o $@

copy:
	$(CP) $(CPFLAGS) main.elf main.bin
	$(OD) $(ODFLAGS) main.elf > main.list

all: $(all) copy
