CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

CCLD = arm-none-eabi-ld
include $(top_srcdir)/src/Makefile.include

lib_LTLIBRARIES = libstd_periph.la
libstd_periph_la_SOURCES = misc.c stm32f10x_adc.c stm32f10x_bkp.c stm32f10x_can.c stm32f10x_cec.c stm32f10x_crc.c stm32f10x_dac.c stm32f10x_dbgmcu.c stm32f10x_dma.c stm32f10x_exti.c stm32f10x_flash.c stm32f10x_fsmc.c stm32f10x_gpio.c stm32f10x_i2c.c stm32f10x_iwdg.c stm32f10x_pwr.c stm32f10x_rcc.c stm32f10x_rtc.c stm32f10x_sdio.c stm32f10x_spi.c stm32f10x_tim.c stm32f10x_usart.c stm32f10x_wwdg.c
libstd_periph_la_CFLAGS = -Wall -c -fno-common -mcpu=cortex-m3 -mthumb \
	$(INCLUDE_PATH)
libstd_periph_la_LDFLAGS = -all-static -static-libtool-libs
