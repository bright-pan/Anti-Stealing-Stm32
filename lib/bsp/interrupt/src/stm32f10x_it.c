/*********************************************************************
 * Filename:      stm32f10x_it.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Fri Apr 15 09:23:55 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Fri May 20 16:24:04 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"
// 定义向量表

__IO uint16_t signal_send_freq_capture = 0;
__IO uint16_t signal_receive_freq_capture = 0;



extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern int main(void);

void Reset_Handler(void)
{
  unsigned long *src, *dst;
  //
  // Copy the data segment initializers from flash to SRAM.
  //
  src = &_etext;
  dst = &_data;
  while (dst < &_edata)
	{
	  *dst++ = *src++;
	}
  //
  // Zero fill the bss segment.
  //
  for(dst = &_bss; dst < &_ebss; dst++)
	{
	  *dst = 0;
	}
  //
  // Call the application's entry point.
  //
  main();
}


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
	{
	}
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
	{
	}
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
	{
	}
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
	{
	}
}

void SVC_Handler(void) {

}

void DebugMon_Handler(void) {

}
/*
  void PendSV_Handler(void) {

  }
*/
void SysTick_Handler(void) {
  //  TimingDelay_Decrement();
  OS_CPU_SR  cpu_sr;

   
  OS_ENTER_CRITICAL();  /* Tell uC/OS-II that we are starting an ISR*/
  OSIntNesting++;
  OS_EXIT_CRITICAL();	  
   
  OSTimeTick();     /* Call uC/OS-II's OSTimeTick() */
   
  OSIntExit();
}

void WWDG_IRQHandler(void){}
void PVD_IRQHandler(void){}
void TAMPER_IRQHandler(void){}
void RTC_IRQHandler(void){}
void FLASH_IRQHandler(void){}
void RCC_IRQHandler(void){}
void EXTI0_IRQHandler(void){}
void EXTI1_IRQHandler(void){}
void EXTI2_IRQHandler(void){}
void EXTI3_IRQHandler(void){}
void EXTI4_IRQHandler(void){}
void DMA1_Channel1_IRQHandler(void){}
void DMA1_Channel2_IRQHandler(void){}
void DMA1_Channel3_IRQHandler(void){}
void DMA1_Channel4_IRQHandler(void){}
void DMA1_Channel5_IRQHandler(void){}
void DMA1_Channel6_IRQHandler(void){}
void DMA1_Channel7_IRQHandler(void){}
void ADC1_2_IRQHandler(void){}
void USB_HP_CAN1_TX_IRQHandler(void){}
void USB_LP_CAN1_RX0_IRQHandler(void){}
void CAN1_RX1_IRQHandler(void){}
void CAN1_SCE_IRQHandler(void){}
void EXTI9_5_IRQHandler(void){}
void TIM1_BRK_IRQHandler(void){}
void TIM1_UP_IRQHandler(void){}
void TIM1_TRG_COM_IRQHandler(void){}
void TIM1_CC_IRQHandler(void){}
void TIM2_IRQHandler(void){}

void TIM3_IRQHandler(void)
{ 
  static __IO uint8_t capture_number = 0;
  static __IO uint16_t read_value1 = 0;
  static __IO uint16_t read_value2 = 0;

  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET)
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    if(capture_number == 0)
    {
      /* Get the Input Capture value */
      read_value1 = TIM_GetCapture2(TIM3);
      capture_number = 1;
    }
    else if(capture_number == 1)
    {
      /* Get the Input Capture value */
      read_value2 = TIM_GetCapture2(TIM3); 
      
      /* Capture computation */
      if (read_value2 > read_value1)
      {
        signal_receive_freq_capture = (read_value2 - read_value1); 
      }
      else
      {
        signal_receive_freq_capture = ((0xFFFF - read_value1) + read_value2); 
      }
      capture_number = 0;
    }
  }
}

void TIM4_IRQHandler(void)
{ 
  static __IO uint8_t capture_number = 0;
  static __IO uint16_t read_value1 = 0;
  static __IO uint16_t read_value2 = 0;

  if(TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET)
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
    if(capture_number == 0)
    {
      /* Get the Input Capture value */
      read_value1 = TIM_GetCapture1(TIM4);
      capture_number = 1;
    }
    else if(capture_number == 1)
    {
      /* Get the Input Capture value */
      read_value2 = TIM_GetCapture1(TIM4); 
      
      /* Capture computation */
      if (read_value2 > read_value1)
      {
        signal_send_freq_capture = (read_value2 - read_value1); 
      }
      else
      {
        signal_send_freq_capture = ((0xFFFF - read_value1) + read_value2); 
      }
      capture_number = 0;
    }
  }
}

void I2C1_EV_IRQHandler(void){}
void I2C1_ER_IRQHandler(void){}
void I2C2_EV_IRQHandler(void){}
void I2C2_ER_IRQHandler(void){}
void SPI1_IRQHandler(void){}
void SPI2_IRQHandler(void){}
void USART1_IRQHandler(void){}
void USART2_IRQHandler(void){}

void USART3_IRQHandler(void){
  //  TimingDelay_Decrement();
  OS_CPU_SR  cpu_sr;
  
   
  OS_ENTER_CRITICAL();  /* Tell uC/OS-II that we are starting an ISR*/
  OSIntNesting++;
  OS_EXIT_CRITICAL();	  

  uint8_t temp; 
  if(USART_GetITStatus(GSM_USART3, USART_IT_ORE) != RESET)
	{
	  /* Read one byte from the receive data register */
	  temp = USART_ReceiveData(GSM_USART3);
	  if(FULL(gsm_buf))
		{
		  //缓冲区已满，丢弃数据
		}
	  else
		{
		  PUTCH(temp, gsm_buf);//缓冲区不是满的则存入缓冲区
		}
	  //记录溢出标识
	}
  if(USART_GetITStatus(GSM_USART3, USART_IT_RXNE) != RESET)
	{
	  /* Read one byte from the receive data register */
	  temp = USART_ReceiveData(GSM_USART3);
	  if(FULL(gsm_buf))
		{
		  //缓冲区已满，丢弃数据
		}
	  else
		{
		  PUTCH(temp, gsm_buf);//缓冲区不是满的则存入缓冲区
		}
	}
  
  OSIntExit();
}
void EXTI15_10_IRQHandler(void){}
void RTCAlarm_IRQHandler(void){}
void USBWakeUp_IRQHandler(void){}
void TIM8_BRK_IRQHandler(void){}
void TIM8_UP_IRQHandler(void){}
void TIM8_TRG_COM_IRQHandler(void){}
void TIM8_CC_IRQHandler(void){}
void ADC3_IRQHandler(void){}
void FSMC_IRQHandler(void){}
void SDIO_IRQHandler(void){}
void TIM5_IRQHandler(void){}
void SPI3_IRQHandler(void){}
void UART4_IRQHandler(void){}
void UART5_IRQHandler(void){}
void TIM6_IRQHandler(void){}
void TIM7_IRQHandler(void){}
void DMA2_Channel1_IRQHandler(void){}
void DMA2_Channel2_IRQHandler(void){}
void DMA2_Channel3_IRQHandler(void){}
void DMA2_Channel4_5_IRQHandler(void){}

