/*********************************************************************
 * Filename:      temperature.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue May  3 09:38:09 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Wed May  4 15:01:43 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/


#include <includes.h>

#define DELAY_US_TICK 72

#define DS18B20_BUS_PIN 			GPIO_Pin_4                 // PE.04
#define DS18B20_BUS_PORT			GPIOE                       // GPIOE
#define DS18B20_BUS_CLK				RCC_APB2Periph_GPIOE

#define DS18B20_BUS_OUTPUT() {\
  DS18B20_BUS_PORT->CRL = (DS18B20_BUS_PORT->CRL & 0xfff0ffff) | 0x00030000;\
}
#define DS18B20_BUS_INPUT() {\
  DS18B20_BUS_PORT->CRL = (DS18B20_BUS_PORT->CRL & 0xfff0ffff) | 0x00040000;\
}

#define DS18B20_BUS_UP() {						\
	DS18B20_BUS_PORT->BSRR = DS18B20_BUS_PIN;	\
  }
#define DS18B20_BUS_DOWN() {					\
	DS18B20_BUS_PORT->BRR = DS18B20_BUS_PIN;	\
  }
#define DS18B20_BUS_READ() (DS18B20_BUS_PORT->IDR & DS18B20_BUS_PIN)

//static GPIO_InitTypeDef GPIO_InitStructure;

void delay_us(uint32_t time)
{
  uint8_t nCount;
  while(time--)
	{
	  for(nCount = 6 ; nCount != 0; nCount--);
	}
}

uint8_t DS18B20_reset(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(DS18B20_BUS_CLK, ENABLE);
    
  DS18B20_BUS_OUTPUT();
  //DS18B20_BUS_DIR = OUTPUT;
  DS18B20_BUS_UP();
  //DS18B20_BUS_DO = 1;
  delay_us(2);
  //asm(" REPEAT #80 \n NOP");// 2US
  DS18B20_BUS_DOWN();
  //DS18B20_BUS_DO = 0;
  delay_us(500);
  // asm(" REPEAT #4000 \n NOP");//100US;
  //asm(" REPEAT #4000 \n NOP");//100US;
  //asm(" REPEAT #4000 \n NOP");//100US;
  //asm(" REPEAT #4000 \n NOP");//100US;
  //asm(" REPEAT #4000 \n NOP");//100US;
  DS18B20_BUS_UP();
  //DS18B20_BUS_DO = 1;
  delay_us(2);
  DS18B20_BUS_INPUT();
  //asm(" REPEAT #100 \n NOP");// 2.5US
  //DS18B20_BUS_DIR = INPUT;
  if(DS18B20_BUS_READ() == DS18B20_BUS_PIN)
	{
	  delay_us(58);
	  //asm(" REPEAT #2300 \n NOP");//57.5US;
	  //(DS18B20_BUS_DI == 1) ? (return 0): (return 1);//没有设备则返回错误代码-1;
	  if(DS18B20_BUS_READ() == DS18B20_BUS_PIN)
		{
		  return 0;
		}
	  else
		{
		  delay_us(300);
		  //asm(" REPEAT #12000 \n NOP");//300US;
		  if(DS18B20_BUS_READ() == DS18B20_BUS_PIN)
			return 1;
		  else
			return 0;
		}

	}
  else
	{
	  return 0;
	}
}

void DS18B20_write(uint8_t command)
{
  uint8_t command_index = 0;
  DS18B20_BUS_OUTPUT();
  // DS18B20_BUS_DIR = OUTPUT;
  DS18B20_BUS_UP();
  //DS18B20_BUS_DO = 1;
  delay_us(2);
  //asm(" REPEAT #80 \n NOP");// 2US;
  for(command_index = 0; command_index < 8; command_index++)
	{
	  if(command & 0x01)
		{
		  //写1;
		  DS18B20_BUS_DOWN();
		  //DS18B20_BUS_DO = 0;
		  delay_us(2);
		  //asm(" REPEAT #80 \n NOP");// 2US;
		  DS18B20_BUS_UP();
		  //DS18B20_BUS_DO = 1;
		  delay_us(58);
		  //asm(" REPEAT #2320 \n NOP");// 58US;
			
		}
	  else
		{
		  //写0;
		  DS18B20_BUS_DOWN();
		  //DS18B20_BUS_DO = 0;
		  delay_us(60);
		  //asm(" REPEAT #2400 \n NOP");//60US;
		  DS18B20_BUS_UP();
		  //DS18B20_BUS_DO = 1;
		  delay_us(2);
		  //asm(" REPEAT #80 \n NOP");// 2US;
		}
	  command >>= 1;
	}
	
}

uint8_t DS18B20_read(void)
{
  uint8_t command_index = 0;
  uint8_t data = 0;

  for(command_index = 0; command_index < 8; command_index++)
	{
	  DS18B20_BUS_OUTPUT();
	  DS18B20_BUS_UP();
	  //DS18B20_BUS_DIR = OUTPUT;
	  //DS18B20_BUS_DO = 1;
	  delay_us(2);
	  //asm(" REPEAT #80 \n NOP");// 2US;
	  DS18B20_BUS_DOWN();
	  //DS18B20_BUS_DO = 0;
	  delay_us(2);
	  //asm(" REPEAT #80 \n NOP");// 2US;
	  DS18B20_BUS_INPUT();
	  //DS18B20_BUS_DIR = INPUT;
	  delay_us(8);
	  //asm(" REPEAT #320 \n NOP");// 8US;

	  data >>= 1;
	  if(DS18B20_BUS_READ() == DS18B20_BUS_PIN)
		{
		  //读1;
		  data |= 0x80;
			
		}
	  delay_us(50);
	  //asm(" REPEAT #2000 \n NOP");// 50US;
	}
  return data;
}

uint8_t TP_convert(void)
{
  if(DS18B20_reset())
	{
	  DS18B20_write(0xcc);
	  DS18B20_write(0x44);
	  return 1;
	}
  else
	{
	  return 0;
	}
}

uint16_t TP_read(void)
{
  uint16_t data = 0;
  if(DS18B20_reset())
	{
	  DS18B20_write(0xcc);
	  DS18B20_write(0xbe);
	  data = DS18B20_read();
	  data |= (uint16_t)DS18B20_read() << 8;  
	  return data;
	}
  else
	{
	  return data;
	}	
}
