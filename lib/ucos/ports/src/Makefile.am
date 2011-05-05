CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = $(LD)
CCAS = $(CC)
CCASFLAGS = -Wall -mcpu=cortex-m3 -mthumb -g
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libucos-ports.la
libucos_ports_la_SOURCES = os_cpu_a.s os_cpu_c.c os_dbg.c
libucos_ports_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libucos_ports_la_LDFLAGS = -all-static -static-libtool-libs

