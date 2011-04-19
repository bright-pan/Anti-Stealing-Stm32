/*********************************************************************
 * Filename:      led.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 19 17:09:06 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue Apr 19 17:17:37 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef LED_H
#define LED_H

typedef enum {
  LED_1,
  LED_2,
  LED_3,
  LED_4,
}LedName;

void led_init(LedName led_name);
void led_on(LedName led_name);
void led_off(LedName led_name);

#endif

