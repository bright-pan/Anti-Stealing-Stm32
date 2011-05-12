/*********************************************************************
 * Filename:      gsm.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Wed May  4 10:00:38 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu May 12 13:09:33 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define GSM_POWER_PIN                    GPIO_Pin_2
#define GSM_POWER_PORT                   GPIOC
#define GSM_POWER_CLK                    RCC_APB2Periph_GPIOC

#define GSM_TERM_ON_PIN                  GPIO_Pin_8
#define GSM_TERM_ON_PORT                 GPIOC
#define GSM_TERM_ON_CLK                  RCC_APB2Periph_GPIOC

#define GSM_RESET_PIN                    GPIO_Pin_7
#define GSM_RESET_PORT                   GPIOC
#define GSM_RESET_CLK                    RCC_APB2Periph_GPIOC

#define GSM_RIN_PIN                      GPIO_Pin_10
#define GSM_RIN_PORT                     GPIOD
#define GSM_RIN_CLK                      RCC_APB2Periph_GPIOD
#define GSM_RIN_READ()                   (GSM_RIN_PORT->IDR & GSM_RIN_PIN)

#define GSM_DSR_PIN                      GPIO_Pin_13
#define GSM_DSR_PORT                     GPIOD
#define GSM_DSR_CLK                      RCC_APB2Periph_GPIOD
#define GSM_DSR_READ()                   (GSM_DSR_PORT->IDR & GSM_DSR_PIN)

#define GSM_DTR_PIN                      GPIO_Pin_15
#define GSM_DTR_PORT                     GPIOD
#define GSM_DTR_CLK                      RCC_APB2Periph_GPIOD
#define GSM_DTR_READ()                   (GSM_DTR_PORT->IDR & GSM_DTR_PIN)

#define GSM_DCD_PIN                      GPIO_Pin_6
#define GSM_DCD_PORT                     GPIOC
#define GSM_DCD_CLK                      RCC_APB2Periph_GPIOC
#define GSM_DCD_READ()                   (GSM_DCD_PORT->IDR & GSM_DCD_PIN)

#define GSM_VIO_PIN                      GPIO_Pin_14
#define GSM_VIO_PORT                     GPIOD
#define GSM_VIO_CLK                      RCC_APB2Periph_GPIOD
#define GSM_VIO_READ()                   (GSM_VIO_PORT->IDR & GSM_VIO_PIN)

#define GSM_USART3_CLK                   RCC_APB1Periph_USART3

#define GSM_USART3_TX_CLK                RCC_APB2Periph_GPIOD
#define GSM_USART3_TX_PIN                GPIO_Pin_8
#define GSM_USART3_TX_PORT               GPIOD

#define GSM_USART3_RX_CLK                RCC_APB2Periph_GPIOD
#define GSM_USART3_RX_PIN                GPIO_Pin_9
#define GSM_USART3_RX_PORT               GPIOD

#define GSM_USART3_CTS_CLK               RCC_APB2Periph_GPIOD
#define GSM_USART3_CTS_PIN               GPIO_Pin_11
#define GSM_USART3_CTS_PORT              GPIOD

#define GSM_USART3_RTS_CLK               RCC_APB2Periph_GPIOD
#define GSM_USART3_RTS_PIN               GPIO_Pin_12
#define GSM_USART3_RTS_PORT              GPIOD



BUFFER gsm_buf;

void gsm_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  //GSM POWER
  RCC_APB2PeriphClockCmd(GSM_POWER_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_POWER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_POWER_PORT, &GPIO_InitStructure);
  //GSM TERM_ON
  RCC_APB2PeriphClockCmd(GSM_TERM_ON_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_TERM_ON_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_TERM_ON_PORT, &GPIO_InitStructure);
  GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
  //GSM RESET
  RCC_APB2PeriphClockCmd(GSM_RESET_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_RESET_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_RESET_PORT, &GPIO_InitStructure);
  GSM_RESET_PORT->BRR = GSM_RESET_PIN;//0
  //GSM RIN
  RCC_APB2PeriphClockCmd(GSM_RIN_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_RIN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_RIN_PORT, &GPIO_InitStructure);
  //GSM DSR
  RCC_APB2PeriphClockCmd(GSM_DSR_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_DSR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_DSR_PORT, &GPIO_InitStructure);
  //GSM DTR
  RCC_APB2PeriphClockCmd(GSM_DTR_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_DTR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_DTR_PORT, &GPIO_InitStructure);
  //GSM DCD
  RCC_APB2PeriphClockCmd(GSM_DCD_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_DCD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_DCD_PORT, &GPIO_InitStructure);
  //GSM VIO
  RCC_APB2PeriphClockCmd(GSM_VIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_VIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_VIO_PORT, &GPIO_InitStructure);
  //GSM USART3 REMAP CLOCK
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  //GSM USART3 REMAP CONFIG
  GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
  //GSM UART3 RX AND CTS AS IN_FLOATING
  RCC_APB2PeriphClockCmd(GSM_USART3_RX_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_USART3_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_USART3_RX_PORT, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(GSM_USART3_CTS_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_USART3_CTS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_USART3_CTS_PORT, &GPIO_InitStructure);
  //GSM UART3 TX AND RTX AS AF_PP
  RCC_APB2PeriphClockCmd(GSM_USART3_TX_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_USART3_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_USART3_TX_PORT, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(GSM_USART3_RTS_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GSM_USART3_RTS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_USART3_RTS_PORT, &GPIO_InitStructure);
  //GSM USART3 CLOCK
  RCC_APB1PeriphClockCmd(GSM_USART3_CLK, ENABLE);

  /* USART3 configured as follow:
	 - BaudRate = 115200 baud  
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control enabled (RTS and CTS signals)
	 - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(GSM_USART3, &USART_InitStructure);
  /* Enable USARTz Receive and Over Run interrupts */
  USART_ITConfig(GSM_USART3, USART_IT_RXNE, ENABLE);
  USART_ITConfig(GSM_USART3, USART_IT_ORE, ENABLE);
  /* Enable the USART3 */
  USART_Cmd(GSM_USART3, ENABLE);
}

