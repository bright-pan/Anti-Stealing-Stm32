/*********************************************************************
 * Filename:      app_cfg.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu May 12 16:13:28 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Jun  9 09:37:52 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

//#include "signal.h"

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define  LIB_STR_CFG_FP_EN                DEF_DISABLED
//#define  uC_PROBE_OS_PLUGIN               DEF_ENABLED
//#define  uC_PROBE_COM_MODULE              DEF_ENABLED


/*
*********************************************************************************************************
*                                       TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                    		0 /* Lower numbers are of higher priority                 */
#define  MUTEX_GSM_PIP					  		1
#define  MUTEX_RS485_PIP				  		2		
#define  MUTEX_SFLASH_PIP							3
#define  MUTEX_CALENDER_PIP 		4

//#define  APP_TASK_LCD_PRIO                      6
#define  APP_TASK_RS485_PRIO                     8                       /* RS485 发送任务 */
#define  APP_TASK_SIGNAL_PRIO                     5                       /* RS485 发送任务 */
#define  APP_TASK_GSM_PRIO                       9				/* GR64 发送任务 */
#define  APP_TASK_SMSSend_PRIO			 10				/* SMS 发送任务 */
#define  APP_TASK_SMSReceive_PRIO		 7				/* SMS 接收任务 */




#define  OS_TASK_TMR_PRIO                      20


/*
*********************************************************************************************************
*                                       TASK STACK SIZES
*
* Notes :   1) Warming, setting a stack size too small may result in the OS crashing. It the OS crashes
*              within a deep nested function call, the stack size may be to blame. The current maximum
*              stack usage for each task may be checked by using uC/OS-View or the stack checking
*              features of uC/OS-II.
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE              256
//#define  APP_TASK_LCD_STK_SIZE                256
//#define  OS_PROBE_TASK_STK_SIZE               160                       /* See probe_com_cfg for RS-232 commication task stack size */
#define  APP_TASK_GSM_STK_SIZE		256
#define APP_TASK_SMSSend_STK_SIZE		256
#define APP_TASK_SMSReceive_STK_SIZE	256
#define APP_TASK_SIGNAL_STK_SIZE		256
#define APP_TASK_RS485_STK_SIZE		256



/*
*********************************************************************************************************
*                                                CONSTANTS
*********************************************************************************************************
*/
#define 	Q_SMS_ALARM_ARRAY_SIZE			20//短信告警邮件队列大小;
#define 	MEM_PART_NUMBER					30//内存分区数量;
#define 	MEM_PART_SIZE						16//内存分区大小;


#define 	SLAVE_DEVICE_MAX_NUMBERS		10//从设备数量;
#define 	DEVICE_NAME_MAX_LENGTH			32//设备名称长度;
#define 	ALARM_TELEPHONE_MAX_NUMBERS	10 //告警手机最大数量;
#define 	ALARM_TELEPHONE_NUMBER_SIZE 	12//手机号码长度(SEMI_OCTET格式);
#define   DEVICE_PASSWORD_MAX_LENGTH		12
#define   GPS_MAX_LENGTH					32

/* PDU构造 */
#define	INTERNATIONAL_ADDRESS_TYPE		0x91
#define	LOCAL_ADDRESS_TYPE				0xA1


#define  	SMSC_DEFAULT 						0x08
#define 	FIRST_OCTET_DEFAULT				0x11
#define	TP_MR_DEFAULT						0x00
#define	TP_TYPE_DEFAULT					INTERNATIONAL_ADDRESS_TYPE//国际地址;
#define	TP_PID_DEFAULT					0x00//普通GSM 协议,点对点方式;
#define	TP_DCS_DEFAULT					0X08//UCS2编码方式;
#define	TP_VP_DEFAULT						0XC2//5分钟有效期限;

/*
*********************************************************************************************************
*                                          					DATA TYPES
*********************************************************************************************************
*/

typedef struct {
  uint16_t interval;//信号间隔时间，单位为秒
  uint16_t freq;//信号频率
  uint16_t freq_spread;//信号频率差值
  uint16_t amp_limit;//信号幅度限值
  uint16_t process_counts;//信号处理次数
  uint16_t process_interval;//信号每次处理之后的间隔，单位为毫秒
  uint16_t send_freq;//发送信号频率
  uint16_t receive_freq;//接收信号频率
  uint16_t amp;//信号幅值
  uint16_t state;//通断状态
}SignalParameters;

typedef struct {
  uint8_t device_name[DEVICE_NAME_MAX_LENGTH];//主设备名称;
  uint16_t device_id;
  uint8_t primary_device_name[DEVICE_NAME_MAX_LENGTH];//主设备名称;
  //uint8_t	slave_device_numbers;//从设备数量,且必须小于SLAVE_DEVICE_MAX_NUMBERS;
  //uint8_t slave_device_id[SLAVE_DEVICE_MAX_NUMBERS];//从设备ID数组,大小为SLAVE_DEVICE_MAX_NUMBERS;
  uint8_t slave_device_name[DEVICE_NAME_MAX_LENGTH];
  //  SMS_ALARM_FRAME slave_device_history_alarm[SLAVE_DEVICE_MAX_NUMBERS];
  uint16_t alarm_telephone_numbers;
  uint8_t alarm_telephone[ALARM_TELEPHONE_MAX_NUMBERS][ALARM_TELEPHONE_NUMBER_SIZE + 1];
  uint8_t service_center_address[ALARM_TELEPHONE_NUMBER_SIZE + 1];
  uint8_t password[DEVICE_PASSWORD_MAX_LENGTH];
  uint8_t gps[GPS_MAX_LENGTH];//主设备名称;
  uint16_t sms_on_off;
  uint16_t rs485_baudrate;
  uint16_t gsm_signal_strength;
  SignalParameters signal_parameters;
  uint16_t temperature;//温度
  struct tm calender;
  }DEVICE_INIT_PARAMATERS;


/*
  typedef struct {
  uint8_t device_id;
  uint8_t function_code;
  uint8_t length;
  uint8_t cable_fault_type;
  struct tm time;
  uint8_t current_on_off;
  uint16_t temperature;
  uint16_t current_intensity;
  uint16_t current_frequency;
  uint16_t crc;
  }SLAVE_DEVICE_STATE_FRAME;
*/

#define INT16_REVERSER(a) ((a) >> 8 | (a) << 8)

#endif
