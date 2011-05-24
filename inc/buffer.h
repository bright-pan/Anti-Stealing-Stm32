/*********************************************************************
 * Filename:      buf.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May 24 09:59:33 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May 24 10:00:29 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __BUFFER_H__
#define __BUFFER_H__

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

#endif
