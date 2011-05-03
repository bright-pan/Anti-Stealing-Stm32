/*********************************************************************
 * Filename:      main.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 12 17:07:41 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue May  3 14:00:03 2011
 *                
 * Description:   application main program.
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define STACK_SIZE 1024
#define BOOT_RAM 0xF1E0F85F
#define NVIC_CCR ((volatile unsigned long *)(0xE000ED14))
typedef void (* const pfn_ISR)(void);

static  void  AppTaskCreate(void);

static  void  AppTaskStart (void *p_arg);
uint8_t data[1000] = {0,};
// 声明函数原型
int main(void);

// VARIABLES
__attribute__ ((section(".stackarea"))) static unsigned long Stack[STACK_SIZE];
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

int  main (void)
{
  //  SystemInit();
    INT8U  err;
	/* Set the Vector Table base location at 0x08000000 */ 
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

    //BSP_IntDisAll();                   /* Disable all interrupts until we are ready to accept them */

    OSInit();                          /* Initialize "uC/OS-II, The Real-Time Kernel"              */

   /* Create the start task */
	err = OSTaskCreateExt(AppTaskStart,(void *)0,
					(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1],
					APP_TASK_START_PRIO,APP_TASK_START_PRIO,
					(OS_STK *)&AppTaskStartStk[0],
					APP_TASK_START_STK_SIZE,(void *)0,
					OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR
					);

#if (OS_TASK_NAME_SIZE > 13)
    OSTaskNameSet(APP_TASK_START_PRIO, (INT8U *)"Start Task", &err);
#endif

//	OSTimeSet(0);
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
	return 0;
}
static  void  AppTaskCreate(void)
{
  
}

static  void  AppTaskStart (void *p_arg)
{
  (void)p_arg;

      bsp_init();                                                 /* Initialize BSP functions                                 */

   #if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
   #endif

//   AppUserIFMbox = OSMboxCreate((void *)0);                  /* Create MBOX for communication between Kbd and UserIF     */
   AppTaskCreate();                                            /* Create application tasks                                 */
    
   while(DEF_TRUE)
	 {  
	  /* Task body, always written as an infinite loop. */
	   //  OSTaskSuspend(OS_PRIO_SELF);
	   /*
		 GPIO_InitTypeDef GPIO_InitStructure;
  
		 //Enable the GPIO_LED Clock
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

		 //Configure the GPIO_LED pin
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		 GPIO_Init(GPIOC, &GPIO_InitStructure);
	   OSTimeDlyHMSM(0,0,0,200);

	   GPIOC->BSRR = GPIO_Pin_2;
	   OSTimeDlyHMSM(0,0,0,200);
	   GPIOC->BRR = GPIO_Pin_2;
	   */
	   OSTimeDlyHMSM(0,0,0,200);
	   led_toggle(LED_1);
	   OSTimeDlyHMSM(0,0,0,200);
	   led_toggle(LED_2);
	   OSTimeDlyHMSM(0,0,0,200);
	   led_toggle(LED_3);
	   OSTimeDlyHMSM(0,0,0,200);
	   led_toggle(LED_4);
	   OSTimeDlyHMSM(0,0,0,200);
	   led_toggle(LED_5);
	   OSTimeDlyHMSM(0,0,0,200);
	   calender_get(&calender);
	   OSTimeDlyHMSM(0,0,0,200);
	   /*
	   uint16_t index = 0;
	   uint16_t length = sizeof(data)/sizeof(uint8_t);
	   
	   for (index = 0; index <=length; index++)
		 {
		   *(data + index) = index;
		 }
	   sFLASH_WriteBuffer(data, 0, length);
	   for (index = 0; index <=length; index++)
		 {
		   *(data + index) = 0;
		 }
	   sFLASH_ReadBuffer(data, 0, length);
	   __NOP();
	   __NOP();
	   __NOP();
	   */
	   uint16_t temp = 0;
	   if(TP_convert())
		 {
		   __NOP();
		   __NOP();
		   temp=TP_read();
		 }
	   // GPIO_SetBits(GPIOC,GPIO_Pin_6);
	 //OSTimeDlyHMSM(0,0,0,200);
	// GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	  /* ExTick=TPReadY();
	   GUI_DispDecAt(ExTick,20,40,4);
	  */
	  
     }
}


// mthomas: added section -> alignment thru linker-script 

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
