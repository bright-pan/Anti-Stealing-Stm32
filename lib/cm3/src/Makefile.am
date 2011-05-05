CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = arm-none-eabi-ld
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libcm3.la
libcm3_la_SOURCES = core_cm3.c
libcm3_la_CFLAGS  = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libcm3_la_LDFLAGS = -all-static -static-libtool-libs
