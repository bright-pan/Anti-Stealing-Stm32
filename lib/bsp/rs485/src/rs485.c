/*********************************************************************
 * Filename:      rs485.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May 24 09:22:40 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Jun 16 10:19:35 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

extern DEVICE_INIT_PARAMATERS device_init_paramaters;

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

uint8_t function_id[2] = {0x04, 0x10};


RS485_ADDRESS_INFO rs485_address_info[28] = {
  {
	0x1100, 32,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, primary_device_name), &(device_init_paramaters.primary_device_name[0]),
  },
  {
	0x1200,	32,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, slave_device_name), &(device_init_paramaters.slave_device_name[0]),
  },
  {
	0x2100,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone_numbers), &(device_init_paramaters.alarm_telephone_numbers),
  },
  {
	0x2200,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[0]), &(device_init_paramaters.alarm_telephone[0]),
  },
  {
	0x2300,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[1]), &(device_init_paramaters.alarm_telephone[1]),
  },
  {
	0x2400,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[2]), &(device_init_paramaters.alarm_telephone[2]),
  },
  {
	0x2500,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[3]), &(device_init_paramaters.alarm_telephone[3]),
  },
  {
	0x2600,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[4]), &(device_init_paramaters.alarm_telephone[4]),
  },
  {
	0x2700,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[5]), &(device_init_paramaters.alarm_telephone[5]),
  },
  {
	0x2800,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[6]), &(device_init_paramaters.alarm_telephone[6]),
  },
  {
	0x2900,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[7]), &(device_init_paramaters.alarm_telephone[7]),
  },
  {
	0x3000,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[8]), &(device_init_paramaters.alarm_telephone[8]),
  },
  {
	0x3100,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[9]), &(device_init_paramaters.alarm_telephone[9]),
  },
  {
	0x3200,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, service_center_address), &(device_init_paramaters.service_center_address[0]),
  },
  {
	0x4100,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, password), &(device_init_paramaters.password[0]),
  },
  {
	0x4200,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, device_id), &(device_init_paramaters.device_id),
  },
  {
	0x4300,	36,	0, &(device_init_paramaters.calender),
  },
  {
	0x5100,	32,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, gps), &(device_init_paramaters.gps[0]),
  },
  {
	0x6100,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, sms_on_off), &(device_init_paramaters.sms_on_off),
  },
  {
	0x7100,
	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, interval),
	&(device_init_paramaters.signal_parameters.interval),
  },
  {
	0x7200,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, freq),
	&(device_init_paramaters.signal_parameters.freq),
  },
  {
	0x7300,	4,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, freq_spread),
	&(device_init_paramaters.signal_parameters.freq_spread),
  },
  {
	0x7400,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, amp_limit),
	&(device_init_paramaters.signal_parameters.amp_limit),
  },
  {
	0x7500,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, process_counts),
	&(device_init_paramaters.signal_parameters.process_counts),
  },
  {
	0x7600,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, process_interval),
	&(device_init_paramaters.signal_parameters.process_interval),
  },
  {
	0x7700,	10,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, send_freq),
	&(device_init_paramaters.signal_parameters.send_freq),
  },
  {
	0x7900,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, rs485_baudrate), &(device_init_paramaters.rs485_baudrate),
  },
  {
	0x8000,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, gsm_signal_strength), &(device_init_paramaters.gsm_signal_strength),
  }
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

uint16_t receive_from_rs485(char *str, uint16_t str_len)
{
  char *str_bk = str;
  uint16_t buf_length = 0;
  uint16_t truth_length = 0;
  buf_length = CHARS(rs485_buf);

  if(buf_length == 0)
	return 0;

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
			  truth_length++;
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
			  truth_length++;
			}
		}
		
	}
  return truth_length;
}

void flush_rs485_buffer(void)
{
  FLUSH(rs485_buf);
}

/*
 * Function rs485_baudrate_set (rate)
 *
 *    unsigned char *puchMsg ; // 用于计算CRC 的报文
 *    unsigned short usDataLen ; // 报文中的字节数
 *
 *
 */
void rs485_baudrate_set(BaudRate rate)
{
  if(rate < BAUDRATE_1200 || rate > BAUDRATE_115200)
	{
	  rate = BAUDRATE_1200;
	}
  
  baudrate = BAUDRATE[rate];
}

// 高位字节的CRC 值
const static uint8_t auchCRCHi[] = {

  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40
  
};

// 低位字节的CRC 值
const static uint8_t auchCRCLo[] = {

  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
  0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
  0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
  0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
  0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
  0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
  0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
  0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
  0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
  0x40
  
};

static uint8_t uchCRCHi = 0xFF; // CRC 的高字节初始化
static uint8_t uchCRCLo = 0xFF; // CRC 的低字节初始化

/*
 * Function crc_16 (puchMsg, usDataLen)
 *
 *    unsigned char *puchMsg // 用于计算CRC 的报文
 *    unsigned short usDataLen // 报文中的字节数
 *    
 *
 */
uint16_t crc_16(uint8_t *puchMsg, uint16_t usDataLen)
{
  uint16_t uIndex; // CRC 查询表索引
  while (usDataLen--) // 完成整个报文缓冲区
	{
	  uIndex = uchCRCHi ^ *puchMsg++; // 计算CRC
	  uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
	  uchCRCLo = auchCRCLo[uIndex];
	}
  return (uchCRCHi << 8 | uchCRCLo);
}

