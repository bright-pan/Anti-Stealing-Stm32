/*********************************************************************
 * Filename:      signal.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 22 11:35:38 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May 17 14:49:50 2011
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

typedef enum {
  SIGNAL_FREQ_4000 = 0,
  SIGNAL_FREQ_8000,
  SIGNAL_FREQ_12000,
  SIGNAL_FREQ_16000,
  SIGNAL_FREQ_20000,
  SIGNAL_FREQ_24000,
  SIGNAL_FREQ_28000,
  SIGNAL_FREQ_32000,
  SIGNAL_FREQ_36000,
  SIGNAL_FREQ_40000,
  SIGNAL_FREQ_44000,
  SIGNAL_FREQ_48000,
  SIGNAL_FREQ_52000,
  SIGNAL_FREQ_56000,
  SIGNAL_FREQ_60000,
  SIGNAL_FREQ_64000,
}SignalFreq;

void signal_init(void);
void signal_send(void);
void signal_receive(void);
void signal_frequency_set(SignalFreq freq);
FunctionalState signal_send_power(FunctionalState state);
FunctionalState signal_receive_power(FunctionalState state);


#endif
