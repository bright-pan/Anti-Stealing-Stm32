/*********************************************************************
 * Filename:      rs485.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May 24 09:54:32 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Mon May 30 13:56:27 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __RS485_H__
#define __RS485_H__
#include "includes.h"

#define RS485_USART2 USART2
#define RS485_USART2_IRQn USART2_IRQn
#define RS485_USART2_PREEMPTION_PRIORITY 2


extern BUFFER rs485_buf;

typedef enum {
  
  BAUDRATE_1200 = 0,
  BAUDRATE_2400,
  BAUDRATE_4800,
  BAUDRATE_9600,
  BAUDRATE_19200,
  BAUDRATE_115200,
  
}BaudRate;

typedef struct {

  uint16_t crc;

}RS485_READ_REQUEST_FRAME;

typedef struct {

  
  uint8_t length_8;
  uint8_t data[200];

}RS485_SET_REQUEST_FRAME;

typedef struct {

  uint8_t device_id;
  uint8_t function_id;
  uint16_t address;
  uint16_t length_16;
  union {
	RS485_READ_REQUEST_FRAME rs485_read_request_frame;
	RS485_SET_REQUEST_FRAME rs485_set_request_frame;
  };
}RS485_REQUEST_FRAME;

typedef struct {
  uint8_t length_8;
  uint8_t data[200];

}RS485_READ_RESPONSE_FRAME;

typedef struct {
  uint16_t address;
  uint16_t length_16;
}RS485_SET_RESPONSE_FRAME;

typedef struct {

  uint8_t device_id;
  uint8_t function_id;
  union {
	RS485_READ_RESPONSE_FRAME rs485_read_response_frame;
	RS485_SET_RESPONSE_FRAME rs485_set_response_frame;
  };
  uint16_t crc;
}RS485_RESPONSE_FRAME;

void rs485_init(void);
FunctionalState rs485_dir_set(FunctionalState state);
uint16_t receive_from_rs485(char *str, uint16_t str_len);
void flush_rs485_buffer(void);
void send_to_rs485(const char *str, uint16_t str_len);
void rs485_baudrate_set(BaudRate rate);

uint16_t crc_16(uint8_t *puchMsg, uint16_t usDataLen);


#endif
