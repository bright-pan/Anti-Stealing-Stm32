CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = $(LD)
CCAS = $(CC)
CCASFLAGS = -Wall -mcpu=cortex-m3 -mthumb

lib_LTLIBRARIES = libucos-core.la
libucos_core_la_SOURCES = os_core.c os_flag.c os_mbox.c os_mem.c os_mutex.c os_q.c os_sem.c os_task.c os_time.c os_tmr.c
libucos_core_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(srcdir)/../inc \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/std_periph/inc \
	-I$(top_srcdir)/lib/bsp/system/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/ucos/core/inc
libucos_core_la_LDFLAGS = -all-static -static-libtool-libs