void crc_16_init(void)
{
  uchCRCHi = 0xFF; // CRC 的高字节初始化
  uchCRCLo = 0xFF; // CRC 的低字节初始化
}



int comp_function_id(const void *m1, const void *m2)
{
  uint8_t *mi1 = (uint8_t *)m1;
  uint8_t *mi2 = (uint8_t *)m2;
  if(*mi1 > *mi2)
	{
	  return 1;
	}
  else if(*mi1 == *mi2)
	{
	  return 0;
	}
  else
	{
	  return -1;
	}
}

int comp_rs485_address_info(const void *m1, const void *m2)
{
  RS485_ADDRESS_INFO *mi1 = (RS485_ADDRESS_INFO *)m1;
  RS485_ADDRESS_INFO *mi2 = (RS485_ADDRESS_INFO *)m2;
  if(mi1->address > mi2->address)
	{
	  return 1;
	}
  else if(mi1->address == mi2->address)
	{
	  return 0;
	}
  else
	{
	  return -1;
	}
}



RS485_REQUEST_FRAME *receive_rs485_frame(RS485_REQUEST_FRAME *request_frame, DEVICE_INIT_PARAMATERS *device_parameters)
{
  RS485_ADDRESS_INFO key, *res;
  void *temp = NULL;
  uint16_t crc = 0;
  crc_16_init();
  //接收device_id字节
  if(receive_from_rs485((char *)&(request_frame->device_id), 1) == 1)
	{
	  //处理device_id
	  if(request_frame->device_id == device_parameters->device_id)
		{
		  crc = crc_16((uint8_t *)&(request_frame->device_id), 1);
		  //OSTimeDlyHMSM(0, 0, 0, 1);
		  //接收function_id
		  if(receive_from_rs485((char *)&(request_frame->function_id), 1) == 1)
			{
			  crc = crc_16((uint8_t *)&(request_frame->function_id), 1);
			  //处理function_id
			  temp = (void *)bsearch((void *)&(request_frame->function_id),
									 (void *)&function_id[0],
									 nr_of_array(function_id),
									 sizeof(function_id[0]),
									 comp_function_id);
			  if(temp != NULL)
				{
				  //接收地址码
				  if(receive_from_rs485((char *)&(request_frame->address), 2) == 2)
					{
					  crc = crc_16((uint8_t *)&(request_frame->address), 2);
					  //处理地址码
					  request_frame->address = __REV16(request_frame->address);
					  //					  request_frame->address = (request_frame->address >> 8) | (request_frame->address << 8);
					  key.address = request_frame->address;
					  res = (RS485_ADDRESS_INFO *)bsearch((void *)&key,
														  (void *)&rs485_address_info[0],
														  nr_of_array(rs485_address_info),
														  sizeof(rs485_address_info[0]),
														  comp_rs485_address_info);
					  if(res != NULL)
						{
						  //接收数据长度
						  if(receive_from_rs485((char *)&(request_frame->length_16), 2) == 2)
							{
							  crc = crc_16((uint8_t *)&(request_frame->length_16), 2);
							  //处理数据长度
							  request_frame->length_16 = __REV16(request_frame->length_16);
							  if(request_frame->length_16 <= 100)
								{
								  switch(request_frame->function_id)
									{
									case 0x04 : {
									  //接收CRC码
									  if(receive_from_rs485((char *)&(request_frame->rs485_read_request_frame.crc), 2) == 2)
										{
										  crc = crc_16((uint8_t *)&(request_frame->rs485_read_request_frame.crc), 2);
										  //校验帧CRC
										  if(crc == 0x0)
											{
											  return request_frame;
											}
										  else
											{
											  return NULL;
											}
										}
									  else
										{
										  return NULL;
										}
									  break;
									}
									case 0x10 : {
									  if((receive_from_rs485((char *)&(request_frame->rs485_set_request_frame.length_8), 1) == 1))
										{
										  crc = crc_16((uint8_t *)&(request_frame->rs485_set_request_frame.length_8), 1);
										  if((receive_from_rs485((char *)&(request_frame->rs485_set_request_frame.data),
																request_frame->rs485_set_request_frame.length_8) \
											  == request_frame->rs485_set_request_frame.length_8))
											{
											  crc = crc_16((uint8_t *)&(request_frame->rs485_set_request_frame.data), request_frame->rs485_set_request_frame.length_8);//校验帧CRC
											  if(receive_from_rs485((char *)&(request_frame->rs485_set_request_frame.crc), 2) == 2)
												{
												  crc = crc_16((uint8_t *)&(request_frame->rs485_set_request_frame.crc), 2);//校验帧CRC
												  if(crc == 0x0)
													{
													  return request_frame;
													}
												  else
													{
													  return NULL;
													}
												}
											  else
												{
												  return NULL;
												}

											}
										  else
											{
											  return NULL;
											}
										}
									  else
										{
										  return NULL;
										}
									  break;
									}
									default:{
									  
									  break;
									}
									}
								  return NULL;
								}
							  else
								{
								  return NULL;
								}
							}
						  else
							{
							  return NULL;
							}
						  
						}
					  else
						{
						  return NULL;
						}
					}
				  else
					{
					  return NULL;
					}
				}
			  else
				{
				  return NULL;
				}
			}
		  else
			{
			  return NULL;
			}
		}
	  else
		{
		  return NULL;
		}
	  
	}
  else
	{
	  return NULL;
	}
}

