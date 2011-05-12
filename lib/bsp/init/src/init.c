/*********************************************************************
 * Filename:      init.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu Apr 14 10:15:43 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Mon May  9 09:17:44 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

int16_t temperature = 0;

static void systick_config(void);
static void interrupt_config(void);
static void gpio_config(void);
static void led_config(void);
static void calender_config(void);
static void signal_config(void);
static void sflash_config(void);
static void temperature_config(void);
static void gsm_config(void);

/*
 * Function bsp_init ()
 *
 *    芯片初始化
 *
 *    1. 时钟频率配置
 *    2. IO端口配置
 *    3. 中断配置
 *    4. led配置
 *    5. 日历配置
 *    6. 信号配置
 *    7. 系统定时器配置
 *
 */
void bsp_init(void)
{
  SystemInit();
  gpio_config();
  interrupt_config();
  led_config();
  calender_config();
  signal_config();
  sflash_config();
  temperature_config();
  gsm_config();
  systick_config();
}

/*
 * Function systick_config ()
 *
 *    系统定时器配置
 *
 */
//时钟配置结构体变量

static void systick_config(void)
{
  RCC_ClocksTypeDef rcc_clocks;
  uint32_t cnts = 0;

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
//定义中断初始化结构

static void interrupt_config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  //EXTI_InitTypeDef EXTI_InitStructure;

  //中断优先级分组为从第0位开始分组，
  //这就意味着本系统最多支持128个抢占优先级，
  //每个抢占优先级中包含有2个子优先级。
  //但是由于STM32为4位中断优先级实现，
  //则最多有16个中断优先级,
  //子优先级在这里没有作用。
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  //设置系统定时器中断优先级
  //抢占优先级为0
  //子优先级为0
  //不屏蔽这个中断
  NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  // NVIC_SystemHandlerPriorityConfig(SystemHandler_PSV,3,3);
  //设置USART3中断优先级
  NVIC_InitStructure.NVIC_IRQChannel = GSM_USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
//IO端口配置结构体变量

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
  led_init(LED_5);
}

/*
 * Function calender_config ()
 *
 *    日历功能初始化
 *
 */
static void calender_config(void)
{
  calender_init();
}

/*
 * Function signal_config ()
 *
 *    信号功能初始化
 *
 */
static void signal_config(void)
{
  signal_frequency_set(SIGNAL_FREQ);
  signal_init();
  signal_send();
}

/*
 * Function sflash_config ()
 *
 *    sflash 功能初始化
 *
 */
static void sflash_config(void)
{
  sFLASH_Init();
  sFLASH_PageSizeSet();
}

static void temperature_config(void)
{
  if(TP_convert())
	{
	  temperature = (int16_t)TP_read();
	}
  else
	{
	  //温度模块故障
	}
}
#define GSM_VIO_PIN                      GPIO_Pin_6
#define GSM_VIO_PORT                     GPIOE
#define GSM_VIO_CLK                      RCC_APB2Periph_GPIOE

static void gsm_config(void)
{
  gsm_init();
  gsm_power(ENABLE);
  gsm_setup(ENABLE);
}

