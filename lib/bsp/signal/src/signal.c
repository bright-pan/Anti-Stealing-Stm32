/*********************************************************************
 * Filename:      signal.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 22 11:38:50 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May 17 17:18:52 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

/* DHR registers offsets */
#define DHR12R1_OFFSET             ((uint32_t)0x00000008)
#define DHR12R2_OFFSET             ((uint32_t)0x00000014)
#define DHR12RD_OFFSET             ((uint32_t)0x00000020)

#define POINT_PER_PERIOD 32

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

//信号点发生周期
static uint16_t period;

//信号点数据
static uint32_t sine12bit[POINT_PER_PERIOD] = {

  2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
  3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
  599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647

};

const static uint32_t signal_freq[] = {
  4000,
  8000,
  12000,
  16000,
  20000,
  24000,
  28000,
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

//DAC初始化结构体定义
DAC_InitTypeDef            DAC_InitStructure;
TIM_ICInitTypeDef  TIM_ICInitStructure;

/*
 * Function signal_init ()
 *
 *    信号发生初始化
 *    1. DAC1初始化
 *    2. DMA2C3初始化
 *    3. TIM2初始化
 *
 */
void signal_init(void)
{
  //变量定义
  //DMA初始化结构体定义
  DMA_InitTypeDef            DMA_InitStructure;
  //TIM初始化结构体定义
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  GPIO_InitTypeDef GPIO_InitStructure;
  
  //使能发送电源引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_SEND_POWER_CLK, ENABLE);
  //配置发生电源引脚为输出
  GPIO_InitStructure.GPIO_Pin = SIGNAL_SEND_POWER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_SEND_POWER_PORT, &GPIO_InitStructure);
  SIGNAL_SEND_POWER_PORT->BRR = SIGNAL_SEND_POWER_PIN;//默认为0,关闭状态
  //使能接收电源引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_RECEIVE_POWER_CLK, ENABLE);
  //配置发生电源引脚为输出
  GPIO_InitStructure.GPIO_Pin = SIGNAL_RECEIVE_POWER_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_RECEIVE_POWER_PORT, &GPIO_InitStructure);
  SIGNAL_RECEIVE_POWER_PORT->BRR = SIGNAL_RECEIVE_POWER_PIN;//默认为关闭状态

  //发送频率检测引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_SEND_FREQ_CLK, ENABLE);
  //设置成模拟输入
  GPIO_InitStructure.GPIO_Pin =  SIGNAL_SEND_FREQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_SEND_FREQ_PORT, &GPIO_InitStructure);
  //发送输入捕获配置
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM4, &TIM_ICInitStructure);
  // Enable the CC1 Interrupt Request
  TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
  // TIM enable counter
  TIM_Cmd(TIM4, ENABLE);


  //接收频率检测引脚时钟
  RCC_APB2PeriphClockCmd(SIGNAL_RECEIVE_FREQ_CLK, ENABLE);
  //设置成模拟输入
  GPIO_InitStructure.GPIO_Pin =  SIGNAL_RECEIVE_FREQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SIGNAL_RECEIVE_FREQ_PORT, &GPIO_InitStructure);
  //接收输入捕获配置
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);
  // Enable the CC2 Interrupt Request
  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
  // TIM enable counter
  TIM_Cmd(TIM3, ENABLE);

  
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


/*
 * Function signal_send ()
 *
 *    信号开始发送
 *
 */
void signal_send(void)
{
  DMA_Cmd(DMA2_Channel3, ENABLE);
  //DAC1使能
  DAC_Cmd(DAC_Channel_1, ENABLE);
  //DAC1DMA使能
  DAC_DMACmd(DAC_Channel_1, ENABLE);
  //TIM2使能
  TIM_Cmd(TIM2, ENABLE);

}

void signal_receive(void)
{
  
}

/*
 * Function signal_off ()
 *
 *    信号关闭
 *
 */
void signal_off(void)
{
  DMA_Cmd(DMA2_Channel3, DISABLE);
  //DAC1关闭
  DAC_Cmd(DAC_Channel_1, DISABLE);
  //DAC1DMA关闭
  DAC_DMACmd(DAC_Channel_1, DISABLE);
  //TIM2关闭
  TIM_Cmd(TIM2, DISABLE);
}

void signal_frequency_set(SignalFreq freq)
{
  if(freq < SIGNAL_FREQ_4000 || freq > SIGNAL_FREQ_64000)
	{
	  freq = SIGNAL_FREQ_32000;
	}
  SystemCoreClockUpdate();
  period = SystemCoreClock / (signal_freq[freq] * POINT_PER_PERIOD);
}

