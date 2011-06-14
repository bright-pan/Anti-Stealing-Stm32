/*********************************************************************
 * Filename:      led.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 19 17:09:06 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue Jun 14 15:51:38 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __LED_H__
#define __LED_H__

typedef enum {
  LED_RUN = 0,
  LED_RS485 = 1,
  LED_GSM = 2,
  LED_SIGNAL_STATE = 3,
  LED_SFLASH = 4,
}LED_TypeDef;

void led_init(LED_TypeDef led_name);
void led_on(LED_TypeDef led_name);
void led_off(LED_TypeDef led_name);
void led_toggle(LED_TypeDef led_name);

#endif

