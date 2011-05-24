/*********************************************************************
 * Filename:      rs485.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May 24 09:22:40 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May 24 10:54:00 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define RS485_DIR_PIN                    GPIO_Pin_1
#define RS485_DIR_PORT                   GPIOA
#define RS485_DIR_CLK                    RCC_APB2Periph_GPIOA

#define RS485_USART2_CLK                   RCC_APB1Periph_USART2

#define RS485_USART2_TX_CLK                RCC_APB2Periph_GPIOA
#define RS485_USART2_TX_PIN                GPIO_Pin_2
#define RS485_USART2_TX_PORT               GPIOA

#define RS485_USART2_RX_CLK                RCC_APB2Periph_GPIOA
#define RS485_USART2_RX_PIN                GPIO_Pin_3
#define RS485_USART2_RX_PORT               GPIOA
/*
#define RS485_USART2_CTS_CLK               RCC_APB2Periph_GPIOA
#define RS485_USART2_CTS_PIN               GPIO_Pin_0
#define RS485_USART2_CTS_PORT              GPIOD

#define RS485_USART2_RTS_CLK               RCC_APB2Periph_GPIOA
#define RS485_USART2_RTS_PIN               GPIO_Pin_1
#define RS485_USART2_RTS_PORT              GPIOD
*/

BUFFER rs485_buf;

static uint32_t baudrate = 0;

const static uint32_t BAUDRATE[] = {

  1200,
  2400,
  4800,
  9600,
  19200,
  115200,
  
};

void rs485_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  //RS485 UART2 RX AND CTS AS IN_FLOATING
  
  RCC_APB2PeriphClockCmd(RS485_USART2_RX_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RS485_USART2_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RS485_USART2_RX_PORT, &GPIO_InitStructure);
  /*
  RCC_APB2PeriphClockCmd(RS485_USART2_CTS_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RS485_USART2_CTS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RS485_USART2_CTS_PORT, &GPIO_InitStructure);
  */
  //RS485 UART2 TX AND RTX AS AF_PP
  RCC_APB2PeriphClockCmd(RS485_USART2_TX_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RS485_USART2_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RS485_USART2_TX_PORT, &GPIO_InitStructure);
  /*
  RCC_APB2PeriphClockCmd(RS485_USART2_RTS_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RS485_USART2_RTS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RS485_USART2_RTS_PORT, &GPIO_InitStructure);
  */
  //RS485 USART2 CLOCK
  RCC_APB1PeriphClockCmd(RS485_USART2_CLK, ENABLE);

  /* USART2 configured as follow:
	 - BaudRate = 115200 baud  
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (no RTS and CTS signals)
	 - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(RS485_USART2, &USART_InitStructure);
  /* Enable USARTz Receive and Over Run interrupts */
  USART_ITConfig(RS485_USART2, USART_IT_RXNE, ENABLE);
  USART_ITConfig(RS485_USART2, USART_IT_ORE, ENABLE);
  /* Enable the USART2 */
  USART_Cmd(RS485_USART2, ENABLE);

  //RS485 DIR
  RCC_APB2PeriphClockCmd(RS485_DIR_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);
  RS485_DIR_PORT->BRR = RS485_DIR_PIN;//0接收
  
}

FunctionalState rs485_dir_set(FunctionalState state)
{
  if(state == ENABLE)
	{
	  RS485_DIR_PORT->BSRR = RS485_DIR_PIN;//1发送
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  RS485_DIR_PORT->BRR = RS485_DIR_PIN;//0接收	  
	  return DISABLE;
	}
  else
	{
	  RS485_DIR_PORT->BRR = RS485_DIR_PIN;//0接收
	  return DISABLE;
	}
}


void send_to_rs485(const char *str, uint16_t str_len)
{
  //	uint8_t err;
  if(str_len)
	{
	  while(str_len--)
		{
		  USART_SendData(RS485_USART2, *str++);
		  while(USART_GetFlagStatus(RS485_USART2, USART_FLAG_TXE) == RESET)
			{
			}
		}
	}
  else
	{
	  str_len = strlen(str);
	  while(str_len--)
		{
		  USART_SendData(RS485_USART2, *str++);
		  while(USART_GetFlagStatus(RS485_USART2, USART_FLAG_TXE) == RESET)
			{
			}
		}		
	}
}

char *receive_from_rs485(char *str, uint16_t str_len)
{
  char *str_bk = str;
  uint16_t buf_length = 0;

  buf_length = CHARS(rs485_buf);

  if(buf_length == 0)
	return NULL;

  if(str_len)
	{

	  if(str_len > buf_length)
		str_len = buf_length;
		
	  while(str_len--)
		{
		  if(EMPTY(rs485_buf))
			{
			  //缓冲区为空就不取数据
			}
		  else
			{
			  GETCH(rs485_buf, *str_bk++);
			}
		}
	}
  else
	{
	  str_len = buf_length;
	  while(str_len--)
		{
		  if(EMPTY(rs485_buf))
			{
			  //缓冲区为空就不取数据
			}
		  else
			{
			  GETCH(rs485_buf, *str_bk++);
			}
		}
		
	}
  return str;
}

void flush_rs485_buffer(void)
{
  FLUSH(rs485_buf);
}

void rs485_baudrate_set(BaudRate rate)
{
  if(rate < BAUDRATE_1200 || rate > BAUDRATE_115200)
	{
	  rate = BAUDRATE_1200;
	}
  
  baudrate = BAUDRATE[rate];
}

