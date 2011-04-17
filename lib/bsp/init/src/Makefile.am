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
	-I$(top_srcdir)/lib/bsp/interrupt/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/ucos/core/inc \
	-I$(top_srcdir)/lib/std_periph/inc
libbsp_init_la_LDFLAGS = -all-static -static-libtool-libs 
#	-L$(top_builddir)/lib/std_periph/src \
#	-lstd_periph.la \
#	-L$(top_builddir)/lib/cm3/src \
#	-lcm3.la