void gsm_power(FunctionalState state)
{
  if(state == ENABLE)
	{
	  GSM_POWER_PORT->BSRR = GSM_POWER_PIN;
	}
  else if(state == DISABLE)
	{
	  GSM_POWER_PORT->BRR = GSM_POWER_PIN;
	}
  else
	{

	}
}

uint8_t gsm_setup(FunctionalState state)
{
  if(state == ENABLE)
	{
	  if(GSM_VIO_READ() != GSM_VIO_PIN)
		{
		  //GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
		  GSM_TERM_ON_PORT->BSRR = GSM_TERM_ON_PIN;//1
		  delay_us(1000 * 1000);
		  //GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
		}
	  if(GSM_VIO_READ() == GSM_VIO_PIN)
		{
		  return GSM_SETUP_ENABLE_SUCCESS;
		}
	  else
		{
		  return GSM_SETUP_ENABLE_FAILURE;
		}
	}
  else if(state == DISABLE)
	{
	  if(GSM_VIO_READ() == GSM_VIO_PIN)
		{
		  GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
		  //GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
		  //
		  delay_us(1000 * 1000);
		  GSM_TERM_ON_PORT->BSRR = GSM_TERM_ON_PIN;//1
		  delay_us(1000 * 1000);
		  GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
		  //GSM_TERM_ON_PORT->BRR = GSM_TERM_ON_PIN;//0
		  //
		}
	  if(GSM_VIO_READ() != GSM_VIO_PIN)
		{
		  return GSM_SETUP_DISABLE_SUCCESS;
		}
	  else
		{
		  return GSM_SETUP_DISABLE_FAILURE;
		}
	}
  else
	{
	  return GSM_SETUP_DISABLE_FAILURE;
  
	}
}

uint8_t gsm_reset(void)
{
  GSM_RESET_PORT->BRR = GSM_RESET_PIN;//0
  GSM_RESET_PORT->BSRR = GSM_RESET_PIN;//1
  delay_us(20*1000);
  GSM_RESET_PORT->BRR = GSM_RESET_PIN;//0
  delay_us(1000*1000);
  if(GSM_VIO_READ() == GSM_VIO_PIN)
	{
	  return GSM_RESET_SUCCESS;
	}
  else
	{
	  return GSM_RESET_FAILURE;
	}

}

void send_to_gsm(const char *str, uint16_t str_len)
{
  //	uint8_t err;
  if(str_len)
	{
	  while(str_len--)
		{
		  USART_SendData(GSM_USART3, *str++);
		  while(USART_GetFlagStatus(GSM_USART3, USART_FLAG_TXE) == RESET)
			{
			}
		}
	}
  else
	{
	  str_len = strlen(str);
	  while(str_len--)
		{
		  USART_SendData(GSM_USART3, *str++);
		  while(USART_GetFlagStatus(GSM_USART3, USART_FLAG_TXE) == RESET)
			{
			}
		}		
	}
}

char *receive_from_gsm(char *str, uint16_t str_len)
{
  char *str_bk = str;
  uint8_t buf_length = 0;

  buf_length = CHARS(gsm_buf);

  if(buf_length == 0)
	return NULL;

  if(str_len)
	{

	  if(str_len > buf_length)
		str_len = buf_length;
		
	  while(str_len--)
		{
		  GETCH(gsm_buf, *str_bk++);
		}
	}
  else
	{
	  str_len = buf_length;
	  while(str_len--)
		{
		  GETCH(gsm_buf, *str_bk++);
		}
		
	}
  return str;
}

void flush_gsm_buffer(void)
{
  FLUSH(gsm_buf);
}
