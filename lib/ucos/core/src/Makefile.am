CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = $(LD)
CCAS = $(CC)
CCASFLAGS = -Wall -mcpu=cortex-m3 -mthumb
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libucos-core.la
libucos_core_la_SOURCES = os_core.c os_flag.c os_mbox.c os_mem.c os_mutex.c os_q.c os_sem.c os_task.c os_time.c os_tmr.c
libucos_core_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libucos_core_la_LDFLAGS = -all-static -static-libtool-libs

