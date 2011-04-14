/*********************************************************************
 * Filename:      init.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu Apr 14 10:15:43 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Apr 14 15:38:00 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

static void systick_config(void);
static void interrupt_config(void);

void init(void)
{
  SystemInit();
}

static void systick_config(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;

    RCC_GetClocksFreq(&rcc_clocks);
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;
    SysTick_Config(cnts);
}

static void interrupt_config(void)
{
 // NVIC_InitTypeDef NVIC_InitStructure;
  //EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  //EXTI_InitTypeDef EXTI_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
 // NVIC_SystemHandlerPriorityConfig(SystemHandler_PSV,3,3);

   /* Enable the EXTI4 Interrupt */
   /*
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); */

   //触摸屏的中断输入为PE4
   /* Enable the EXTI Line4 Interrupt */
   /*
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);  */
}
