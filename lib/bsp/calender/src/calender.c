/*********************************************************************
 * Filename:      calender.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 19 17:19:59 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed Apr 27 11:48:23 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

//定义系统日历时间变量
struct tm calender;

static void RTC_Configuration(void);


/*
 * Function calender_init ()
 *
 *    日历初始化
 *    1. 设置RTC参数为一秒递增
 *    2. 使用备份寄存器BKP_DR1
 *       初始化后为0xA5A5
 *       
 *
 */
void calender_init(void)
{
  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
	// RTC Configuration
	RTC_Configuration();
	//设置初始化时间为2011年1月1日1时1分1秒
	calender.tm_sec = 1;
	calender.tm_min = 1;
	calender.tm_hour = 1;
	calender.tm_mday = 1;
	calender.tm_mon = 1;
	calender.tm_year = 2011;
	calender.tm_wday = 0;
	calender.tm_yday = 0;
	calender.tm_isdst = 0;
	calender_set(&calender);
	// Enable PWR and BKP clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	// Allow access to BKP Domain
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
}

/*
 * Function calender_set (tm_time)
 *
 *    日历时钟设置
 *
 */
void calender_set(struct tm *tm_time)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  RTC_WaitForLastTask();
  
  tm_time->tm_year -= 1900;
  tm_time->tm_mon -= 1;
  
  RTC_SetCounter(mktime(tm_time));
  
  RTC_WaitForLastTask();

  PWR_BackupAccessCmd(DISABLE);

}
/*
 * Function calender_get (tm_time)
 *
 *    读取日历时钟
 *
 */
struct tm *calender_get(struct tm *tm_time)
{
  time_t clock = 0;
  
  RTC_WaitForLastTask();
  
  clock = (time_t)RTC_GetCounter();
  gmtime_r(&clock, tm_time);
  
  tm_time->tm_year += 1900;
  tm_time->tm_mon += 1;
  
  RTC_WaitForLastTask();
  
  return tm_time;
}

/*
 * Function RTC_Configuration ()
 *
 *    RTC时钟模块初始化
 *
 */
static void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  //  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  //RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
