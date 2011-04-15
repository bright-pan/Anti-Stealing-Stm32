/*********************************************************************
 * Filename:      main.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 12 17:07:41 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Fri Apr 15 11:54:59 2011
 *                
 * Description:   application main program.
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define STACK_SIZE 1024
#define BOOT_RAM 0xF1E0F85F
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
// mthomas: added section -> alignment thru linker-script 
__attribute__ ((section(".stackarea"))) 
static unsigned long Stack[STACK_SIZE];

__attribute__ ((section(".isr_vector")))
pfn_ISR VectorArray[] = {
  (pfn_ISR)((unsigned long)Stack + sizeof(Stack)),	// The initial stack pointer,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,
  BusFault_Handler,
  UsageFault_Handler,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  SVC_Handler,
  DebugMon_Handler,
  (pfn_ISR)0,
  OSPendSV_Handler,
  SysTick_Handler,
  WWDG_IRQHandler,
  PVD_IRQHandler,
  TAMPER_IRQHandler,
  RTC_IRQHandler,
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_IRQHandler,
  EXTI1_IRQHandler,
  EXTI2_IRQHandler,
  EXTI3_IRQHandler,
  EXTI4_IRQHandler,
  DMA1_Channel1_IRQHandler,
  DMA1_Channel2_IRQHandler,
  DMA1_Channel3_IRQHandler,
  DMA1_Channel4_IRQHandler,
  DMA1_Channel5_IRQHandler,
  DMA1_Channel6_IRQHandler,
  DMA1_Channel7_IRQHandler,
  ADC1_2_IRQHandler,
  USB_HP_CAN1_TX_IRQHandler,
  USB_LP_CAN1_RX0_IRQHandler,
  CAN1_RX1_IRQHandler,
  CAN1_SCE_IRQHandler,
  EXTI9_5_IRQHandler,
  TIM1_BRK_IRQHandler,
  TIM1_UP_IRQHandler,
  TIM1_TRG_COM_IRQHandler,
  TIM1_CC_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  TIM4_IRQHandler,
  I2C1_EV_IRQHandler,
  I2C1_ER_IRQHandler,
  I2C2_EV_IRQHandler,
  I2C2_ER_IRQHandler,
  SPI1_IRQHandler,
  SPI2_IRQHandler,
  USART1_IRQHandler,
  USART2_IRQHandler,
  USART3_IRQHandler,
  EXTI15_10_IRQHandler,
  RTCAlarm_IRQHandler,
  USBWakeUp_IRQHandler,
  TIM8_BRK_IRQHandler,
  TIM8_UP_IRQHandler,
  TIM8_TRG_COM_IRQHandler,
  TIM8_CC_IRQHandler,
  ADC3_IRQHandler,
  FSMC_IRQHandler,
  SDIO_IRQHandler,
  TIM5_IRQHandler,
  SPI3_IRQHandler,
  UART4_IRQHandler,
  UART5_IRQHandler,
  TIM6_IRQHandler,
  TIM7_IRQHandler,
  DMA2_Channel1_IRQHandler,
  DMA2_Channel2_IRQHandler,
  DMA2_Channel3_IRQHandler,
  DMA2_Channel4_5_IRQHandler,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)0,
  (pfn_ISR)BOOT_RAM,
};
