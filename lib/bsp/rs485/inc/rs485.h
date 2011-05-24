/*********************************************************************
 * Filename:      rs485.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May 24 09:54:32 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May 24 10:53:32 2011
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


void rs485_init(void);
FunctionalState rs485_dir_set(FunctionalState state);
char *receive_from_rs485(char *str, uint16_t str_len);
void flush_rs485_buffer(void);
void send_to_rs485(const char *str, uint16_t str_len);
void rs485_baudrate_set(BaudRate rate);


#endif
