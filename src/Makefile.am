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
	-I$(top_srcdir)/lib/bsp/interrupt/inc
main_elf_LDADD = $(top_builddir)/lib/std_periph/src/libstm32periph.la \
	$(top_builddir)/lib/bsp/interrupt/src/libinterrput.la
main_elf_LINK = $(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=link $(CCLD) \
	$(main_elf_LDFLAGS) $(LDFLAGS) -o $@

copy:
	$(CP) $(CPFLAGS) main.elf main.bin
	$(OD) $(ODFLAGS) main.elf > main.list
