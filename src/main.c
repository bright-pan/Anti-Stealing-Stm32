/*********************************************************************
 * Filename:      main.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 12 17:07:41 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Apr 14 15:13:12 2011
 *                
 * Description:   application main program.
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define STACK_TOP 0x20000800
#define NVIC_CCR ((volatile unsigned long *)(0xE000ED14))

// 声明函数原型
int main(void);
void myDelay(unsigned long delay );
void Clk_Init (void);


// VARIABLES

GPIO_InitTypeDef GPIO_InitStructure;


/*
 * Function main ()
 *
 *    application start entry
 *
 */
int main(void)
{
	
  *NVIC_CCR = *NVIC_CCR | 0x200; /* 设置NVIC 的STKALIGN */
	// Init clock system
  SystemInit();

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
					

	        // Configure PC.12 as output push-pull (LED)
	        GPIO_WriteBit(GPIOC,GPIO_Pin_12,Bit_SET);
	        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	        GPIO_Init(GPIOC, &GPIO_InitStructure);

	    

	  while(1)
		{
		  
	            GPIOC->BRR |= 0x00001000;
	            myDelay(500000);
	            GPIOC->BSRR |= 0x00001000;
	            myDelay(500000);
	      
	      }
}
//Functions definitions
void myDelay(unsigned long delay )
{ 
  while(delay) delay--;
}

/*
 * Function Clk_Init ()
 *
 *    System clock initialize.
 *
 */
void Clk_Init (void)
{
  // 1. Cloking the controller from internal HSI RC (8 MHz)
  RCC_HSICmd(ENABLE);
  // wait until the HSI is ready
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  // 2. Enable ext. high frequency OSC
  RCC_HSEConfig(RCC_HSE_ON);
  // wait until the HSE is ready
  while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
  // 3. Init PLL
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); // 72MHz
//  RCC_PLLConfig(RCC_PLLSource_HSE_Div2,RCC_PLLMul_9); // 72MHz
  RCC_PLLCmd(ENABLE);
  // wait until the PLL is ready
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  // 4. Set system clock divders
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  // Flash 1 wait state 
  *(vu32 *)0x40022000 = 0x12;
  // 5. Clock system from PLL
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

typedef void (* const pfn_ISR)(void);

__attribute__ ((section(".isr_vector"))) pfn_ISR VectorArray[] ={
  (pfn_ISR)STACK_TOP,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
};
