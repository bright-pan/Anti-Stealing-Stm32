CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = arm-none-eabi-ld

lib_LTLIBRARIES = libinterrput.la
libinterrput_la_SOURCES = stm32f10x_it.c
libinterrput_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(srcdir)/../inc \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/std_periph/inc \
	-I$(top_srcdir)/lib/bsp/system/inc
libinterrput_la_LDFLAGS = -all-static -static-libtool-libs
