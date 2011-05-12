/*********************************************************************
 * Filename:      sms.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu May  5 13:25:11 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu May 12 15:49:46 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

uint16_t NUM_UCS_MAP[16] = {

  0X3000,  0X3100, 
  0X3200,  0X3300,
  0X3400,  0X3500,
  0X3600,  0X3700,
  0X3800,  0X3900,
  0X4100,  0X4200,
  0X4300,  0X4400,
  0X4500,  0X4600,
};

static uint8_t HEX_CHAR_MAP[16] = {

  '0','1','2','3',
  '4','5','6','7',
  '8','9','A','B',
  'C','D','E','F'
};

static uint8_t NUM_MAP[10] = {

  '\x00','\x01','\x02','\x03','\x04',
  '\x05','\x06','\x07','\x08','\x09'

};

static uint8_t ALPHA_MAP[7] = {

  '\x00','\x0a','\x0b','\x0c',
  '\x0d','\x0e','\x0f'

};


uint8_t *String_To_Semi_Octet(uint8_t *str_dest, uint8_t *str_src, uint8_t len)
{

  STRING_TO_INT *temp = (STRING_TO_INT *)str_src;
  register int16_t len_bk = len >> 1;
	
  for (; len_bk >= 0;len_bk--)
	{
	  str_dest[len_bk] = (uint8_t)((temp->STI[len_bk] & 0x0f00) >> 4) | ((temp->STI[len_bk] & 0x000f));
	}
  len >>= 1;
  str_dest[len++] |= 0xf0;
  str_dest[len] = '\0';
  return str_dest;
}

void String_To_Hex(uint8_t *str_dest, uint8_t *str_src, uint16_t len)
{

  len = len >> 1;

  for(; len > 0; --len)
	{
	  if(isdigit(*str_src))
		*str_dest = NUM_MAP[(*str_src++ & 0x0f)] << 4;
	  else
		*str_dest = ALPHA_MAP[(*str_src++ & 0x0f)] << 4;
	  if(isdigit(*str_src))
		*str_dest++ |= NUM_MAP[(*str_src++ & 0x0f)];
	  else
		*str_dest++ |= ALPHA_MAP[(*str_src++ & 0x0f)];
	}
  *str_dest = '\0';
}

uint8_t UCS_Len(uint16_t *UCS, uint16_t end_sign)
{

  uint8_t length = 0;
	
  while(*UCS++ != end_sign)
	length++;
	
  return length;

}

uint8_t UCS_Len_Restraints(uint16_t *UCS, uint16_t end_sign, uint8_t max_len)
{

  uint8_t length = 0;
	
  while(*UCS++ != end_sign)
	{
	  if(++length >= max_len)
		break;
	}
	
  return length;

}

uint8_t *UCS_To_String(uint16_t *UCS, uint8_t * str, uint8_t UCS_len)
{

  uint8_t *str_dest = str;
  uint8_t *str_src = (uint8_t *)UCS;

  while(UCS_len--)
	{
	  *str_dest++ = *++str_src;

	  str_src++;
	}
  *str_dest++ = '#';
  *str_dest = '\0';
  return str;
	
}

uint16_t *UCS_Char(uint16_t *UCS, uint16_t ucs_char, uint8_t UCS_len)
{

  while(UCS_len--)
	{

	  if(*UCS++ == ucs_char)
		return UCS;
		
	}
  return NULL;
}

void TP_Str_To_Octet(uint8_t *TP_octet, uint8_t *TP_str, uint8_t TP_type, uint8_t TP_len)
{
  uint8_t length = ((TP_len & 0x01) ? TP_len + 1 : TP_len) >> 1;
  uint8_t *TP_octet_bk = TP_octet;
  if(TP_type == INTERNATIONAL_ADDRESS_TYPE)
	{
	  *TP_octet++ = 0X68;//国际地址开头为86;
	  while(length--)
		{
		  *TP_octet = *(TP_str + 1) << 4;
		  *TP_octet++ |= *TP_str++ & 0x0f;
		  TP_str++;
		}
	  if(TP_len & 0x01)
		{
		  TP_len++;
		  TP_len >>= 1;
		  TP_octet_bk[TP_len] |= 0xf0;

		}
	}
  else
	{
	  while(length--)
		{
		  *TP_octet = *(TP_str + 1) << 4;
		  *TP_octet++ |= *TP_str++ & 0x0f;
		  TP_str++;
		}
	  if(TP_len & 0x01)
		{
		  TP_len++;
		  TP_len >>= 1;
		  TP_octet_bk[TP_len] |= 0xf0;

		}
	}	
}

uint16_t *SMS_Send_User_Data_Copy(uint16_t *UCS, uint8_t *data_src, uint16_t *UCS_Len, uint16_t data_max_len)
{
  uint8_t *data_dest = (uint8_t *)UCS;
  data_max_len >>= 1;
  while(data_max_len--)
	{
	  if(*data_src == 0 && *(data_src+ 1) == 0)
		break;
	  *data_dest++ = *data_src++;
	  *data_dest++ = *data_src++;
	  (*UCS_Len) ++;
	}
  return (uint16_t *)data_dest;
}

void Alarm_Mail_Data_To_UCS(uint16_t *UCS, SMS_ALARM_FRAME *sms_alarm_mail, uint16_t *UCS_Len)
{

  /* 邮件时间解析格式为20XX-XX-XX,XX:XX:XX*/
  *UCS++ = YU;//于
  *UCS_Len += 1;
	
  *UCS++ = 0X3200;// 2;
  *UCS++ = 0X3000;// 0;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_year & 0xf0) << 4) | 0x3000;//解析年的高四为年的十位;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_year & 0x0f) << 8) | 0x3000;//解析年的高四为年的个位;
  *UCS++ = YEAR;// 年;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_mon & 0xf0) << 4) | 0x3000;//解析年的高四为月的十位;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_mon & 0x0f) << 8) | 0x3000;//解析年的高四为月的个位;
  *UCS++ = MONTH;// 月;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_mday & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_mday & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
  *UCS++ = DAY;// 日;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_hour & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_hour & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
  *UCS++ = HOUR;// 时;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_min & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_min & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
  *UCS++ = MINUTE;// 分;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_sec & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
  *UCS++ = ((uint16_t )(sms_alarm_mail->time.tm_sec & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
  *UCS++ = SECOND;// 秒;
  *UCS_Len += 20;
  *UCS++ = FAN;
  *UCS++ = SHENG;
  *UCS_Len += 2;
  /* 线缆通断状态解析{接通,断开} */
  if(sms_alarm_mail->cable_fault_type == CABLE_DISCONNECT_FAULT)
	{
	  /* 断开 */
	  *UCS++ = UCS2_DUAN;
	  *UCS++ = UCS2_KAI;
	}
  else
	{
	  /* 连通 */
	  *UCS++ = UCS2_LIAN;
	  *UCS++ = UCS2_TONG;
	}
  *UCS_Len += 2;

  *UCS++ = SHI;
  *UCS++ = JIAN;
  *UCS_Len += 2;

  *UCS++ = COMMA_SIGN;
  *UCS++ = QING;
  *UCS++ = JIN;
  *UCS++ = KUAI;
  *UCS++ = CHU;
  *UCS++ = LI;
  *UCS_Len += 6;

  *UCS++ = POUND_SIGN;
  *UCS_Len += 1;
  /*
  // 线缆带电状况{有电,无电} 
  if(sms_alarm_mail->current_on_off == CURRENT_ON)
  {
  // 有电
  *UCS++ = UCS2_YOU;
  *UCS++ = UCS2_DIAN;
  }
  else
  {
  //无电 
  *UCS++ = UCS2_WU;
  *UCS++ = UCS2_DIAN;
  }
  *UCS++ = POUND_SIGN;
  *UCS_Len += 3;	

  //  线缆温度解析 XXX度/XX度
  switch(sms_alarm_mail->temperature & 0xf800)
  {
  case 0x0000 : {
  sms_alarm_mail->temperature &=  0x07FF;
  sms_alarm_mail->temperature >>= 4;
  if(sms_alarm_mail->temperature <= 9)
  {
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature];
  *UCS++ = UCS2_DU;
  *UCS++ = POUND_SIGN;
  *UCS_Len += 3;				

  }
  else if(sms_alarm_mail->temperature <= 99)
  {
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature / 10];
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature % 10];
  *UCS++ = UCS2_DU;
  *UCS++ = POUND_SIGN;
  *UCS_Len += 4;				

  }else if(sms_alarm_mail->temperature <= 999)
  {
				
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature / 100];//百位;
  sms_alarm_mail->temperature %= 100;
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature / 10];
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature % 10];
  *UCS++ = UCS2_DU;
  *UCS++ = POUND_SIGN;
  *UCS_Len += 5;
  }
			
  break;
  }

  case 0xf800 : {

  sms_alarm_mail->temperature &=  0x07FF;
  sms_alarm_mail->temperature >>= 4;
  if(sms_alarm_mail->temperature <= 9)
  {
  *UCS++ = LINE_SIGN;
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature];
  *UCS++ = UCS2_DU;
  *UCS++ = POUND_SIGN;
  *UCS_Len += 4;				

  }
  else if(sms_alarm_mail->temperature <= 99)
  {
  *UCS++ = LINE_SIGN;
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature / 10];
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature % 10];
  *UCS++ = UCS2_DU;
  *UCS++ = POUND_SIGN;
  *UCS_Len += 5;				

  }else if(sms_alarm_mail->temperature <= 999)
  {
  *UCS++ = LINE_SIGN;
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature / 100];//百位;
  sms_alarm_mail->temperature %= 100;
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature / 10];
  *UCS++ = NUM_UCS_MAP[sms_alarm_mail->temperature % 10];
  *UCS++ = UCS2_DU;
  *UCS++ = POUND_SIGN;
  *UCS_Len += 6;
  }
  break;
  }
  case 0xA000 : {

  break;
  }
  case 0xB000 : {

  break;
  }
  default : {

  break;
  }
  }
  */
}

void Send_Hex_Char_To_GSM(uint8_t *Hex_char, uint16_t Hex_len, uint16_t off_set)
{
  //  uint8_t err;
	
  Hex_char += off_set;
	
  while(Hex_len--)
	{
	  USART_SendData(GSM_USART3, HEX_CHAR_MAP[*Hex_char >> 4]);
	  while(USART_GetFlagStatus(GSM_USART3, USART_FLAG_TXE) == RESET)
		{
		}
	  USART_SendData(GSM_USART3, HEX_CHAR_MAP[*Hex_char++ & 0x0f]);
	  while(USART_GetFlagStatus(GSM_USART3, USART_FLAG_TXE) == RESET)
		{
		}
	}
}

void Send_PDU_To_GSM(SMS_SEND_PDU_FRAME *sms_pdu, SMS_HEAD_6 *sms_head)
{

  /* SMSC头的发送
   *  如果长度为0，只需要发送长度那个字节;
   *  否则则发送全部SMSC头;
   */
  if(sms_pdu->SMSC.SMSC_Length)
	{
	  Send_Hex_Char_To_GSM(&(sms_pdu->SMSC.SMSC_Length), 1, 0);
	  Send_Hex_Char_To_GSM(&(sms_pdu->SMSC.SMSC_Type_Of_Address), sms_pdu->SMSC.SMSC_Length, 0);
	}
  else
	{
	  Send_Hex_Char_To_GSM(&(sms_pdu->SMSC.SMSC_Length), 1, 0);
	}

  /*
   * 	发送TPDU 头;
   */
  Send_Hex_Char_To_GSM(&(sms_pdu->TPDU.First_Octet), sizeof(sms_pdu->TPDU) - sizeof(sms_pdu->TPDU.TP_UD), 0);

  /* 
   * 	发送短信数据;
   */
 	
  if(sms_head->sms_numbers > 1)
	{
	  Send_Hex_Char_To_GSM((uint8_t *)sms_head, sizeof(SMS_HEAD_6), 0);
	  Send_Hex_Char_To_GSM(sms_pdu->TPDU.TP_UD, sms_pdu->TPDU.TP_UDL - sizeof(SMS_HEAD_6), (sms_head->sms_index - 1)*(140 - sizeof(SMS_HEAD_6)));
	}
  else
	{
	  Send_Hex_Char_To_GSM(sms_pdu->TPDU.TP_UD, sms_pdu->TPDU.TP_UDL, 0);
	
	}

  /*
   *	发送结束字符;
   */
  send_to_gsm("\x1A", 1);
}


