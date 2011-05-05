/*********************************************************************
 * Filename:      main.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 12 17:07:41 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu May  5 17:57:19 2011
 *                
 * Description:   application main program.
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define STACK_SIZE 1024
#define BOOT_RAM 0xF1E0F85F
#define NVIC_CCR ((volatile uint32_t *)(0xE000ED14))
typedef void (* const pfn_ISR)(void);
__attribute__ ((section(".stackarea"))) static uint32_t Stack[STACK_SIZE];
/* 任务栈定义区 */
static OS_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];
static OS_STK  AppGR64TaskStk[APP_TASK_GR64_STK_SIZE];
static OS_STK  AppRS485TaskStk[APP_TASK_RS485_STK_SIZE];
static OS_STK  AppSMSSendTaskStk[APP_TASK_SMSSend_STK_SIZE];
static OS_STK  AppSMSReceiveTaskStk[APP_TASK_SMSReceive_STK_SIZE];
/* 设备运行参数 */
OS_EVENT *MUTEX_DEVICE_INIT_PARAMATERS;
DEVICE_INIT_PARAMATERS device_init_paramaters;
//MODBUS_RECEIVE_FRAME slave_device_state_frame[SLAVE_DEVICE_MAX_NUMBERS];
//TIME_FRAME set_time;
const DEVICE_INIT_PARAMATERS device_init_paramaters_const = {
  "zxsoft:anti-stealing2011",
	"高新区一号开闭所",
	1,
	{0x01, },
	{"比亚迪出线电缆",},
	{{0,},},
	1,
	{"13474656377",},
	"",
	"\x00\x31\x00\x32\x00\x33\x00\x34\x00\x35\x00\x36",
	"",
	1,
};
/* 设备使用分配信号量 */
OS_EVENT *MUTEX_GR64;//设备使用互斥信号量;
OS_EVENT *MUTEX_RS485;//设备使用互斥信号量;
OS_EVENT *MUTEX_SFLASH;

/* 用于发送中断完成指示 */
OS_EVENT *SEM_RS485_SEND_BYTE;//发送一个字节已完成信号量;
OS_EVENT *SEM_GR64_SEND_BYTE;//发送一个字节已完成信号量;

/* 用于内存分配 */
OS_EVENT *SEM_MEM_PART_ALLOC;//内存分配信号量;
OS_MEM	  *MEM_BUF;//内存缓冲区;
uint8_t MEM_PART[MEM_PART_NUMBER][MEM_PART_SIZE];//内存缓冲区MEM_PART_NUMBER 个分区,各个分区大小为MEM_PART_SIZE Bytes;

/* SMS发送告警发送邮件队列 */
OS_EVENT *Q_SMS_ALARM;
void *Q_SMS_ALARM_ARRAY[Q_SMS_ALARM_ARRAY_SIZE];//消息队列数组大小为10;

/* SMS 消息指示信号量 */
OS_EVENT *SEM_SMS_MSG_INDICATOR;


/* 帧处理内存区*/
uint8_t GR64_SEND_BUF[256];
uint8_t GR64_RECEIVE_BUF[256];

uint8_t SMS_RECEIVE_PROCESS_BUF[256];
uint8_t SMS_SEND_PROCESS_BUF[512];
uint8_t SMS_UCS_PROCESS_BUF[DEVICE_NAME_MAX_LENGTH];

uint8_t RS485_SEND_BUF[32];
uint8_t RS485_RECEIVE_BUF[32];

OS_EVENT *SEM_SMS_OK;
OS_EVENT *SEM_SMS_FAULT;

/* 铁电存储器使用信号量 */


