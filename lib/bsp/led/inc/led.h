/*********************************************************************
 * Filename:      led.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 19 17:09:06 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed Apr 20 10:34:06 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef LED_H
#define LED_H

typedef enum {
  LED_1 = 0,
  LED_2 = 1,
  LED_3 = 2,
  LED_4 = 3,
}LED_TypeDef;

void led_init(LED_TypeDef led_name);
void led_on(LED_TypeDef led_name);
void led_off(LED_TypeDef led_name);
void led_toggle(LED_TypeDef led_name);

#endif

