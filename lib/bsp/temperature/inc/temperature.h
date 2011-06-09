/*********************************************************************
 * Filename:      temperature.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May  3 09:39:08 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed Jun  8 09:58:40 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

//extern int16_t temperature;

#define TP_CONVERT_VALUE 0.0625

int16_t TP_read(void);
uint8_t TP_convert(void);
void delay_us(uint32_t time);

#endif
