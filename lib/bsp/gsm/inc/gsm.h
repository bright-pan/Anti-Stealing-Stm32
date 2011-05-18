/*********************************************************************
 * Filename:      gsm.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Wed May  4 09:59:51 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed May 18 09:48:17 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __GSM_H__
#define __GSM_H__
#include "includes.h"

#define GSM_USART3 USART3
#define GSM_USART3_IRQn USART3_IRQn
#define GSM_USART3_PREEMPTION_PRIORITY 1

#define GSM_RESEND_NUMBERS 5
#define	SEND_ALL 0
#define	RECEIVE_ALL	0 

#define BUF_SIZE 512

#define INC(a) ((a) = ((a)+1) & (BUF_SIZE-1))
#define DEC(a) ((a) = ((a)-1) & (BUF_SIZE-1))
#define EMPTY(a) ((a).head == (a).tail)
#define FLUSH(a) ((a).head = (a).tail = 0)
#define LEFT(a) (((a).tail-(a).head-1)&(BUF_SIZE-1))
#define LAST(a) ((a).buf[(BUF_SIZE-1)&((a).head-1)])
#define FULL(a) (!LEFT(a))
#define CHARS(a) (((a).head-(a).tail)&(BUF_SIZE-1))
#define GETCH(queue,c) \
  (void)({c=(queue).buf[(queue).tail];INC((queue).tail);})
#define PUTCH(c,queue)											\
  (void)({(queue).buf[(queue).head]=(c);INC((queue).head);})

typedef struct {
	uint16_t head;
	uint16_t tail;
	uint8_t buf[BUF_SIZE];
}BUFFER;

typedef struct {
	BUFFER r_buf;
	BUFFER w_buf;
}RW_BUFFER;

typedef enum {
  GSM_SETUP_ENABLE_SUCCESS,
  GSM_SETUP_ENABLE_FAILURE,
  GSM_SETUP_DISABLE_SUCCESS,
  GSM_SETUP_DISABLE_FAILURE,
  GSM_RESET_SUCCESS,
  GSM_RESET_FAILURE,
  GSM_SMS_SEND_SUCCESS,
  GSM_SMS_SEND_FAILURE,
}GsmStatus;

extern BUFFER gsm_buf;

GsmStatus gsm_reset(void);
GsmStatus gsm_setup(FunctionalState state);
void gsm_power(FunctionalState state);
void gsm_init(void);
char *receive_from_gsm(char *str, uint16_t str_len);
void flush_gsm_buffer(void);
void send_to_gsm(const char *str, uint16_t str_len);

#endif
