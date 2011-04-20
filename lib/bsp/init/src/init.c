/*********************************************************************
 * Filename:      init.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu Apr 14 10:15:43 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed Apr 20 11:26:31 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

static void systick_config(void);
static void interrupt_config(void);
static void gpio_config(void);
static void led_config(void);


/*
 * Function bsp_init ()
 *
 *    芯片初始化
 *
 *    1. 时钟频率配置
 *    2. IO端口配置
 *    3. 中断配置
 *    4. led配置
 *    5. 系统定时器配置
 *
 */
void bsp_init(void)
{
  SystemInit();
  gpio_config();
  interrupt_config();
  led_config();
  systick_config();
}

/*
 * Function systick_config ()
 *
 *    系统定时器配置
 *
 */
static void systick_config(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;

	RCC_GetClocksFreq(&rcc_clocks);
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;
    SysTick_Config(cnts);
}

/*
 * Function interrupt_config ()
 *
 *    中断初始化配置
 *
 */
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


/*
 * Function gpio_config ()
 *
 *    将IO端口都设置成模拟输入，以降低功耗以及增强电磁兼容
 *
 */
static void gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, DISABLE);  
}

/*
 * Function led_config ()
 *
 *    LED灯配置
 *
 */
static void led_config(void)
{
  led_init(LED_1);
  led_init(LED_2);
  led_init(LED_3);
  led_init(LED_4);
}
