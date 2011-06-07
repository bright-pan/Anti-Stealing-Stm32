/*********************************************************************
 * Filename:      sms.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu May  5 13:34:39 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue Jun  7 10:10:43 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __SMS_H__
#define __SMS_H__
#include "includes.h"

/* 电缆故障类型 */
#define CABLE_DISCONNECT_FAULT	0x01
#define CURRENT_ON				0x01

/* 数据帧正确 */
#define FRAME_CORRECT 			0x00

/* 短信帧功能分隔标记 */
#define POUND_SIGN 	0x2300//#井号
#define COMMA_SIGN  0x2c00
#define COLON_SIGN	0x3a00// :;
#define LINE_SIGN	0x2D00// :;

/* 汉字UCS2 编码 */

#define UCS2_LIAN	0XDE8F//连
#define UCS2_TONG	0X1A90//通
#define UCS2_DUAN	0XAD65//断
#define UCS2_KAI	0X005F//开
#define UCS2_YOU	0X0967//有
#define UCS2_DIAN	0X3575//电
#define UCS2_WU		0XE065//无
#define UCS2_DU 	0XA65E//度

#define YEAR		0X745e
#define MONTH		0X0867
#define DAY			0Xe565
#define HOUR		0XF665
#define MINUTE		0X0652
#define SECOND		0XD279

#define FAN			0xd153
#define SHENG	 	0x1f75//发生
#define SHI			0x8b4e
#define JIAN		0xf64e//事件

//请尽快处理;
#define QING		0xf78b
#define	JIN			0x3d5c
#define	KUAI		0xeb5f
#define	CHU			0x0459
#define	LI			0x0674

#define	YU			0x8e4e//于

#define SMS_ALARM_FRAME_TYPE		1
#define SMS_QUERY_FRAME_TYPE		2

#define MODBUS_DATA_QUERY 		0x03
#define MODBUS_TIME_SET			0x10



typedef struct {
	uint16_t STI[8];
}STRING_TO_INT;

typedef struct {

	uint8_t SMSC_Length;//计算方式不同;
	uint8_t SMSC_Type_Of_Address;
	uint8_t SMSC_Address_Value[7];
	
}SMSC_TYPE;

typedef struct {

	uint8_t TP_OA_Length;//计算方式不同;
	uint8_t TP_OA_Type_Of_Address;
	uint8_t TP_OA_Address_Value[7];
	
}TP_OA_TYPE;
typedef struct {

	uint8_t TP_DA_Length;//计算方式不同;
	uint8_t TP_DA_Type_Of_Address;
	uint8_t TP_DA_Address_Value[7];
	
}TP_DA_TYPE;

typedef struct {
	
	uint8_t TP_SCTS_Year;//BCD;
	uint8_t TP_SCTS_Month;
	uint8_t TP_SCTS_Day;
	uint8_t TP_SCTS_Hour;
	uint8_t TP_SCTS_minute;
	uint8_t TP_SCTS_Second;
	uint8_t TP_SCTS_Time_Zone;
	
}TP_SCTS_TYPE;

typedef struct{
	uint8_t sms_head_length;
	uint8_t sms_laber_length;
	uint8_t sms_head_surplus_length;
	uint8_t sms_laber;
	uint8_t sms_numbers;
	uint8_t sms_index;
}SMS_HEAD_6;



typedef struct {

  uint8_t First_Octet;
  TP_OA_TYPE TP_OA;//9字节;
  uint8_t TP_PID;
  uint8_t TP_DCS;
  TP_SCTS_TYPE TP_SCTS;//7字节;
  uint8_t TP_UDL;//用户长度必须小于140 个字节;
  uint8_t TP_UD[140];
}SMS_RECEIVE_TPDU_TYPE;





typedef struct {
	
	uint8_t 	First_Octet;
	uint8_t		TP_MR;
	TP_DA_TYPE		TP_DA;//9字节;
	uint8_t 	TP_PID;
	uint8_t 	TP_DCS;
	uint8_t		TP_VP;// 1个字节;
	uint8_t		TP_UDL;//用户长度必须小于420 个字节;
	uint8_t		TP_UD[420];
	
}SMS_SEND_TPDU_TYPE;

typedef struct {
	
	SMSC_TYPE SMSC;
	SMS_RECEIVE_TPDU_TYPE TPDU;
	
}SMS_RECEIVE_PDU_FRAME;

typedef struct {
	
	SMSC_TYPE SMSC;
	SMS_SEND_TPDU_TYPE TPDU;
	
}SMS_SEND_PDU_FRAME;



typedef struct {

	uint16_t ZXSOFT[6];
	uint16_t SIGN_1;
	uint16_t SET[2];
	uint16_t SIGN_2;
	uint16_t PASSWORD[6];
	uint16_t SIGN_3;
	uint16_t FUCTION_CODE[2];
	uint16_t SIGN_4;
	uint16_t DATA[50];
	
}SMS_SET_FRAME;

typedef struct {
	
	uint16_t ZXSOFT[6];
	uint16_t SIGN_1;
	uint16_t LIST[2];
	uint16_t SIGN_2;	
	uint16_t FUCTION_CODE[2];
	uint16_t SIGN_3;
	uint16_t DATA[50];
	
}SMS_LIST_FRAME;
/*
typedef struct{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t week;
	uint8_t year;
}TIME_FRAME;
*/

typedef struct {
 
	uint8_t device_id;
	uint8_t function_code;
	uint16_t address;
	uint16_t length;
	uint16_t crc;
	
}MODBUS_DATA_QUERY_FRAME;
typedef struct {
	int16_t temperature;
	uint8_t state;
	struct tm time;
}SMS_ALARM_FRAME;
typedef struct {
	TP_OA_TYPE		TP_OA;//9字节;
	uint8_t function_code;
}SMS_QUERY_FRAME;


typedef struct {
	uint16_t sms_mail_type;
	union {
		SMS_ALARM_FRAME sms_alarm_frame;
		SMS_QUERY_FRAME sms_query_frame;
	};
}SMS_MAIL_FRAME;

extern uint16_t NUM_UCS_MAP[16];

void String_To_Hex(uint8_t *str_dest, uint8_t *str_src, uint16_t len);
uint8_t *String_To_Semi_Octet(uint8_t *str_dest, uint8_t *str_src, uint8_t len);
uint8_t UCS_Len(uint16_t *UCS, uint16_t end_sign);
uint8_t *UCS_To_String(uint16_t *UCS, uint8_t * str, uint8_t UCS_len);
uint16_t *UCS_Char(uint16_t *UCS, uint16_t ucs_char, uint8_t UCS_len);
void TP_Str_To_Octet(uint8_t *TP_octet, uint8_t *TP_str, uint8_t TP_type, uint8_t TP_len);

void Alarm_Mail_Data_To_UCS(uint16_t *UCS, SMS_ALARM_FRAME *sms_alarm_mail, uint16_t *UCS_Len);
uint16_t *SMS_Send_User_Data_Copy(uint16_t *UCS, uint8_t *data_src, uint16_t *UCS_Len, uint16_t data_max_len);

void Send_PDU_To_GSM(SMS_SEND_PDU_FRAME *sms_pdu, SMS_HEAD_6 *sms_head);

#endif