/* 用于GR64 的AT指令 */
const uint8_t *AT = (const uint8_t *)"AT\r";
const uint8_t *ATE0 = (const uint8_t *)"ATE0\r";
const uint8_t *ATIPR = (const uint8_t *)"AT+IPR=9600\r";
const uint8_t *ATCSQ = (const uint8_t *)"AT+CSQ\r";
const uint8_t *ATCMGF = (const uint8_t *)"AT+CMGF=0\r";
//int8_t *ATCNMI     = "AT+CNMI=3,1\r";
const uint8_t *ATCMGL = (const uint8_t *)"AT+CMGL=4\r";
const uint8_t *ATCNMI = (const uint8_t *)"AT+CNMI=1,1,0,2,1\r";
const uint8_t *ATCSCA = (const uint8_t *)"AT+CSCA?\r";
const uint8_t *ATCCLK = (const uint8_t *)"AT+CCLK?\r";
const uint8_t *ATOK = (const uint8_t *)"OK\r\n";
const uint8_t *ATCPMS = (const uint8_t *)"AT+CPMS=\"ME\",\"ME\",\"ME\"\r";
const uint8_t *ATSMSRI = (const uint8_t *)"AT*E2SMSRI=50\r";
const uint8_t *ATE2IPA = (const uint8_t *)"AT*E2IPA=1,1\r";
const uint8_t *ATE2IPA0 = (const uint8_t *)"AT*E2IPA=0,1\r";
const uint8_t *ATCT = (const uint8_t *)"CONNECT\r\n";
const uint8_t *ATO = (const uint8_t *)"ATO\r";
const uint8_t *ATE2IPC = (const uint8_t *)"AT*E2IPC\r";
const uint8_t *ATASKCGDCONT = (const uint8_t *)"AT+CGDCONT?\r";
const uint8_t *ATE2IPS  = (const uint8_t *)"AT*E2IPS=2,8,2,1020,1\r";
const uint8_t *ATGETIP  = (const uint8_t *)"AT*E2IPI=0\r";
const uint8_t *ATCIMI  = (const uint8_t *)"AT+CIMI\r";
const uint8_t *ZXSOFT = (const uint8_t *)"\x00\x7a\x00\x78\x00\x73\x00\x6f\x00\x66\x00\x74";//ZXSOFT帧头匹配;
const uint8_t *SMS_SET = (const uint8_t *)"\x8b\xbe\x7f\x6e";//设置命令匹配参数;

const uint8_t *SET_PRIMARY_DEVICE_NAME = (const uint8_t *)"\x00\x30\x00\x31\x00\x30";//010;设置主设备名称;
const uint8_t *SET_SLAVE_DEVICE_NUMBERS = (const uint8_t *)"\x00\x30\x00\x32\x00\x30";//020;设置从设备数;
const uint8_t *SET_SLAVE_DEVICE_ID = (const uint8_t *)"\x00\x30\x00\x32\x00\x31";//021;//设置从设备通讯ID;
const uint8_t *SET_SLAVE_DEVICE_NAME = (const uint8_t *)"\x00\x30\x00\x32\x00\x32";//022;//设置从设备名称;
const uint8_t *SET_ALARM_TELEPHONE_NUMBERS = (const uint8_t *)"\x00\x30\x00\x33\x00\x30";//030;设置告警电话数量;
const uint8_t *SET_ALARM_TELEPHONE = (const uint8_t *)"\x00\x30\x00\x33\x00\x31";//031;设置告警电话号码;

const uint8_t *SMS_LIST = (const uint8_t *)"\x67\xe5\x8b\xe2";//查询命令;
const uint8_t *SUPER_PASSWORD = (const uint8_t *)"\x00\x36\x00\x35\x00\x34\x00\x33\x00\x32\x00\x31";

static  void  AppStartTask(void *p_arg);
//static  void  AppLCDTask(void *p_arg);
static  void  AppTaskCreate(void);
//static void AppGR64Task(void *p_arg);
//static void AppRS485Task(void *p_arg);
//static void AppSMSSendTask(void *p_arg);
//static void AppSMSReceiveTask(void *p_arg);

static uint8_t byte2BCD(uint8_t bVal)
{
  return  ( (bVal/10)<<4) + (bVal%10);
}

uint8_t data[1000] = {0,};

