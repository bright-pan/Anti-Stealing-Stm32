/*********************************************************************
 * Filename:      signal.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 22 11:38:50 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed Jul  6 14:08:11 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

/* DHR registers offsets */
#define DHR12R1_OFFSET             ((uint32_t)0x00000008)
#define DHR12R2_OFFSET             ((uint32_t)0x00000014)
#define DHR12RD_OFFSET             ((uint32_t)0x00000020)

#define ADC1_DR_Address    ((uint32_t)0x4001244C)


#define POINT_PER_PERIOD 32

#define SIGNAL_DEFAULT_FREQ 30000
//发送电源引脚
#define SIGNAL_SEND_POWER_PIN                    GPIO_Pin_5
#define SIGNAL_SEND_POWER_PORT                   GPIOE
#define SIGNAL_SEND_POWER_CLK                    RCC_APB2Periph_GPIOE
//接收电源引脚
#define SIGNAL_RECEIVE_POWER_PIN                 GPIO_Pin_6
#define SIGNAL_RECEIVE_POWER_PORT                GPIOE
#define SIGNAL_RECEIVE_POWER_CLK                 RCC_APB2Periph_GPIOE

//发送信号频率检测引脚
#define SIGNAL_SEND_FREQ_PIN                 GPIO_Pin_6
#define SIGNAL_SEND_FREQ_PORT                GPIOB
#define SIGNAL_SEND_FREQ_CLK                 RCC_APB2Periph_GPIOB
//接收信号频率检测引脚
#define SIGNAL_RECEIVE_FREQ_PIN                 GPIO_Pin_5
#define SIGNAL_RECEIVE_FREQ_PORT                GPIOB
#define SIGNAL_RECEIVE_FREQ_CLK                 RCC_APB2Periph_GPIOB
//接收信号幅值检测引脚
#define SIGNAL_RECEIVE_AMP_PIN                 GPIO_Pin_0
#define SIGNAL_RECEIVE_AMP_PORT                GPIOB
#define SIGNAL_RECEIVE_AMP_CLK                 RCC_APB2Periph_GPIOB
//接收机电池检测引脚
#define SIGNAL_BATTERY_PIN                 GPIO_Pin_5
#define SIGNAL_BATTERY_PORT                GPIOA
#define SIGNAL_BATTERY_CLK                 RCC_APB2Periph_GPIOA

//蜂鸣器告警引脚
#define BEEP_ALARM_PIN                 GPIO_Pin_0
#define BEEP_ALARM_PORT                GPIOA
#define BEEP_ALARM_CLK                 RCC_APB2Periph_GPIOA
//外部告警引脚
#define EXTERNAL_ALARM_PIN                 GPIO_Pin_9
#define EXTERNAL_ALARM_PORT                GPIOA
#define EXTERNAL_ALARM_CLK                 RCC_APB2Periph_GPIOA



//信号频率数组
const static uint32_t signal_freq[] = {
  40,
  120,
  480,
  960,
  4000,
  8000,
  12000,
  16000,
  20000,
  24000,
  28000,
  30000,
  32000,
  36000,
  40000,
  44000,
  48000,
  52000,
  56000,
  60000,
  64000,
};


//信号点数据
static uint32_t sine12bit[POINT_PER_PERIOD] = {

  2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
  3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
  599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647

};

//信号点发生周期
static uint16_t period = SIGNAL_DEFAULT_FREQ;


//ADC1转换的数据数组
__IO uint16_t adc1_buf[ADC1_BUF_SIZE];


//DAC初始化结构体定义
DAC_InitTypeDef DAC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;

void signal_amp_battery_init(void)
{
  //DMA初始化结构体定义
  DMA_InitTypeDef            DMA_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  //接收幅值检测引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_RECEIVE_AMP_CLK, ENABLE);
  //设置成模拟输入
  GPIO_InitStructure.GPIO_Pin =  SIGNAL_RECEIVE_AMP_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_RECEIVE_AMP_PORT, &GPIO_InitStructure);
  //接收机电池检测引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_RECEIVE_AMP_CLK, ENABLE);
  //设置成模拟输入
  GPIO_InitStructure.GPIO_Pin =  SIGNAL_RECEIVE_AMP_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_RECEIVE_AMP_PORT, &GPIO_InitStructure);
  //接收信号幅值及接收机电池AD采样配置
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc1_buf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC1_BUF_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  
  /* ADC1 configuration ------------------------------------------------------*/
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = ADC1_BUF_SIZE;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);

  
}

