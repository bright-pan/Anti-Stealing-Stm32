/*********************************************************************
 * Filename:      led.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 19 17:19:59 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Fri May 13 17:20:55 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define LEDn                             5
#define LED1_PIN                         GPIO_Pin_0
#define LED1_GPIO_PORT                   GPIOD
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOD
  
#define LED2_PIN                         GPIO_Pin_1
#define LED2_GPIO_PORT                   GPIOD
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOD

#define LED3_PIN                         GPIO_Pin_2  
#define LED3_GPIO_PORT                   GPIOD
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOD 

#define LED4_PIN                         GPIO_Pin_3 
#define LED4_GPIO_PORT                   GPIOD
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOD

#define LED5_PIN                         GPIO_Pin_4
#define LED5_GPIO_PORT                   GPIOD
#define LED5_GPIO_CLK                    RCC_APB2Periph_GPIOD


GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT,
                                 LED4_GPIO_PORT, LED5_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN,
                                 LED4_PIN, LED5_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK,
                                 LED4_GPIO_CLK, LED5_GPIO_CLK};



/*
 * Function led_init (led_name)
 *
 *    led initializing for led_name
 *
 */
void led_init(LED_TypeDef led_name)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(GPIO_CLK[led_name], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[led_name];
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_PORT[led_name], &GPIO_InitStructure);

}

/*
 * Function led_on (led_name)
 *
 *    turn on for led_name
 *
 */
void led_on(LED_TypeDef led_name)
{
  GPIO_PORT[led_name]->BSRR = GPIO_PIN[led_name];
}

/*
 * Function led_off (led_name)
 *
 *    trun off led for led_name
 *
 */
void led_off(LED_TypeDef led_name)
{
  GPIO_PORT[led_name]->BRR = GPIO_PIN[led_name];   
}


/*
 * Function led_toggle (led_name)
 *
 *    toggle status for led_name
 *
 */
void led_toggle(LED_TypeDef led_name)
{
  GPIO_PORT[led_name]->ODR ^= GPIO_PIN[led_name];
}

