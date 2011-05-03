/*********************************************************************
 * Filename:      signal.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 22 11:38:50 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May  3 13:15:02 2011
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

//信号点发生周期
static uint16_t period = 2250;

//信号点数据
static uint32_t sine12bit[POINT_PER_PERIOD] = {

  2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
  3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
  599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647

};

//DAC初始化结构体定义
DAC_InitTypeDef            DAC_InitStructure;

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
  //DAC1使能
  DAC_Cmd(DAC_Channel_1, DISABLE);
  //DAC1DMA使能
  DAC_DMACmd(DAC_Channel_1, DISABLE);
  //TIM2使能
  TIM_Cmd(TIM2, DISABLE);
}

void signal_frequency_set(uint32_t freq)
{
  if(freq == 0)
	{
	  freq = 32000;
	}
  SystemCoreClockUpdate();
  period = SystemCoreClock / (freq * POINT_PER_PERIOD);
}