FunctionalState signal_amp_battery(FunctionalState state)
{
  if(state == ENABLE)
	{
	  /* Enable DMA1 channel1 */
	  DMA_Cmd(DMA1_Channel1, ENABLE);
	  /* Enable ADC1 DMA */
	  ADC_DMACmd(ADC1, ENABLE);

	  /* Enable ADC1 */
	  ADC_Cmd(ADC1, ENABLE);

	  /* Enable ADC1 reset calibaration register */   
	  ADC_ResetCalibration(ADC1);
	  /* Check the end of ADC1 reset calibration register */
	  while(ADC_GetResetCalibrationStatus(ADC1));

	  /* Start ADC1 calibaration */
	  ADC_StartCalibration(ADC1);
	  /* Check the end of ADC1 calibration */
	  while(ADC_GetCalibrationStatus(ADC1));
	  //ADC1采样开始
	  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  /* Enable DMA1 channel1 */
	  DMA_Cmd(DMA1_Channel1, DISABLE);
	  /* Enable ADC1 DMA */
	  ADC_DMACmd(ADC1, DISABLE);
	  /* Enable ADC1 */
	  ADC_Cmd(ADC1, DISABLE);
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  /* Enable DMA1 channel1 */
	  DMA_Cmd(DMA1_Channel1, DISABLE);
	  /* Enable ADC1 DMA */
	  ADC_DMACmd(ADC1, DISABLE);
	  /* Enable ADC1 */
	  ADC_Cmd(ADC1, DISABLE);
	  return DISABLE;
	}
}



void signal_freq_test_init(void)
{
  //TIM初始化结构体定义
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  uint16_t PrescalerValue = (uint16_t)(SystemCoreClock / 12000000) - 1;

  //发送频率检测引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_SEND_FREQ_CLK, ENABLE);
  //设置成浮空输入
  GPIO_InitStructure.GPIO_Pin =  SIGNAL_SEND_FREQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_SEND_FREQ_PORT, &GPIO_InitStructure);

  //发送输入捕获配置
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM4, &TIM_ICInitStructure);
  //设置TIM4的计数器基准频率为6M
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);
  // Enable the CC1 Interrupt Request
  TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
  // TIM enable counter
  //  TIM_Cmd(TIM4, ENABLE);

  //接收频率检测引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_RECEIVE_FREQ_CLK, ENABLE);
  //设置成浮空输入
  GPIO_InitStructure.GPIO_Pin =  SIGNAL_RECEIVE_FREQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_RECEIVE_FREQ_PORT, &GPIO_InitStructure);
  //GPIO REMAP CLOCK
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  //PIN PB5 TIM3－2 REMAP CONFIG
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
  //接收输入捕获配置
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);
  //设置TIM3的计数器基准频率为6M
  TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
  // Enable the CC2 Interrupt Request
  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
  // TIM enable counter
  //  TIM_Cmd(TIM3, ENABLE);

}

FunctionalState signal_freq_test(FunctionalState state)
{
  if(state == ENABLE)
	{
	  // TIM enable counter
	  TIM_Cmd(TIM3, ENABLE);
	  // TIM enable counter
	  TIM_Cmd(TIM4, ENABLE);
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  // TIM enable counter
	  TIM_Cmd(TIM3, DISABLE);
	  // TIM enable counter
	  TIM_Cmd(TIM4, DISABLE);
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  // TIM enable counter
	  TIM_Cmd(TIM3, DISABLE);
	  // TIM enable counter
	  TIM_Cmd(TIM4, DISABLE);
	  return DISABLE;
	}
}

void signal_power_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //使能发送电源引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_SEND_POWER_CLK, ENABLE);
  //配置发送电源引脚为输出
  GPIO_InitStructure.GPIO_Pin = SIGNAL_SEND_POWER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_SEND_POWER_PORT, &GPIO_InitStructure);
  SIGNAL_SEND_POWER_PORT->BRR = SIGNAL_SEND_POWER_PIN;//默认为0,关闭状态
  //使能接收电源引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_RECEIVE_POWER_CLK, ENABLE);
  //配置接收电源引脚为输出
  GPIO_InitStructure.GPIO_Pin = SIGNAL_RECEIVE_POWER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_RECEIVE_POWER_PORT, &GPIO_InitStructure);
  SIGNAL_RECEIVE_POWER_PORT->BRR = SIGNAL_RECEIVE_POWER_PIN;//默认为关闭状态  
}

FunctionalState signal_send_power(FunctionalState state)
{
  if(state == ENABLE)
	{
	  SIGNAL_SEND_POWER_PORT->BSRR = SIGNAL_SEND_POWER_PIN;//设置为1，状态为开启
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  SIGNAL_SEND_POWER_PORT->BRR = SIGNAL_SEND_POWER_PIN;//设置为0，状态为关闭
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  SIGNAL_SEND_POWER_PORT->BRR = SIGNAL_SEND_POWER_PIN;//设置为0，状态为关闭
	  return DISABLE;
	}
}

FunctionalState signal_receive_power(FunctionalState state)
{
  if(state == ENABLE)
	{
	  SIGNAL_RECEIVE_POWER_PORT->BSRR = SIGNAL_RECEIVE_POWER_PIN;//设置为1，状态为开启
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  SIGNAL_RECEIVE_POWER_PORT->BRR = SIGNAL_RECEIVE_POWER_PIN;//设置为0，状态为关闭
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  SIGNAL_RECEIVE_POWER_PORT->BRR = SIGNAL_RECEIVE_POWER_PIN;//设置为0，状态为关闭
	  return DISABLE;
	}

}


