/*********************************************************************
 * Filename:      signal.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 22 11:35:38 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Jun  9 11:32:47 2011
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

#define SIGNAL_SEND_FREQ_TIM4_CH1_IRQn TIM4_IRQn
#define SIGNAL_SEND_FREQ_TIM4_PREEMPTION_PRIORITY 6
#define SIGNAL_RECEIVE_FREQ_TIM3_CH2_IRQn TIM3_IRQn
#define SIGNAL_RECEIVE_FREQ_TIM3_PREEMPTION_PRIORITY 5

#define ADC1_BUF_SIZE 2
#define SIGNAL_BATTERY 0
#define SIGNAL_AMP 1
extern __IO uint16_t adc1_buf[ADC1_BUF_SIZE];
/*
typedef struct {
  
  uint16_t interval;//信号间隔时间，单位为分钟
  uint16_t freq;//信号频率
  uint16_t freq_spread;//信号频率差值
  uint16_t amp_limit;//信号幅度限值
  uint16_t process_counts;//信号处理次数
  uint16_t process_interval;//信号每次处理之后的间隔，单位为毫秒
  
}SignalParameters;

*/
typedef enum {
  SIGNAL_FREQ_4000 = 0,
  SIGNAL_FREQ_8000,
  SIGNAL_FREQ_12000,
  SIGNAL_FREQ_16000,
  SIGNAL_FREQ_20000,
  SIGNAL_FREQ_24000,
  SIGNAL_FREQ_28000,
  SIGNAL_FREQ_30000,
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

void signal_amp_battery_init(void);
FunctionalState signal_amp_battery(FunctionalState state);

void signal_freq_test_init(void);
FunctionalState signal_freq_test(FunctionalState state);

void signal_power_init(void);
FunctionalState signal_send_power(FunctionalState state);
FunctionalState signal_receive_power(FunctionalState state);

void signal_send_init(void);
FunctionalState signal_send(FunctionalState state);

void signal_frequency_set(SignalFreq freq);

void external_alarm_init(void);
void beep_alarm_init(void);

FunctionalState beep_alarm_set(FunctionalState state);

FunctionalState external_alarm_set(FunctionalState state);

#endif