int  main (void)
{
  //  SystemInit();
    INT8U  err;
	/* Set the Vector Table base location at 0x08000000 */ 
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

    //BSP_IntDisAll();                   /* Disable all interrupts until we are ready to accept them */

    OSInit();                          /* Initialize "uC/OS-II, The Real-Time Kernel"              */

   /* Create the start task */
	err = OSTaskCreateExt(AppStartTask,(void *)0,
					(OS_STK *)&AppStartTaskStk[APP_TASK_START_STK_SIZE-1],
					APP_TASK_START_PRIO,APP_TASK_START_PRIO,
					(OS_STK *)&AppStartTaskStk[0],
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

static  void  AppStartTask (void *p_arg)
{
  (void)p_arg;
  uint8_t err;
      bsp_init();                                                 /* Initialize BSP functions                                 */

   #if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
   #endif

//   AppUserIFMbox = OSMboxCreate((void *)0);                  /* Create MBOX for communication between Kbd and UserIF     */

SEM_RS485_SEND_BYTE = OSSemCreate(0);
SEM_GR64_SEND_BYTE = OSSemCreate(0);

MUTEX_GR64 = OSMutexCreate(MUTEX_GR64_PIP, &err);
MUTEX_RS485 = OSMutexCreate(MUTEX_RS485_PIP, &err);
MUTEX_SFLASH = OSMutexCreate(MUTEX_SFLASH_PIP, &err);

/* 创建MEM_PART_NUMBER 个内存分区且分区大小为MEM_PART_SIZE 供邮箱队列使用 */
SEM_MEM_PART_ALLOC = OSSemCreate(MEM_PART_NUMBER);
MEM_BUF = OSMemCreate(MEM_PART, MEM_PART_NUMBER, MEM_PART_SIZE, &err);

/* 创建邮件列表且列表大小为Q_SMS_ALARM_ARRAY_SIZE 个消息 */
Q_SMS_ALARM = OSQCreate(Q_SMS_ALARM_ARRAY, Q_SMS_ALARM_ARRAY_SIZE);

/* 创建短消息状态信号量 */
SEM_SMS_OK = OSSemCreate(0);
SEM_SMS_FAULT = OSSemCreate(0);

MUTEX_DEVICE_INIT_PARAMATERS = OSMutexCreate(MUTEX_DEVICE_INIT_PARAMETERS_PIP, &err);

SEM_SMS_MSG_INDICATOR = OSSemCreate(0);

   AppTaskCreate();                                            /* Create application tasks                                 */
/* 全局变量初始化 */
	OSMutexPend(MUTEX_SFLASH, 0, &err);

	sFLASH_ReadBuffer((uint8_t *)&device_init_paramaters, \
				SFLASH_DEVICE_INIT_PARAMATERS_START, \
				sizeof(DEVICE_INIT_PARAMATERS));
//		device_init_paramaters = device_init_paramaters_const;
//		SFLASH_write(SFLASH_DEVICE_INIT_PARAMATERS_START, (CPU_INT08U *)&device_init_paramaters, sizeof(DEVICE_INIT_PARAMATERS));
	if(memcpy(device_init_paramaters.device_name, device_init_paramaters_const.device_name, DEVICE_NAME_MAX_LENGTH))
	{
		device_init_paramaters = device_init_paramaters_const;
		sFLASH_WriteBuffer((uint8_t *)&device_init_paramaters, \
					 SFLASH_DEVICE_INIT_PARAMATERS_START, \
					 sizeof(DEVICE_INIT_PARAMATERS));
	}
	OSMutexPost(MUTEX_SFLASH);
    
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
	   
	   uint16_t index = 0;
	   uint16_t length = sizeof(data)/sizeof(uint8_t);
	   
	   for (index = 0; index <=length; index++)
		 {
		   *(data + index) = 0XAA;
		 }
	   sFLASH_WriteBuffer(data, 0, length);
	   for (index = 0; index <=length; index++)
		 {
		   *(data + index) = 0;
		 }
	   sFLASH_ReadBuffer(data, 0, length);
	   __NOP();
	   for (index = 0; index <=length; index++)
		 {
		   *(data + index) = 0;
		 }
	   sFLASH_WriteBuffer(data, 550, 550);
	   for (index = 0; index <=length; index++)
		 {
		   *(data + index) = 0xaa;
		 }

	   sFLASH_ReadBuffer(data, 550,  550);
	   __NOP();
	   __NOP();
	   
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
