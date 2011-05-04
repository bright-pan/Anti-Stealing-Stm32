CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = arm-none-eabi-ld
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libbsp-system.la
libbsp_system_la_SOURCES = system_stm32f10x.c
libbsp_system_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libbsp_system_la_LDFLAGS = -all-static -static-libtool-libs

