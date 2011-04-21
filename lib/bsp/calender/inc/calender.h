/*********************************************************************
 * Filename:      calender.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Wed Apr 20 16:28:21 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Apr 21 13:23:24 2011
 *                
 * Description:   calander for system time.
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __CALENDER_H__
#define __CALENDER_H__

#include <time.h>

extern struct tm calender;

void calender_init(void);
void calender_set(struct tm *tm_time);
struct tm *calender_get(struct tm *tm_time);

#endif

