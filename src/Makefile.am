CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CPFLAGS = -Obinary
ODFLAGS	= -S

CCLD = arm-none-eabi-ld


bin_PROGRAMS = main.elf
main_elf_SOURCES = main.c
main_elf_LDFLAGS = -T$(srcdir)/stm32f103vct6.gld \
	-nostartfiles -static-libtool-libs
main_elf_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/lib/bsp/system/inc \
	-I$(top_srcdir)/lib/cm3/inc \
	-I$(top_srcdir)/lib/std_periph/inc \
	-I$(top_srcdir)/lib/bsp/interrupt/inc \
	-I$(top_srcdir)/lib/bsp/init/inc \
	-I$(top_srcdir)/lib/ucos/ports/inc \
	-I$(top_srcdir)/lib/ucos/core/inc \
	-I$(top_srcdir)/lib/bsp/system/inc
main_elf_LDADD = $(top_builddir)/lib/bsp/init/src/libbsp-init.la \
	$(top_builddir)/lib/bsp/interrupt/src/libbsp-interrput.la \
	$(top_builddir)/lib/cm3/src/libcm3.la \
	$(top_builddir)/lib/std_periph/src/libstd_periph.la \
	$(top_builddir)/lib/bsp/system/src/libbsp-system.la \
	$(top_builddir)/lib/ucos/ports/src/libucos-ports.la \
	$(top_builddir)/lib/ucos/core/src/libucos-core.la
main_elf_LINK = $(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=link $(CCLD) \
	$(main_elf_LDFLAGS) $(LDFLAGS) -o $@


copy:
	$(CP) $(CPFLAGS) main.elf main.bin
	$(OD) $(ODFLAGS) main.elf > main.list

all: $(all) copy