void signal_send_init(void)
{
  //TIM初始化结构体定义
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  //DMA初始化结构体定义
  DMA_InitTypeDef            DMA_InitStructure;
  //GPIO_InitTypeDef GPIO_InitStructure;

  //DAC信号产生
  //DMA时钟使能
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
  //GPIOA时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  //DAC时钟使能
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  //TIM2时钟使能
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  //TIM2定时器配置
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//定时器基本配置初始化
  TIM_TimeBaseStructure.TIM_Period = period;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//TIM2定时器配置初始化
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//TIM2定时器为输出触发

  //DAC1配置初始化
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;//设置DAC触发器为TIM2
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//不产生波形
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);//DAC1通道初始化
  
  //DMA2通道3关闭
  DMA_DeInit(DMA2_Channel3);
  //DMA配置
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_BASE + DHR12R1_OFFSET;//DAC1通道右对齐寄存器地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&sine12bit;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = sizeof(sine12bit) / sizeof(uint32_t);
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  //DMA2C4初始化
  DMA_Init(DMA2_Channel3, &DMA_InitStructure);

}

/*
 * Function signal_send ()
 *
 *    信号开始发送
 *
 */
FunctionalState signal_send(FunctionalState state)
{
  if(state == ENABLE)
	{
	  DMA_Cmd(DMA2_Channel3, ENABLE);
	  //DAC1使能
	  DAC_Cmd(DAC_Channel_1, ENABLE);
	  //DAC1DMA使能
	  DAC_DMACmd(DAC_Channel_1, ENABLE);
	  //TIM2使能
	  TIM_Cmd(TIM2, ENABLE);
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  DMA_Cmd(DMA2_Channel3, DISABLE);
	  //DAC1使能
	  DAC_Cmd(DAC_Channel_1, DISABLE);
	  //DAC1DMA使能
	  DAC_DMACmd(DAC_Channel_1, DISABLE);
	  //TIM2使能
	  TIM_Cmd(TIM2, DISABLE);
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  DMA_Cmd(DMA2_Channel3, DISABLE);
	  //DAC1使能
	  DAC_Cmd(DAC_Channel_1, DISABLE);
	  //DAC1DMA使能
	  DAC_DMACmd(DAC_Channel_1, DISABLE);
	  //TIM2使能
	  TIM_Cmd(TIM2, DISABLE);
	  return DISABLE;	  
	}
}

void signal_frequency_set(SignalFreq freq)
{
  if(freq < SIGNAL_FREQ_40 || freq > SIGNAL_FREQ_64000)
	{
	  freq = SIGNAL_FREQ_30000;
	}
  SystemCoreClockUpdate();
  period = SystemCoreClock / (signal_freq[freq] * POINT_PER_PERIOD);
}

void beep_alarm_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //使能发送电源引脚时钟
  RCC_APB2PeriphClockCmd(BEEP_ALARM_CLK, ENABLE);
  //配置发送电源引脚为输出
  GPIO_InitStructure.GPIO_Pin = BEEP_ALARM_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BEEP_ALARM_PORT, &GPIO_InitStructure);
  //BEEP_ALARM_PORT->BSRR = BEEP_ALARM_PIN;//默认为1,关闭状态
  BEEP_ALARM_PORT->BRR = BEEP_ALARM_PIN;//默认为1,关闭状态
}

void external_alarm_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //使能发送电源引脚时钟
  RCC_APB2PeriphClockCmd(EXTERNAL_ALARM_CLK, ENABLE);
  //配置发送电源引脚为输出
  GPIO_InitStructure.GPIO_Pin = EXTERNAL_ALARM_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(EXTERNAL_ALARM_PORT, &GPIO_InitStructure);
  EXTERNAL_ALARM_PORT->BSRR = EXTERNAL_ALARM_PIN;//默认为1,关闭状态
  
}

FunctionalState beep_alarm_set(FunctionalState state)
{
  if(state == ENABLE)
	{
	  BEEP_ALARM_PORT->BRR = BEEP_ALARM_PIN;//0,打开
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  BEEP_ALARM_PORT->BSRR = BEEP_ALARM_PIN;//默认为1,关闭状态
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  BEEP_ALARM_PORT->BSRR = BEEP_ALARM_PIN;//默认为1,关闭状态
	  return DISABLE;	  
	}
  
}

FunctionalState external_alarm_set(FunctionalState state)
{
  if(state == ENABLE)
	{
	  EXTERNAL_ALARM_PORT->BRR = EXTERNAL_ALARM_PIN;//0,打开
	  return ENABLE;
	}
  else if(state == DISABLE)
	{
	  EXTERNAL_ALARM_PORT->BSRR = EXTERNAL_ALARM_PIN;//默认为1,关闭状态
	  return DISABLE;	  
	}
  else
	{
	  //默认为关闭
	  EXTERNAL_ALARM_PORT->BSRR = EXTERNAL_ALARM_PIN;//默认为1,关闭状态
	  return DISABLE;	  
	}

}
