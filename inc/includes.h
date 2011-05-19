/*********************************************************************
 * Filename:      includes.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu Apr 21 09:58:30 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu May 19 16:48:13 2011
 *                
 * Description:   all include file
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#define   DEF_FALSE 0
#define   DEF_TRUE  1
//系统头文件
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
//ucos内核头文件
#include "ucos_ii.h"
//STM32标准库头文件
#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "core_cm3.h"
//用户自定义头文件
#include "stm32f10x_it.h"
#include "init.h"
#include "led.h"
#include "calender.h"
#include "signal.h"
#include "sflash.h"
#include "temperature.h"
#include "gsm.h"
#include "sms.h"
#include "app_cfg.h"

#endif

