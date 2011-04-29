/*********************************************************************
 * Filename:      signal.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 22 11:35:38 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed Apr 27 13:01:51 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/
#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include "includes.h"

//信号点发生周期
//extern uint16_t period;
//信号点数据
//extern uint32_t sine12bit[32];

#define SIGNAL_FREQ 32000//发射信号频率为32K

void signal_init(void);
void signal_send(void);
void signal_receive(void);
void signal_frequency_set(uint32_t freq);

#endif
