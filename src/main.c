/*********************************************************************
 * Filename:      main.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Tue Apr 12 17:07:41 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu Jun  9 11:39:40 2011
 *                
 * Description:   application main program
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define STACK_SIZE 2048
#define BOOT_RAM 0xF1E0F85F
#define NVIC_CCR ((volatile unsigned long *)(0xE000ED14))
typedef void (* const pfn_ISR)(void);
__attribute__ ((section(".stackarea"))) static uint32_t Stack[STACK_SIZE];
// 任务栈定义区
__attribute__((aligned(8))) static OS_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];
__attribute__((aligned(8))) static OS_STK  AppGSMTaskStk[APP_TASK_GSM_STK_SIZE];
__attribute__((aligned(8))) static OS_STK  AppSMSSendTaskStk[APP_TASK_SMSSend_STK_SIZE];
__attribute__((aligned(8))) static OS_STK  AppSMSReceiveTaskStk[APP_TASK_SMSReceive_STK_SIZE];
__attribute__((aligned(8))) static OS_STK  AppRS485TaskStk[APP_TASK_RS485_STK_SIZE];
__attribute__((aligned(8))) static OS_STK  AppSignalTaskStk[APP_TASK_SIGNAL_STK_SIZE];


//信号初始状态为连通
//static uint8_t signal_state = SET;


// 设备运行参数
OS_EVENT *MUTEX_DEVICE_INIT_PARAMATERS;
DEVICE_INIT_PARAMATERS device_init_paramaters;
//SLAVE_DEVICE_STATE_FRAME slave_device_state_frame[SLAVE_DEVICE_MAX_NUMBERS];
//TIME_FRAME set_time;

const DEVICE_INIT_PARAMATERS device_init_paramaters_const = {
  "zxsoft:as-2011-06-09",//device name
  1,//id
  "高新区一号开闭所",//pramiry device name
  "比亚迪出线电缆",//slave device name
  1,//alarm telephone numbers
  {
	"13474656377",//alarm telephone
  },
  "",//sms center number
  "\x00\x31\x00\x32\x00\x33\x00\x34\x00\x35\x00\x36",//系统设置密码
  "",//GPS info
  1,//sms switch
  BAUDRATE_1200,//rs485 baudrate
  0,
  {
	0,//signal interval time
	SIGNAL_FREQ_30000,//signal freq
	200,//signal freq spread
	500,//signal amp limit
	10,//signal process times
	999,//signal process interval
	0,//signal send freq
	0,//signal receive freq
	0,//signal amp
	SET,//signal state
  },
  0,//temperature
};
// 设备使用分配信号量
OS_EVENT *MUTEX_GSM;//设备使用互斥信号量;
OS_EVENT *MUTEX_RS485;//设备使用互斥信号量;
OS_EVENT *MUTEX_CALENDER;//日历互斥信号量
// 铁电存储器使用信号量
OS_EVENT *MUTEX_SFLASH;

// 用于发送中断完成指示
OS_EVENT *SEM_RS485_SEND_BYTE;//发送一个字节已完成信号量;
OS_EVENT *SEM_GSM_SEND_BYTE;//发送一个字节已完成信号量;

// 用于内存分配
OS_EVENT *SEM_MEM_PART_ALLOC;//内存分配信号量;
OS_MEM	  *MEM_BUF;//内存缓冲区;
uint8_t MEM_PART[MEM_PART_NUMBER][MEM_PART_SIZE];//内存缓冲区MEM_PART_NUMBER 个分区,各个分区大小为MEM_PART_SIZE Bytes;

// SMS发送告警发送邮件队列
OS_EVENT *Q_SMS_ALARM;
void *Q_SMS_ALARM_ARRAY[Q_SMS_ALARM_ARRAY_SIZE];//消息队列数组大小为10;

// SMS 消息指示信号量 
OS_EVENT *SEM_SMS_MSG_INDICATOR;


// 帧处理内存区
#define GSM_SEND_BUF_SIZE 256
uint8_t GSM_SEND_BUF[GSM_SEND_BUF_SIZE + 1] = {'\0',};

#define GSM_RECEIVE_BUF_SIZE 512
uint8_t GSM_RECEIVE_BUF[GSM_RECEIVE_BUF_SIZE + 1] = {'\0',};

#define SMS_RECEIVE_PROCESS_BUF_SIZE 256
uint8_t SMS_RECEIVE_PROCESS_BUF[SMS_RECEIVE_PROCESS_BUF_SIZE + 1] = {'\0',};
#define SMS_SEND_PROCESS_BUF_SIZE 512
uint8_t SMS_SEND_PROCESS_BUF[SMS_SEND_PROCESS_BUF_SIZE + 1] = {'\0',};
uint8_t SMS_UCS_PROCESS_BUF[DEVICE_NAME_MAX_LENGTH + 1] = {'\0',};

uint8_t RS485_SEND_BUF[256];
uint8_t RS485_RECEIVE_BUF[256];
uint8_t RS485_PROCESS_BUF[256];

OS_EVENT *SEM_SMS_OK;
OS_EVENT *SEM_SMS_FAULT;



// 用于MG232 的AT指令
const char *AT = (const char *)"AT\r";
const char *ATE0 = (const char *)"ATE0\r";
const char *ATIPR = (const char *)"AT+IPR=9600\r";
const char *ATCSQ = (const char *)"AT+CSQ\r";
const char *ATCMGF = (const char *)"AT+CMGF=0\r";
//int8_t *ATCNMI     = "AT+CNMI=3,1\r";
const char *ATCMGL = (const char *)"AT+CMGL=0\r";
const char *ATCNMI = (const char *)"AT+CNMI=1,2,0,0,0\r";
const char *ATCSCA = (const char *)"AT+CSCA?\r";
const char *ATCCLK = (const char *)"AT+CCLK?\r";
const char *ATOK = (const char *)"OK\r\n";
const char *ATCPMS = (const char *)"AT+CPMS=\"SM\",\"SM\",\"SM\"\r";
const char *ATSMSRI = (const char *)"AT*E2SMSRI=50\r";
const char *ATE2IPA = (const char *)"AT*E2IPA=1,1\r";
const char *ATE2IPA0 = (const char *)"AT*E2IPA=0,1\r";
const char *ATCT = (const char *)"CONNECT\r\n";
const char *ATO = (const char *)"ATO\r";
const char *ATE2IPC = (const char *)"AT*E2IPC\r";
const char *ATASKCGDCONT = (const char *)"AT+CGDCONT?\r";
const char *ATE2IPS  = (const char *)"AT*E2IPS=2,8,2,1020,1\r";
const char *ATGETIP  = (const char *)"AT*E2IPI=0\r";
const char *ATCIMI  = (const char *)"AT+CIMI\r";
const char *ZXSOFT = (const char *)"\x00\x7a\x00\x78\x00\x73\x00\x6f\x00\x66\x00\x74";//ZXSOFT帧头匹配;
const char *SMS_SET = (const char *)"\x8b\xbe\x7f\x6e";//设置命令匹配参数;

const char *SET_PRIMARY_DEVICE_NAME = (const char *)"\x00\x30\x00\x31\x00\x30";//010;设置主设备名称;
const char *SET_SLAVE_DEVICE_NUMBERS = (const char *)"\x00\x30\x00\x32\x00\x30";//020;设置从设备数;
const char *SET_SLAVE_DEVICE_ID = (const char *)"\x00\x30\x00\x32\x00\x31";//021;//设置从设备通讯ID;
const char *SET_SLAVE_DEVICE_NAME = (const char *)"\x00\x30\x00\x32\x00\x32";//022;//设置从设备名称;
const char *SET_ALARM_TELEPHONE_NUMBERS = (const char *)"\x00\x30\x00\x33\x00\x30";//030;设置告警电话数量;
const char *SET_ALARM_TELEPHONE = (const char *)"\x00\x30\x00\x33\x00\x31";//031;设置告警电话号码;

const char *SMS_LIST = (const char *)"\x67\xe5\x8b\xe2";//查询命令;
const char *SUPER_PASSWORD = (const char *)"\x00\x36\x00\x35\x00\x34\x00\x33\x00\x32\x00\x31";

static  void  AppStartTask(void *p_arg);
//static  void  AppLCDTask(void *p_arg);
static  void  AppTaskCreate(void);
static void AppGSMTask(void *p_arg);
static void AppRS485Task(void *p_arg);
static void AppSignalTask(void *p_arg);
static void AppSMSSendTask(void *p_arg);
//static void AppSMSReceiveTask(void *p_arg);
static void AppSMSReceiveTask(void *p_arg);

static uint8_t byte2BCD(uint8_t bVal)
{
  return  ( (bVal/10)<<4) + (bVal%10);
}


//uint8_t data[1000] = {0,};

int  main (void)
{
  INT8U  err;
  *NVIC_CCR= *NVIC_CCR | 0x200; // 设置STKALIGN位

  // Set the Vector Table base location at 0x08000000
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   


  //  sprintf((char *)GSM_SEND_BUF, "%f", 1.12);
  //BSP_IntDisAll();                   // Disable all interrupts until we are ready to accept them

  OSInit();                          // Initialize "uC/OS-II, The Real-Time Kernel"

  // Create the start task
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

  //OSTimeSet(0);
  OSStart();                                                  // Start multitasking (i.e. give control to uC/OS-II)
  return 0;
}
static  void  AppTaskCreate(void)
{
  OSTaskCreateExt(AppGSMTask,
				  (void *)0,
				  (OS_STK *)&AppGSMTaskStk[APP_TASK_GSM_STK_SIZE-1],
				  APP_TASK_GSM_PRIO,
				  APP_TASK_GSM_PRIO,
				  (OS_STK *)&AppGSMTaskStk[0],
				  APP_TASK_GSM_STK_SIZE,
				  (void *)0,
				  OS_TASK_OPT_STK_CLR);
    
  OSTaskCreateExt(AppSMSSendTask,
				  (void *)0,
				  (OS_STK *)&AppSMSSendTaskStk[APP_TASK_SMSSend_STK_SIZE-1],
				  APP_TASK_SMSSend_PRIO,
				  APP_TASK_SMSSend_PRIO,
				  (OS_STK *)&AppSMSSendTaskStk[0],
				  APP_TASK_SMSSend_STK_SIZE,
				  (void *)0,
				  OS_TASK_OPT_STK_CLR);
  
  OSTaskCreateExt(AppSMSReceiveTask,
				  (void *)0,
				  (OS_STK *)&AppSMSReceiveTaskStk[APP_TASK_SMSReceive_STK_SIZE -1],
				  APP_TASK_SMSReceive_PRIO,
				  APP_TASK_SMSReceive_PRIO,
				  (OS_STK *)&AppSMSReceiveTaskStk[0],
				  APP_TASK_SMSReceive_STK_SIZE,
				  (void *)0,
				  OS_TASK_OPT_STK_CLR);
  OSTaskCreateExt(AppSignalTask,
				  (void *)0,
				  (OS_STK *)&AppSignalTaskStk[APP_TASK_SIGNAL_STK_SIZE-1],
				  APP_TASK_SIGNAL_PRIO,
				  APP_TASK_SIGNAL_PRIO,
				  (OS_STK *)&AppSignalTaskStk[0],
				  APP_TASK_SIGNAL_STK_SIZE,
				  (void *)0,
				  OS_TASK_OPT_STK_CLR);
  OSTaskCreateExt(AppRS485Task,
				  (void *)0,
				  (OS_STK *)&AppRS485TaskStk[APP_TASK_RS485_STK_SIZE-1],
				  APP_TASK_RS485_PRIO,
				  APP_TASK_RS485_PRIO,
				  (OS_STK *)&AppRS485TaskStk[0],
				  APP_TASK_RS485_STK_SIZE,
				  (void *)0,
				  OS_TASK_OPT_STK_CLR);
  
    
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
  SEM_GSM_SEND_BYTE = OSSemCreate(0);

  MUTEX_GSM = OSMutexCreate(MUTEX_GSM_PIP, &err);
  MUTEX_RS485 = OSMutexCreate(MUTEX_RS485_PIP, &err);
  MUTEX_SFLASH = OSMutexCreate(MUTEX_SFLASH_PIP, &err);
  MUTEX_CALENDER = OSMutexCreate(MUTEX_CALENDER_PIP, &err);

  // 创建MEM_PART_NUMBER 个内存分区且分区大小为MEM_PART_SIZE 供邮箱队列使用
  SEM_MEM_PART_ALLOC = OSSemCreate(MEM_PART_NUMBER);
  MEM_BUF = OSMemCreate(MEM_PART, MEM_PART_NUMBER, MEM_PART_SIZE, &err);

  // 创建邮件列表且列表大小为Q_SMS_ALARM_ARRAY_SIZE 个消息
  Q_SMS_ALARM = OSQCreate(Q_SMS_ALARM_ARRAY, Q_SMS_ALARM_ARRAY_SIZE);

  // 创建短消息状态信号量
  SEM_SMS_OK = OSSemCreate(0);
  SEM_SMS_FAULT = OSSemCreate(0);

  //  MUTEX_DEVICE_INIT_PARAMATERS = OSMutexCreate(MUTEX_DEVICE_INIT_PARAMETERS_PIP, &err);

  SEM_SMS_MSG_INDICATOR = OSSemCreate(0);

  AppTaskCreate();                                            /* Create application tasks                                 */
  // 全局变量初始化
  OSMutexPend(MUTEX_SFLASH, 0, &err);

  sFLASH_ReadBuffer((uint8_t *)&device_init_paramaters, \
					SFLASH_DEVICE_INIT_PARAMATERS_START, \
					sizeof(DEVICE_INIT_PARAMATERS));

  if(memcmp(device_init_paramaters.device_name, device_init_paramaters_const.device_name, DEVICE_NAME_MAX_LENGTH))
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
	  OSTimeDlyHMSM(0,0,0,500);
	  led_toggle(LED_RUN);
	  /*	   
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
	  */
	  //读取温度
	  if(TP_convert())
		{
		  device_init_paramaters.temperature = (int16_t)(TP_read() * TP_CONVERT_VALUE * 10);
		}
	  //读取时间
	  calender_get(&(device_init_paramaters.calender));
	  OSTimeDlyHMSM(0,0,0,400);

	}
}


/*****************************************************

				GSM 处理任务

******************************************************/
static void AppGSMTask(void *p_arg)
{
  uint8_t err;
  char *match;
  (void)p_arg;
  uint8_t counts = 0;
  uint8_t cnt = 0;
  OSTimeDlyHMSM(0, 0, 10, 0);//冷却模块否则GSM_VIO状态不对;
  while(1)
	{
	  if(gsm_setup(ENABLE) == GSM_SETUP_ENABLE_FAILURE)
		{
		  OSTimeDlyHMSM(0, 0, 10, 0);//冷却模块否则GSM_VIO状态不对;
		  continue;			
		}
	  else
		{
		  /*****************************************************
					验证芯片与GSM 通讯是否正常
		  ******************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm(AT, SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);

		  match = strstr((const char *)GSM_RECEIVE_BUF, ATOK);
		  OSMutexPost(MUTEX_GSM);
		  if(!match)
			{
			  led_off(LED_GSM);
			  cnt++;
			  if(cnt > 10)
				{
				  cnt = 0;					
				}
			  else
				{
				  OSTimeDlyHMSM(0, 0, 2, 0);//等待接收数据
				}	
			  continue;
			}
		  
		  led_on(LED_GSM);
		  
		  /*****************************************************
						验证通讯网络注册状况
		  ******************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm("AT+CREG?\r", SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
			
		  match = strstr((const char *)GSM_RECEIVE_BUF, "+CREG: 0,1\r\n\r\nOK\r\n");
			
			
		  if(!(match || strstr((const char *)GSM_RECEIVE_BUF, "+CREG: 0,5\r\n\r\nOK\r\n")))
			{
			  OSMutexPost(MUTEX_GSM);
			  counts++;
			  if(counts > 10)
				{
				  counts = 0;
				  if(gsm_setup(DISABLE))
					{
					  OSTimeDlyHMSM(0, 0, 10, 0);//冷却模块否则GSM_VIO状态不对;
					}
					
				}
			  else
				{
				  OSTimeDlyHMSM(0, 0, 5, 0);//冷却模块否则GSM_VIO状态不对;
				
				}
			  continue;
			}
		  OSMutexPost(MUTEX_GSM);

		  /******************************************************************
			 信号强度检测;
		  *******************************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm(ATCSQ, SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
		  match = strstr((const char *)GSM_RECEIVE_BUF, ATOK);

		  if(!match)
			{
			  OSMutexPost(MUTEX_GSM);
			  continue;
			}
		  siscanf((const char *)GSM_RECEIVE_BUF, "%*[^ ]%d[^,]", (int *)&(device_init_paramaters.gsm_signal_strength));
		  OSMutexPost(MUTEX_GSM);
		  
		  /******************************************************************
				设置短信数据格式为PDU;
		  *******************************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm(ATCMGF, SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);

		  match = strstr((const char *)GSM_RECEIVE_BUF, ATOK);
		  OSMutexPost(MUTEX_GSM);
		  if(!match)
			{
			  continue;
			}
		  /******************************************************************
				设置短信数据接受提醒消息;
		  *******************************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm(ATCNMI, SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
			
		  match = strstr((const char *)GSM_RECEIVE_BUF, ATOK);
		  OSMutexPost(MUTEX_GSM);
		  if(!match)
			{
			  continue;
			}
		  /******************************************************************
				设置短信读取、写入、存储空间类型都为SM;(SIM卡存储)
		  *******************************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  //Send_To_GSM(ATCPMS, SEND_ALL);
			
		  send_to_gsm(ATCPMS, SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
			
		  match = strstr((const char *)GSM_RECEIVE_BUF, ATOK);
		  OSMutexPost(MUTEX_GSM);
		  if(!match)
			{
			  continue;
			}
		  /*****************************************************************************
								删除无用短信数据
		  ******************************************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm("AT+CMGD=1,3\r", SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
		  OSMutexPost(MUTEX_GSM);
		  /*****************************************************
					读取短消息中心号码
		  ******************************************************/
		  OSMutexPend(MUTEX_GSM, 0, &err);
		  //Flush_GSM_Buffer();
		  send_to_gsm("AT+CSCA?\r", SEND_ALL);
		  OSTimeDlyHMSM(0, 0, 0, 100);//等待接收数据
		  
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
			
		  match = strstr((const char *)GSM_RECEIVE_BUF, ATOK);
		  if(match)
			{
			  siscanf((const char *)GSM_RECEIVE_BUF, "%*[^86]86%[^\"]", GSM_SEND_BUF);
			  if(!memcmp((void *)(device_init_paramaters.service_center_address), GSM_SEND_BUF, ALARM_TELEPHONE_NUMBER_SIZE - 1))
				{

				}
			  else
				{
				  memcpy((void *)(device_init_paramaters.service_center_address), GSM_SEND_BUF, ALARM_TELEPHONE_NUMBER_SIZE - 1);
				  OSMutexPend(MUTEX_SFLASH, 0, &err);
				  /*
					Iron_Write(IRON_SERVICE_CENTER_ADDRESS, \
					device_init_paramaters.service_center_address, \
					ALARM_TELEPHONE_NUMBER_SIZE);*/
				  sFLASH_WriteBuffer(device_init_paramaters.service_center_address, \
									 SFLASH_DEVICE_INIT_PARAMATERS_START +	\
									 OFF_SET_OF(DEVICE_INIT_PARAMATERS, service_center_address) , \
									 ALARM_TELEPHONE_NUMBER_SIZE);
				  OSMutexPost(MUTEX_SFLASH);

				}
			  OSMutexPost(MUTEX_GSM);
			}
		  else
			{
			  OSMutexPost(MUTEX_GSM);
			  continue;
			}

		  OSSemPost(SEM_SMS_OK);//启动SMSSend 任务;
		  OSSemPend(SEM_SMS_FAULT, 0, &err);//等待SMS发送失败信号;
		  flush_gsm_buffer();
		  //OSTimeDlyHMSM(0, 5, 0, 0);//检验完成以后每五分钟检测一次;
		}
	}
}

TP_OA_TYPE TP_OA_temp;
//OS_SEM_DATA sem_data;
static void AppSMSReceiveTask(void *p_arg)
{
  (void)p_arg;
  uint8_t err;
  char *match = NULL;
  uint16_t *temp = NULL;
  // 由于SSCANF函数%d输出数据时必须是偶地址
  //  uint16_t unread_sms_index = 0;
  uint16_t length = 0;
  uint16_t index = 0;
  int TEMP[5];
  //  uint16_t id = 0;

  SMS_RECEIVE_PDU_FRAME *sms_receive_pdu_frame = NULL; 
  SMS_SET_FRAME *sms_set_frame = NULL;
  SMS_LIST_FRAME *sms_list_frame = NULL;
  SMS_MAIL_FRAME * sms_mail = NULL;
  SMS_QUERY_FRAME *sms_query_mail = NULL;
  DEVICE_INIT_PARAMATERS *device_parameters = &device_init_paramaters;
	
  while(1)
	{
	  OSTimeDlyHMSM(0, 0, 10, 0);//每十秒钟检测一次;	  
	  //OSSemQuery(SEM_SMS_OK, &sem_data);//有短消息信号;
	  //sem_sms_ok = sem_data.OSCnt;
	  //OSSemQuery(SEM_SMS_FAULT, &sem_data);//有短消息信号;
	  //sem_sms_fault = sem_data.OSCnt;
	  //	  if (sem_sms_ok == 1 && sem_sms_fault == 0)
	  //{
	  OSMutexPend(MUTEX_GSM, 0, &err);
	  flush_gsm_buffer();
	  send_to_gsm(ATCMGL, SEND_ALL);
	  OSTimeDlyHMSM(0, 0, 2, 0);
	  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
	  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
	  match = strstr((char *)GSM_RECEIVE_BUF, "\r\n+CMGL:");
	  
	  if(!match)
		{
		  OSMutexPost(MUTEX_GSM);	  
		  continue;
		}
	  else
		{
		  while(1)
			{
			  memset((void *)SMS_RECEIVE_PROCESS_BUF, '\0', SMS_RECEIVE_PROCESS_BUF_SIZE);
			  siscanf(match, "%*[^:]%*[^\r]\r\n%s[^\r]", SMS_RECEIVE_PROCESS_BUF);
			  String_To_Hex(SMS_RECEIVE_PROCESS_BUF, SMS_RECEIVE_PROCESS_BUF, strlen((char *)SMS_RECEIVE_PROCESS_BUF));
		
			  sms_receive_pdu_frame = (SMS_RECEIVE_PDU_FRAME *)SMS_RECEIVE_PROCESS_BUF;//用PDU结构体解析出PDU格式中的短信数据;
			  /* 检测帧尾必须为# 标记 */
			  if(sms_receive_pdu_frame->TPDU.TP_UD[sms_receive_pdu_frame->TPDU.TP_UDL- 1] == (uint8_t)(POUND_SIGN >> 8) &&\
				 sms_receive_pdu_frame->TPDU.TP_UD[sms_receive_pdu_frame->TPDU.TP_UDL - 2] == (uint8_t)(POUND_SIGN))
				{
				  /* 
				   * 由于以后需要用到整型数据来解析帧数据并且该数据
				   * 地址为奇地址所以必须把数据定位到偶地址上,这样用
				   * 整型解析数据的时候就不会出错.整型变量在奇地址的
				   * 时候就会出现总线取值错误;16位数据总线一次取得数
				   * 据为两个字节所以每次取数据的地址应该为偶地址
				   * 如果是奇地址的时候，去字符数据时取一次数据就可以
				   * 但是如果取整型数据是理论上则需要取两次数据才可以
				   * 取到所需要的数据,但是有的CPU则不支持奇地址取数据;
				   */
				  TP_OA_temp = sms_receive_pdu_frame->TPDU.TP_OA;
				  memcpy((void *)SMS_RECEIVE_PROCESS_BUF, \
						 (void *)(sms_receive_pdu_frame->TPDU.TP_UD), \
						 (size_t)(sms_receive_pdu_frame->TPDU.TP_UDL));
			
				  sms_list_frame = (SMS_LIST_FRAME *)SMS_RECEIVE_PROCESS_BUF;//用LIST短信结构体来解析短信数据;

				  err = memcmp((void *)sms_list_frame->ZXSOFT,	\
							   (void *)ZXSOFT, \
							   sizeof(sms_list_frame->ZXSOFT));
				  /* 帧头匹配处理 */
				  if(!err)
					{
					  /* 功能间隔符# 匹配处理 */
					  if((sms_list_frame->SIGN_1 == POUND_SIGN) && (sms_list_frame->SIGN_2 == POUND_SIGN))
						{
						  err = memcmp((void *)sms_list_frame->LIST, \
									   (void *)SMS_SET, \
									   sizeof(sms_list_frame->LIST));
						  if(err)
							{
							  if(sms_list_frame->SIGN_3 == POUND_SIGN)
								{
								  switch ((uint8_t)((sms_list_frame->FUCTION_CODE[0] >> 4) & 0xf0) | \
										  ((sms_list_frame->FUCTION_CODE[1] >> 8) & 0x0f))
									{
									  /* 功能码为#10#的时候数据处理 */
									case 0x10 : {
									  /* 查询主设备名称 */
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x10;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									  /* 功能码为#11#的时候数据处理 */
									case 0x11 : {
									  /* 设置主设备密码 */
											
									  break;
		
									}
									  /* 功能码为#20#的时候数据处理 */
									case 0x20 : {
									  /* 查询从设备数量 */
											
									  break;
		
									}
									  /*****************/
									  /* 功能码为#21#的时候数据处理 */
									case 0x21 : {
									  /* 查询从设备ID */
									  break;
		
									}
									  /***********/
									  /* 功能码为#22#的时候数据处理 */
									case 0x22 : {
									  /* 设置从设备名称 */
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x22;									  
									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									
									  /* 功能码为#30#的时候数据处理 */
									case 0x30 : {
									  /* 设置报警手机号码数量 */
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x30;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									
									  /* 功能码为#31#的时候数据处理 */
									case 0x31 : {
									  /* 设置手机号码 */
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x31;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									  /* 功能码为#40#的时候数据处理 */
									case 0x40 : {
									  /* 查询电缆状态*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x40;
									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
											
									  break;
		
									}
									  /* 功能码为#41#的时候数据处理 */
									case 0x41 : {
									  /* 查询短信告警开关状态*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x41;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x50 : {
									  /* 查询gps位置*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x50;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x51 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x51;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x52 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x52;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x53 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x53;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x54 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x54;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x55 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x55;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x56 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x56;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									case 0x57 : {
									  /* 查询信号间隔时间*/
									  /* 分配一个16 字节的内存分区 用于告警邮件的存储*/
									  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
									  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
									  sms_mail->sms_mail_type = SMS_QUERY_FRAME_TYPE;
									  sms_query_mail = &(sms_mail->sms_query_frame);
									  sms_query_mail->TP_OA = TP_OA_temp;
									  sms_query_mail->function_code = 0x57;

									  /* 发送短信告警邮件 */
									  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
									  break;
		
									}
									  /* 无效功能码时不做处理 */
									default :

									  break;
									}
								}
							  else
								{
								  //帧格式不匹配;
								}
							}
						  else
							{
							  /* SET 命令类型匹配处理 */
							  sms_set_frame = (SMS_SET_FRAME *)sms_list_frame;//用SET 命令结构来解析数据;

							  /* 功能间隔符# 匹配处理 */
							  if((sms_set_frame->SIGN_3 == POUND_SIGN) && (sms_set_frame->SIGN_4 == POUND_SIGN))
								{
								  /* 设置操作密码处理 */
								  err = memcmp((void *)sms_set_frame->PASSWORD, \
											   (void *)SUPER_PASSWORD, \
											   sizeof(sms_set_frame->PASSWORD));

								  if(!err)
									{
									  err = memcmp((void *)sms_set_frame->PASSWORD, \
												   (void *)device_init_paramaters.password, \
												   sizeof(sms_set_frame->PASSWORD));
									}
								
								  if(err)
									{
									  /* 设置操作密码正确,处理功能码 */
									  switch ((uint8_t)((sms_set_frame->FUCTION_CODE[0] >> 4) & 0xf0) | \
											  ((sms_set_frame->FUCTION_CODE[1] >> 8) & 0x0f)) 
										{
										  /* 功能码为#10#的时候数据处理 */
										case 0x10 : {
										  /* 设置主设备名称 */
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//取得设备名称长度;
										  if(length)
											{
											  memset((void *)device_parameters->primary_device_name, \
													 0, \
													 (size_t)DEVICE_NAME_MAX_LENGTH);
											  if(length <= (DEVICE_NAME_MAX_LENGTH >> 1))//不能超出预设长度;
												{
												  memcpy((void *)device_parameters->primary_device_name, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(length << 1));
												}
											  else
												{
												  memcpy((void *)device_parameters->primary_device_name, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(DEVICE_NAME_MAX_LENGTH));
												}
											  OSMutexPend(MUTEX_SFLASH, 0, &err);
											  /*
												Iron_Write(IRON_PRIMARY_DEVICE_NAME, \
												device_parameters->primary_device_name, \
												DEVICE_NAME_MAX_LENGTH);
											  */
											  sFLASH_WriteBuffer(device_parameters->primary_device_name, \
																 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																 OFF_SET_OF(DEVICE_INIT_PARAMATERS, primary_device_name),	\
																 DEVICE_NAME_MAX_LENGTH);
											  OSMutexPost(MUTEX_SFLASH);
											}
										  break;
		
										}
										  /* 功能码为#11#的时候数据处理 */
										case 0x11 : {
										  /* 设置主设备密码 */
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//取得密码长度;
										  if(length)
											{
											  memset((void *)device_parameters->password, \
													 0, \
													 (size_t)DEVICE_PASSWORD_MAX_LENGTH);
											  if(length <= (DEVICE_PASSWORD_MAX_LENGTH >> 1))//不能超出预设长度;
												{
												  memcpy((void *)device_parameters->password, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(length << 1));
												}
											  else
												{
												  memcpy((void *)device_parameters->password, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(DEVICE_PASSWORD_MAX_LENGTH));
												}								
												
											  OSMutexPend(MUTEX_SFLASH, 0, &err);
											  /*
												Iron_Write(IRON_PASSWORD, \
												device_parameters->password, \
												DEVICE_PASSWORD_MAX_LENGTH);
											  */
											  sFLASH_WriteBuffer(device_parameters->password, \
																 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																 OFF_SET_OF(DEVICE_INIT_PARAMATERS, password), \
																 DEVICE_PASSWORD_MAX_LENGTH);
													
											  OSMutexPost(MUTEX_SFLASH);

											}
										  break;
		
										}
										  /* 功能码为#20#的时候数据处理 */
										case 0x20 : {
										  /* 设置从设备数量 */
										  break;
		
										}
										  /*****************/
										  /* 功能码为#21#的时候数据处理 */
										case 0x21 : {
										  /* 设置从设备ID */
										  break;
		
										}
										  /***********/
										  /* 功能码为#22#的时候数据处理 */
										case 0x22 : {
										  /* 设置从设备名称 */
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{
											  memset((void *)device_parameters->slave_device_name, \
													 0, \
													 (size_t)DEVICE_NAME_MAX_LENGTH);
											  if(length <= (DEVICE_NAME_MAX_LENGTH >> 1))//不能超出预设长度;
												{
												  memcpy((void *)device_parameters->slave_device_name, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(length << 1));
												}
											  else
												{
												  memcpy((void *)device_parameters->slave_device_name, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(DEVICE_NAME_MAX_LENGTH));
												}
											  OSMutexPend(MUTEX_SFLASH, 0, &err);
											  /*
												Iron_Write(IRON_SLAVE_DEVICE_NAME, \
												device_parameters->slave_device_name, \
												DEVICE_NAME_MAX_LENGTH);
											  */
											  sFLASH_WriteBuffer(device_parameters->slave_device_name, \
																 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																 OFF_SET_OF(DEVICE_INIT_PARAMATERS, slave_device_name),	\
																 DEVICE_NAME_MAX_LENGTH);
											  OSMutexPost(MUTEX_SFLASH);
											  
											}
										  else
											{
											  //无数据项;
											}
										  break;
		
										}
									
										  /* 功能码为#30#的时候数据处理 */
										case 0x30 : {
										  /* 设置报警手机号码数量 */
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{
											  if(length <= 5)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= ALARM_TELEPHONE_MAX_NUMBERS)
													{
													  device_parameters->alarm_telephone_numbers = ALARM_TELEPHONE_MAX_NUMBERS;
													}
												  else
													{
													  device_parameters->alarm_telephone_numbers = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  /*
													Iron_Write(IRON_ALARM_TELEPHONE_NUMBERS, \
													(uint8_t *)&(device_parameters->alarm_telephone_numbers), \
													1);
												  */
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->alarm_telephone_numbers), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone_numbers), \
																	 sizeof(device_parameters->alarm_telephone_numbers));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于5个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
									
										  /* 功能码为#31#的时候数据处理 */
										case 0x31 : {
										  /* 设置手机号码 */
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{
											  if(length <= 20)
												{
												  temp = UCS_Char(sms_set_frame->DATA, COMMA_SIGN, length);//查找逗号分隔符;
												  if(temp)
													{
													  length = UCS_Len(sms_set_frame->DATA, COMMA_SIGN);
													  /* 分配内存 */
													  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
													  match = OSMemGet(MEM_BUF, &err);
														
													  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
															  "%d[^#]",\
															  (int *)&index);
													  /* 释放内存 */
													  OSMemPut(MEM_BUF, (void *)match);
													  OSSemPost(SEM_MEM_PART_ALLOC);
													  if(index && (index <= device_parameters->alarm_telephone_numbers))
														{
														  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN) - length;//名称域长度;
														  if(--length)
															{
															  if(length == ALARM_TELEPHONE_NUMBER_SIZE - 1)
																{
																  /* 分配内存 */
																  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
																  match = OSMemGet(MEM_BUF, &err);
																	
																  siscanf((char *)UCS_To_String(temp, (uint8_t *)match, length), \
																		  "%11s[^#]",\
																		  device_parameters->alarm_telephone[index - 1]);
																  /* 释放内存 */
																  OSMemPut(MEM_BUF, (void *)match);
																  OSSemPost(SEM_MEM_PART_ALLOC);
																  /*
																	Iron_Write(IRON_ALARM_TELEPHONE + (index - 1) * ALARM_TELEPHONE_NUMBER_SIZE, \
																	device_parameters->alarm_telephone[index - 1], \
																	ALARM_TELEPHONE_NUMBER_SIZE);
																  */
																  OSMutexPend(MUTEX_SFLASH, 0, &err);
																  sFLASH_WriteBuffer(device_parameters->alarm_telephone[index - 1], \
																					 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																					 OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone) + (index - 1) * ALARM_TELEPHONE_NUMBER_SIZE, \
																	ALARM_TELEPHONE_NUMBER_SIZE - 1);
																  OSMutexPost(MUTEX_SFLASH);
																}
															  else
																{
																  //长度不等于11位手机号码; 
																}
															}
														  else 
															{ 
															  //名称数据域为空; 
															} 
														} 
													  else 
														{ 
														  //设置序号不符合要求 
														} 
													} 
												  else 
													{ 
													  //无逗号分隔符; 
													} 
													
												} 
											  else 
												{ 
												  //长度大于20个字符数据; 
												} 
											} 
										  else 
											{ 
											  //无数据项; 
											} 
													
										  __NOP(); 
										  break; 
		
										} 
										  /* 功能码为#31#的时候数据处理 */
										case 0x40 : { 
										  /* 设置从设备时间*/
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{ 
											  if(length <= 16)
												{
												  if(UCS_Char(sms_set_frame->DATA, COMMA_SIGN, length))//查找逗号分隔符;
													{
													  /* 分配内存 */
													  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
													  match = OSMemGet(MEM_BUF, &err);
													  
													  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
															  "%d,%d,%d,%d,%d[^#]",\
															  &TEMP[0],&TEMP[1],&TEMP[2],&TEMP[3],&TEMP[4]);
													  
													  /* 释放内存 */
													  
													  OSMemPut(MEM_BUF, (void *)match);
													  OSSemPost(SEM_MEM_PART_ALLOC);

													  OSMutexPend(MUTEX_CALENDER, 0, &err);

													  device_parameters->calender.tm_year = byte2BCD((uint8_t)(TEMP[0] % 100));
													  device_parameters->calender.tm_mon = byte2BCD((uint8_t)(TEMP[1] % 100));
													  device_parameters->calender.tm_mday = byte2BCD((uint8_t)(TEMP[2] % 100));
													  device_parameters->calender.tm_hour = byte2BCD((uint8_t)(TEMP[3] % 100));
													  device_parameters->calender.tm_min = byte2BCD((uint8_t)(TEMP[4] % 100));
													  calender_set(&(device_parameters->calender));

													  OSMutexPost(MUTEX_CALENDER);
																										  
													}
												  else
													{
													  //没有逗号分隔符;
													}

												}
											  else
												{
												  //长度大于2个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
										  break;
		
										}
										case 0x41 : {
										  /* 设置告警信息开关*/
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{
											  if(length <= 1)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length == 1)
													{
													  device_parameters->sms_on_off = 1;
													}
												  else
													{
													  device_parameters->sms_on_off = 0;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  /*
													Iron_Write(IRON_ALARM_TELEPHONE_NUMBERS, \
													(uint8_t *)&(device_parameters->alarm_telephone_numbers), \
													1);
												  */
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->sms_on_off), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, sms_on_off), \
																	 sizeof(device_parameters->sms_on_off));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于5个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
										  break;
		
										}
										  /* 功能码为#10#的时候数据处理 */
										case 0x50 : {
										  /* 设置GPS位置 */
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//取得设备名称长度;
										  if(length)
											{
											  memset((void *)device_parameters->gps, \
													 0, \
													 (size_t)GPS_MAX_LENGTH);
											  if(length <= (GPS_MAX_LENGTH >> 1))//不能超出预设长度;
												{
												  memcpy((void *)device_parameters->gps, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(length << 1));
												}
											  else
												{
												  memcpy((void *)device_parameters->gps, \
														 (void *)sms_set_frame->DATA, \
														 (size_t)(GPS_MAX_LENGTH));
												}
											  OSMutexPend(MUTEX_SFLASH, 0, &err);
											  /*
												Iron_Write(IRON_PRIMARY_DEVICE_NAME, \
												device_parameters->primary_device_name, \
												DEVICE_NAME_MAX_LENGTH);
											  */
											  sFLASH_WriteBuffer(device_parameters->gps, \
																 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																 OFF_SET_OF(DEVICE_INIT_PARAMATERS, gps), \
																 GPS_MAX_LENGTH);
											  OSMutexPost(MUTEX_SFLASH);
											}
										  break;
		
										}
										case 0x51 : {
										  //设置信号间隔时间
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);
										  if(length)
											{
											  if(length <= 4)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 3000)
													{
													  device_parameters->signal_parameters.interval = 0;
													}
												  else
													{
													  device_parameters->signal_parameters.interval = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->signal_parameters.interval), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + \
																	 OFF_SET_OF(SignalParameters, interval),	\
																	 sizeof(device_parameters->signal_parameters.interval));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于4个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										case 0x52 : {
										  //设置信号频率
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);
										  if(length)
											{
											  if(length <= 2)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 10)
													{
													  device_parameters->signal_parameters.freq = SIGNAL_FREQ_30000;
													}
												  else
													{
													  device_parameters->signal_parameters.freq = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->signal_parameters.freq), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + \
																	 OFF_SET_OF(SignalParameters, freq),	\
																	 sizeof(device_parameters->signal_parameters.freq));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于4个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										case 0x53 : { 
										  // 设置频率差值与幅值限值
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{ 
											  if(length <= 10)
												{
												  if(UCS_Char(sms_set_frame->DATA, COMMA_SIGN, length))//查找逗号分隔符;
													{
													  /* 分配内存 */
													  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
													  match = OSMemGet(MEM_BUF, &err);
													  
													  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
															  "%d,%d[^#]",\
															  &TEMP[0],&TEMP[1]);
													  
													  /* 释放内存 */
													  
													  OSMemPut(MEM_BUF, (void *)match);
													  OSSemPost(SEM_MEM_PART_ALLOC);
													  device_parameters->signal_parameters.freq_spread = TEMP[0];//信号频率差值
													  device_parameters->signal_parameters.amp_limit = TEMP[1];//信号幅值限值
													  OSMutexPend(MUTEX_SFLASH, 0, &err);
													  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->signal_parameters.freq_spread), \
																		 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																		 OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + \
																		 OFF_SET_OF(SignalParameters, freq_spread),	\
																		 sizeof(device_parameters->signal_parameters.freq_spread) + \
																		 sizeof(device_parameters->signal_parameters.amp_limit));
													  OSMutexPost(MUTEX_SFLASH);

													}
												  else
													{
													  //没有逗号分隔符;
													}

												}
											  else
												{
												  //长度大于2个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
										  break;
		
										}
										case 0x54 : {
										  //设置信号处理次数
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);
										  if(length)
											{
											  if(length <= 4)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 3000)
													{
													  device_parameters->signal_parameters.process_counts = 10;
													}
												  else
													{
													  device_parameters->signal_parameters.process_counts = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->signal_parameters.process_counts), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + \
																	 OFF_SET_OF(SignalParameters, process_counts),	\
																	 sizeof(device_parameters->signal_parameters.process_counts));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于4个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										case 0x55 : {
										  //设置信号处理间隔
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);
										  if(length)
											{
											  if(length <= 4)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 3000)
													{
													  device_parameters->signal_parameters.process_interval = 1;
													}
												  else
													{
													  device_parameters->signal_parameters.process_interval = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->signal_parameters.process_interval), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + \
																	 OFF_SET_OF(SignalParameters, process_interval),	\
																	 sizeof(device_parameters->signal_parameters.process_interval));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于4个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										case 0x56 : {
										  //设置信号处理间隔
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);
										  if(length)
											{
											  if(length <= 4)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 3000)
													{
													  device_parameters->signal_parameters.process_interval = 1;
													}
												  else
													{
													  device_parameters->signal_parameters.process_interval = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->signal_parameters.process_interval), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + \
																	 OFF_SET_OF(SignalParameters, process_interval),	\
																	 sizeof(device_parameters->signal_parameters.process_interval));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于4个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										case 0x57 : {
										  //设置设备地址
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{
											  if(length <= 5)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%x[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 0xff)
													{
													  device_parameters->device_id = 0x01;
													}
												  else
													{
													  device_parameters->device_id = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  /*
													Iron_Write(IRON_ALARM_TELEPHONE_NUMBERS, \
													(uint8_t *)&(device_parameters->alarm_telephone_numbers), \
													1);
												  */
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->device_id), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, device_id), \
																	 sizeof(device_parameters->device_id));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于5个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										case 0x58 : {
										  //设置设备地址
										  length = UCS_Len(sms_set_frame->DATA, POUND_SIGN);//设置从设备数量数据长度;
										  if(length)
											{
											  if(length <= 2)
												{
												  /* 分配内存 */
												  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
												  match = OSMemGet(MEM_BUF, &err);
													
												  siscanf((char *)UCS_To_String(sms_set_frame->DATA, (uint8_t *)match, length), \
														  "%d[^#]", \
														  (int *)&length);
													
												  /* 释放内存 */
												  OSMemPut(MEM_BUF, (void *)match);
												  OSSemPost(SEM_MEM_PART_ALLOC);
												  if(length >= 6)
													{
													  device_parameters->rs485_baudrate = 0;
													}
												  else
													{
													  device_parameters->rs485_baudrate = length;
													}
												  OSMutexPend(MUTEX_SFLASH, 0, &err);
												  /*
													Iron_Write(IRON_ALARM_TELEPHONE_NUMBERS, \
													(uint8_t *)&(device_parameters->alarm_telephone_numbers), \
													1);
												  */
												  sFLASH_WriteBuffer((uint8_t *)&(device_parameters->rs485_baudrate), \
																	 SFLASH_DEVICE_INIT_PARAMATERS_START + \
																	 OFF_SET_OF(DEVICE_INIT_PARAMATERS, rs485_baudrate), \
																	 sizeof(device_parameters->rs485_baudrate));
												  OSMutexPost(MUTEX_SFLASH);
												}
											  else
												{
												  //长度大于5个字符数据;
												}
											}
										  else
											{
											  //无数据项;
											}
													
										  break;
		
										}
										  /* 无效功能码时不做处理 */
										default :

										  break;
										}
									}
								  else
									{
									  /* 设置操作密码不正确处理 */
									  __NOP();
									  //continue;
									}
								}
							  else
								{
								  __NOP();
								  //continue;
								}
							}
						}
					  else
						{
						  /* 功能间隔符# 不匹配处理 */
						  __NOP();
									  
						  //continue;
						}
					}
				  else
					{
					  /* 非法帧头处理 */
					  __NOP();
					  //continue;
					}
				}
			  else
				{
				  /* 非法帧尾 处理*/
				  //continue;
				}
			  match += 20;//阻止重复匹配已处理的帧
			  match = strstr(match, "\r\n+CMGL:");
			  if(!match)
				{
				  OSMutexPost(MUTEX_GSM);
				  break;
				}

			}
		  
		}

	  __NOP();
	  OSTimeDlyHMSM(0, 0, 0, 50);//等待接收数据
		
	  //		}
	}

}


GsmStatus sms_send(SMS_SEND_PDU_FRAME *sms_send_pdu_frame, uint16_t sms_data_length)
{
  char *match = NULL;
  //uint16_t sms_length = sms_send_pdu_frame->TPDU.TP_UDL;
  SMS_HEAD_6 sms_head = {0x05, 0x00, 0x03, 0x86, 0x00, 0x00};
  uint16_t temp;
  uint8_t sms_index;
  sms_head.sms_numbers = sms_data_length / 140;

  if(sms_head.sms_numbers)
	{
	  if(sms_data_length % 140)
		{
		  sms_head.sms_numbers++;
			
		}
	  else
		{
		  if(sms_head.sms_numbers == 1)
			goto PROCESS;
		}
	  temp = sms_data_length + sms_head.sms_numbers * sizeof(SMS_HEAD_6);

	  sms_head.sms_numbers = temp / 140;

	  if(temp % 140)
		{
		  sms_head.sms_numbers++;
			
		}
	}
	
 PROCESS:	


  if(sms_head.sms_numbers <= 1)
	{
	  //单条短信
	  sms_send_pdu_frame->TPDU.TP_UDL = sms_data_length;
	  siprintf((char *)GSM_SEND_BUF, \
			   "AT+CMGS=%d\x0D", \
			   sms_send_pdu_frame->TPDU.TP_UDL + \
			   (sizeof(sms_send_pdu_frame->TPDU) - sizeof(sms_send_pdu_frame->TPDU.TP_UD)));
	  send_to_gsm((char *)GSM_SEND_BUF, GSM_SEND_BUF_SIZE);
	  OSTimeDlyHMSM(0, 0, 0, 500);
	  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
	  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
	  match = memchr((char *)GSM_RECEIVE_BUF, '>', GSM_RECEIVE_BUF_SIZE);
	  if(!match)
		{
		  send_to_gsm("\x1B", 1);//取消发送
		  return GSM_SMS_SEND_FAILURE;
		}
	  memset((void *)match, 2, 5);
	  Send_PDU_To_GSM(sms_send_pdu_frame, &sms_head);
	  OSTimeDlyHMSM(0, 0, 0, 500);
	  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);
	  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
	  match = strstr((char *)GSM_RECEIVE_BUF, "ERROR");
	  if(!match)
		{
		  return GSM_SMS_SEND_SUCCESS;
		}
	}
  else
	{
	  //多条短信
	  for (sms_index = 1; sms_index <= sms_head.sms_numbers; sms_index++)
		{

		  //处理短信帧头
		  sms_head.sms_index = sms_index;
		  if(sms_index == sms_head.sms_numbers)
			{
			  sms_send_pdu_frame->TPDU.TP_UDL = (sms_data_length + sms_head.sms_numbers * sizeof(SMS_HEAD_6)) % 140;

			}
		  else
			{
			  sms_send_pdu_frame->TPDU.TP_UDL = 140;
			}
			
		  siprintf((char *)GSM_SEND_BUF, \
				   "AT+CMGS=%d\x0D", \
				   sms_send_pdu_frame->TPDU.TP_UDL + \
				   (sizeof(sms_send_pdu_frame->TPDU) - sizeof(sms_send_pdu_frame->TPDU.TP_UD)));
		  send_to_gsm((char *)GSM_SEND_BUF, GSM_SEND_BUF_SIZE);
		  OSTimeDlyHMSM(0, 0, 0, 500);
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);

		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
		  match = memchr((char *)GSM_RECEIVE_BUF, '>', GSM_RECEIVE_BUF_SIZE);
						
		  if(!match)
			{
			  send_to_gsm("\x1B", 1);//取消发送
			  return GSM_SMS_SEND_FAILURE;
			}
		  memset((void *)match, 2, 5);
		  Send_PDU_To_GSM(sms_send_pdu_frame, &sms_head);
		  OSTimeDlyHMSM(0, 0, 0, 500);
		  memset((void *)GSM_RECEIVE_BUF, '\0', GSM_RECEIVE_BUF_SIZE);

		  receive_from_gsm((char *)GSM_RECEIVE_BUF, GSM_RECEIVE_BUF_SIZE);
		  match = strstr((char *)GSM_RECEIVE_BUF, "ERROR");
		  if(!match)
			{
			  return GSM_SMS_SEND_SUCCESS;
			}
		}
	}
	
  return GSM_SMS_SEND_FAILURE;
}


void sms_alarm_mail_analysis(SMS_SEND_PDU_FRAME *sms_send_pdu_frame, SMS_ALARM_FRAME *sms_alarm_mail, DEVICE_INIT_PARAMATERS *device_parameters)
{
  GsmStatus gsm_status;
  uint8_t alarm_telephone_cnt;
  uint8_t err;
  uint8_t send_index;
  uint16_t sms_data_length;

  sms_send_pdu_frame->SMSC.SMSC_Length = SMSC_DEFAULT;
  if(sms_send_pdu_frame->SMSC.SMSC_Length)
	{
	  sms_send_pdu_frame->SMSC.SMSC_Type_Of_Address = TP_TYPE_DEFAULT;
	  TP_Str_To_Octet(sms_send_pdu_frame->SMSC.SMSC_Address_Value, \
					  device_parameters->service_center_address, \
					  TP_TYPE_DEFAULT, \
					  ALARM_TELEPHONE_NUMBER_SIZE - 1);
	}

  sms_send_pdu_frame->TPDU.First_Octet = FIRST_OCTET_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_MR = TP_MR_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_DA.TP_DA_Length = (TP_TYPE_DEFAULT == INTERNATIONAL_ADDRESS_TYPE) ? 0X0D : 0X0B;
  sms_send_pdu_frame->TPDU.TP_DA.TP_DA_Type_Of_Address = TP_TYPE_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_PID = TP_PID_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_DCS = TP_DCS_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_VP = TP_VP_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_UDL = 0;
  sms_data_length = 0;
  *((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length) = POUND_SIGN;//添加#号
  sms_data_length ++;//#号长度

  SMS_Send_User_Data_Copy((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length, \
						  device_parameters->primary_device_name, \
						  &(sms_data_length), \
						  DEVICE_NAME_MAX_LENGTH);
  *((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length) = LINE_SIGN;//添加-号
  sms_data_length ++;//-号长度

  SMS_Send_User_Data_Copy((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length, \
						  device_parameters->slave_device_name, \
						  &(sms_data_length), \
						  DEVICE_NAME_MAX_LENGTH);

  *((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length) = POUND_SIGN;//添加#号
  sms_data_length ++;//#号长度

  Alarm_Mail_Data_To_UCS((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length, \
						 sms_alarm_mail, \
						 &(sms_data_length));

  sms_data_length <<= 1;//字节数
  OSMutexPend(MUTEX_GSM, 0, &err);
  for(alarm_telephone_cnt = 0; \
	  alarm_telephone_cnt < device_parameters->alarm_telephone_numbers; \
	  alarm_telephone_cnt++)
	{


		
	  TP_Str_To_Octet(sms_send_pdu_frame->TPDU.TP_DA.TP_DA_Address_Value, \
					  device_parameters->alarm_telephone[alarm_telephone_cnt], \
					  TP_TYPE_DEFAULT, \
					  ALARM_TELEPHONE_NUMBER_SIZE - 1);
		

	  for(send_index = 0; send_index <= GSM_RESEND_NUMBERS; send_index++)
		{
		  gsm_status = sms_send(sms_send_pdu_frame, sms_data_length);
	
		  if(gsm_status == GSM_SMS_SEND_SUCCESS)
			{
			  break;//发送成功则退出重发机制	  
			}
		  else
			{
			  //发送失败
			  OSTimeDlyHMSM(0, 0, 1, 0);
			}
		}
	  OSTimeDlyHMSM(0, 0, 2, 0);
	}
  OSMutexPost(MUTEX_GSM);
}

void sms_query_mail_analysis(SMS_SEND_PDU_FRAME *sms_send_pdu_frame, SMS_QUERY_FRAME *sms_query_mail, DEVICE_INIT_PARAMATERS *device_parameters)
{
  GsmStatus gsm_status;
  //	uint8_t alarm_telephone_cnt;
  uint8_t err;
  uint16_t *UCS = NULL;
  uint16_t *UCS_len = NULL;
  uint16_t temp;
  uint8_t index;
  uint8_t send_index;
  //  SLAVE_DEVICE_STATE_FRAME *slave_device_state = NULL;
  uint16_t sms_data_length;
  sms_send_pdu_frame->SMSC.SMSC_Length = SMSC_DEFAULT;
  if(sms_send_pdu_frame->SMSC.SMSC_Length)
	{
	  sms_send_pdu_frame->SMSC.SMSC_Type_Of_Address = TP_TYPE_DEFAULT;
	  TP_Str_To_Octet(sms_send_pdu_frame->SMSC.SMSC_Address_Value, \
					  device_parameters->service_center_address, \
					  TP_TYPE_DEFAULT, \
					  ALARM_TELEPHONE_NUMBER_SIZE - 1);
	}

  sms_send_pdu_frame->TPDU.First_Octet = FIRST_OCTET_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_MR = TP_MR_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_DA.TP_DA_Length = (TP_TYPE_DEFAULT == INTERNATIONAL_ADDRESS_TYPE) ? 0X0D : 0X0B;
  sms_send_pdu_frame->TPDU.TP_DA.TP_DA_Type_Of_Address = TP_TYPE_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_PID = TP_PID_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_DCS = TP_DCS_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_VP = TP_VP_DEFAULT;
  sms_send_pdu_frame->TPDU.TP_UDL = 0;
  sms_data_length = 0;
  *((uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length) = POUND_SIGN;//添加#号
  sms_data_length ++;//#号长度
  switch(sms_query_mail->function_code)
	{
	case 0x10 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  UCS = SMS_Send_User_Data_Copy(UCS, \
									device_parameters->primary_device_name, \
									UCS_len, \
									DEVICE_NAME_MAX_LENGTH);

	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;

	  break;
	}
	case 0x20 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);	  
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;
	  break;
	}
	case 0x21 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);	  
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;
	  break;
	}
	case 0x22 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  UCS = SMS_Send_User_Data_Copy(UCS, \
									device_parameters->slave_device_name, \
									UCS_len, \
									DEVICE_NAME_MAX_LENGTH);
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;
	  break;
	}
	case 0x30 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->alarm_telephone_numbers;
	  if(temp <= ALARM_TELEPHONE_MAX_NUMBERS)
		{
		  if(temp < 10)
			{
			  *UCS++ = NUM_UCS_MAP[temp];
			  (*UCS_len)++;				
					
			}else if(temp < 100)
			{

			  *UCS++ = NUM_UCS_MAP[temp / 10];
			  *UCS++ = NUM_UCS_MAP[temp % 10];
			  *UCS_len += 2;
			}
						
		  *UCS++ = POUND_SIGN;
		  (*UCS_len)++;			

		}
	  break;
	}
	case 0x31 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  if(device_parameters->alarm_telephone_numbers <= ALARM_TELEPHONE_MAX_NUMBERS)			
		{
		  for(index = 0; index < device_parameters->alarm_telephone_numbers; index++)
			{
			  temp = index + 1;
			  if(temp < 10)
				{
				  *UCS++ = NUM_UCS_MAP[temp];
				  (*UCS_len)++;				
						
				}else if(temp < 100)
				{

				  *UCS++ = NUM_UCS_MAP[temp / 10];
				  *UCS++ = NUM_UCS_MAP[temp % 10];
				  *UCS_len += 2;
				}

					
			  *UCS++ = COMMA_SIGN;
			  (*UCS_len)++;
					
			  for(temp = 0; temp < ALARM_TELEPHONE_NUMBER_SIZE - 1; temp++)
				{
				  *UCS++ = NUM_UCS_MAP[device_parameters->alarm_telephone[index][temp] - 0x30];
				}
			  *UCS_len += ALARM_TELEPHONE_NUMBER_SIZE - 1;

			  *UCS++ = POUND_SIGN;
			  (*UCS_len)++;
			}
			
		}
	  break;
	}
	case 0x40 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  UCS = SMS_Send_User_Data_Copy(UCS,								\
									device_parameters->slave_device_name, \
									UCS_len,							\
									DEVICE_NAME_MAX_LENGTH);
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;
	  
	  *UCS++ = 0X3200;// 2;
	  *UCS++ = 0X3000;// 0;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_year & 0xf0) << 4) | 0x3000;//解析年的高四为年的十位;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_year & 0x0f) << 8) | 0x3000;//解析年的高四为年的个位;
	  *UCS++ = YEAR;// 年;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_mon & 0xf0) << 4) | 0x3000;//解析年的高四为月的十位;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_mon & 0x0f) << 8) | 0x3000;//解析年的高四为月的个位;
	  *UCS++ = MONTH;// 月;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_mday & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_mday & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
	  *UCS++ = DAY;// 日;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_hour & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_hour & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
	  *UCS++ = HOUR;// 时;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_min & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_min & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
	  *UCS++ = MINUTE;// 分;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_sec & 0xf0) << 4) | 0x3000;//解析年的高四为日的十位;
	  *UCS++ = ((uint16_t )(device_parameters->calender.tm_sec & 0x0f) << 8) | 0x3000;//解析年的高四为日的个位;
	  *UCS++ = SECOND;// 秒;
	  
	  *UCS_len += 20;
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;

	  //线缆通段状态解析
	  if(device_parameters->signal_parameters.state == RESET)
		{
		  //断开
		  *UCS++ = UCS2_DUAN;
		  *UCS++ = UCS2_KAI;
		}
	  else
		{
		  //连通
		  *UCS++ = UCS2_LIAN;
		  *UCS++ = UCS2_TONG;
		}
	  *UCS++ = POUND_SIGN;
	  *UCS_len += 3;
	  //设备温度
	  if(device_parameters->temperature < 10)
		{
		  *UCS++ = NUM_UCS_MAP[0];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 4;		
					
		}
	  else if(device_parameters->temperature < 100)
		{

		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature / 10];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature % 10];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 4;
		}
	  else if(device_parameters->temperature < 1000)
		{
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature / 100];
		  temp = device_parameters->temperature % 100;
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[temp % 10];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 5;
		}
	  else
		{
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature / 100];
		  temp = device_parameters->temperature % 100;
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[temp % 10];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 5;
		}
						
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  

	  break;
	}
	case 0x41 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->sms_on_off;
	  *UCS++ = NUM_UCS_MAP[temp];
	  (*UCS_len)++;		
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}
	case 0x50 : {
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  UCS = SMS_Send_User_Data_Copy(UCS, \
									device_parameters->gps, \
									UCS_len, \
									GPS_MAX_LENGTH);

	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;

	  break;
	}
	case 0x51 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->signal_parameters.interval;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}
	case 0x52 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->signal_parameters.freq;
	  if(temp < 10)
		{
		  *UCS++ = NUM_UCS_MAP[temp];
		  (*UCS_len)++;
		}
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}
	case 0x53 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->signal_parameters.freq_spread;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}

	  *UCS++ = COMMA_SIGN;
	  (*UCS_len)++;

	  temp = device_parameters->signal_parameters.amp_limit;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}
	case 0x54 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->signal_parameters.process_counts;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}

	case 0x55 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->signal_parameters.process_interval;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}
	case 0x56 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->device_id;
	  if(temp <= 0xff)
		{
		  *UCS++ = NUM_UCS_MAP[temp >> 4];
		  *UCS++ = NUM_UCS_MAP[temp & 0x0f];
		  *UCS_len += 2;
		}
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}
	case 0x57 : {
			
	  UCS = (uint16_t *)(sms_send_pdu_frame->TPDU.TP_UD) + sms_data_length;
	  UCS_len = &(sms_data_length);
	  temp = device_parameters->signal_parameters.send_freq;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}

	  *UCS++ = COMMA_SIGN;
	  (*UCS_len)++;
	  
	  temp = device_parameters->signal_parameters.receive_freq;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}

	  *UCS++ = COMMA_SIGN;
	  (*UCS_len)++;

	  temp = device_parameters->signal_parameters.amp;
	  if(temp <= 3000)
		{
		  *UCS++ = NUM_UCS_MAP[temp / 1000];
		  temp %= 1000;
		  *UCS++ = NUM_UCS_MAP[temp / 100];
		  temp %= 100;		  
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  temp %= 10;
		  *UCS++ = NUM_UCS_MAP[temp];
		  *UCS_len += 4;
		}

	  *UCS++ = COMMA_SIGN;
	  (*UCS_len)++;

	  //设备温度
	  if(device_parameters->temperature < 10)
		{
		  *UCS++ = NUM_UCS_MAP[0];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 4;		
					
		}
	  else if(device_parameters->temperature < 100)
		{

		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature / 10];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature % 10];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 4;
		}
	  else if(device_parameters->temperature < 1000)
		{
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature / 100];
		  temp = device_parameters->temperature % 100;
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[temp % 10];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 5;
		}
	  else
		{
		  *UCS++ = NUM_UCS_MAP[device_parameters->temperature / 100];
		  temp = device_parameters->temperature % 100;
		  *UCS++ = NUM_UCS_MAP[temp / 10];
		  *UCS++ = LINE_SIGN;
		  *UCS++ = NUM_UCS_MAP[temp % 10];
		  *UCS++ = UCS2_DU;
		  *UCS_len += 5;
		}

	  *UCS++ = COMMA_SIGN;
	  (*UCS_len)++;

	  temp = device_parameters->signal_parameters.state;
	  if(temp <= 1)
		{
		  *UCS++ = NUM_UCS_MAP[temp];
		  (*UCS_len)++;
		}
	  
	  *UCS++ = POUND_SIGN;
	  (*UCS_len)++;			
	  break;
	}

	default : 
	  break;
	}
	
  sms_data_length <<= 1;//字节数
  sms_send_pdu_frame->TPDU.TP_DA = *((TP_DA_TYPE *)&(sms_query_mail->TP_OA));//目标电话号码


  OSMutexPend(MUTEX_GSM, 0, &err);
  for(send_index = 0; send_index <= GSM_RESEND_NUMBERS; send_index++)
	{
	  gsm_status = sms_send(sms_send_pdu_frame, sms_data_length);
	
	  if(gsm_status == GSM_SMS_SEND_SUCCESS)
		{
		  break;//发送成功则退出重发机制	  
		}
	  else
		{
		  //发送失败
		  OSTimeDlyHMSM(0, 0, 1, 0);
		}
	}

  OSMutexPost(MUTEX_GSM);
}


static void AppSMSSendTask(void *p_arg)
{
  (void)p_arg;
  uint8_t err;
  SMS_ALARM_FRAME *sms_alarm_mail = NULL;
  SMS_MAIL_FRAME *sms_mail = NULL;
  SMS_QUERY_FRAME *sms_query_mail = NULL;
  SMS_SEND_PDU_FRAME *sms_send_pdu_frame = (SMS_SEND_PDU_FRAME *)SMS_SEND_PROCESS_BUF;
  DEVICE_INIT_PARAMATERS *device_parameters = &device_init_paramaters;

  while(1)
	{
	  OSSemPend(SEM_SMS_OK, 0, &err);//等待短信状态OK信号
	  //OSSemPost(SEM_SMS_OK);//维持短信OK状态，保持使能短信接收任务
	  while(1)
		{

		  //接收短信邮件
		  //sms_alarm_mail = (SMS_ALARM_FRAME *)OSQPend(Q_SMS_ALARM, 30000, &err);
		  sms_mail = (SMS_MAIL_FRAME *)OSQPend(Q_SMS_ALARM, 60000, &err);
			
		  if(err == OS_TIMEOUT)//如果空闲60秒没有邮件则检查SMS状态
			{
			  //OSSemPend(SEM_SMS_OK, 0, &err);//关闭短信OK状态，阻塞短信接受任务
			  OSSemPost(SEM_SMS_FAULT);//启动SMS状态检测任务
			  break;//跳出循环进入等待短信OK状态信号
			}
		  //处理收到的邮件

		  if(sms_mail->sms_mail_type == SMS_ALARM_FRAME_TYPE)
			{
			  //短信告警邮件处理
			  sms_alarm_mail = &(sms_mail->sms_alarm_frame);
			  sms_alarm_mail_analysis(sms_send_pdu_frame, sms_alarm_mail, device_parameters);
			}
		  else if(sms_mail->sms_mail_type == SMS_QUERY_FRAME_TYPE)
			{
			  //短信查询邮件处理
			  sms_query_mail = &(sms_mail->sms_query_frame);
			  sms_query_mail_analysis(sms_send_pdu_frame, sms_query_mail, device_parameters);
			}
		  else
			{
			  //其他短信邮件处理
			}
			
		  //短信邮件处理完成释放短信邮件所占用的空间
		  OSMemPut(MEM_BUF, (void *)sms_mail);
		  OSSemPost(SEM_MEM_PART_ALLOC);



		}
		
	}
}


// mthomas: added section -> alignment thru linker-script 

__attribute__ ((section(".isr_vector")))
pfn_ISR VectorArray[] = {
  //  (pfn_ISR)(((unsigned long)Stack + sizeof(Stack) - 1) & 0xFFFFFFF8),	// The initial stack pointer,
  (pfn_ISR)(((unsigned long)Stack + sizeof(Stack))),	// The initial stack pointer,
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
};

static void AppSignalTask(void *p_arg)
{
  (void)p_arg;
  uint8_t err;
  uint16_t index = 0;
  uint32_t signal_send_freq = 0;
  uint32_t signal_receive_freq = 0;
  uint32_t signal_freq_spread = 0;
  uint32_t signal_amp = 0;
  
  DEVICE_INIT_PARAMATERS *device_parameters = &device_init_paramaters;
  SMS_MAIL_FRAME *sms_mail = NULL;
  SMS_ALARM_FRAME *sms_alarm_mail = NULL;
  
  while(1)
	{
	  if (device_parameters->signal_parameters.interval != 0)
		{
		  OSTimeDlyHMSM(0, device_parameters->signal_parameters.interval / 60, \
						device_parameters->signal_parameters.interval / 60, 0);
		  
		  //打开信号收发电源
		  signal_receive_power(ENABLE);//接收电源打开
		  OSTimeDlyHMSM(0, 0, 0, 50);
		  signal_send_power(ENABLE);//发送电源打开
		  signal_frequency_set(device_parameters->signal_parameters.freq);//信号
		  signal_send_init();//信号发送初始化
		  signal_send(ENABLE);//信号发送
		  signal_freq_test(ENABLE);//信号频率检测
		  OSTimeDlyHMSM(0, 0, 3, 0);//等待信号稳定
		  //处理信号频率及幅值
		  signal_send_freq = 0;
		  signal_receive_freq = 0;
		  signal_amp = 0;
		  //由于参数需要作被除数所以确保这个数不为0
		  if(device_parameters->signal_parameters.process_counts < 1)
			{
			  device_parameters->signal_parameters.process_counts = 1;
			}

	  
		  for(index = 0; index < device_parameters->signal_parameters.process_counts; index++)
			{
			  signal_send_freq += signal_send_freq_capture;
			  signal_receive_freq += signal_receive_freq_capture;
			  signal_amp += adc1_buf[SIGNAL_AMP];
			  OSTimeDlyHMSM(0, 0, 0, device_parameters->signal_parameters.process_interval);
			}
		  signal_send_freq /= device_parameters->signal_parameters.process_counts;
		  signal_receive_freq /= device_parameters->signal_parameters.process_counts;
		  signal_amp /= device_parameters->signal_parameters.process_counts;

		  device_parameters->signal_parameters.send_freq = signal_send_freq;
		  device_parameters->signal_parameters.receive_freq = signal_receive_freq;
		  device_parameters->signal_parameters.amp = signal_amp;
	  
		  if(signal_send_freq >= signal_receive_freq)
			{
			  signal_freq_spread = signal_send_freq - signal_receive_freq;
		  
			}
		  else
			{
			  signal_freq_spread = signal_receive_freq - signal_send_freq;
			}

		  //信号断开	  
		  if(signal_freq_spread > device_parameters->signal_parameters.freq_spread \
			 || signal_amp < device_parameters->signal_parameters.amp_limit)
			{
			  led_off(LED_SIGNAL_STATE);
			  //信号的历史状态为连通
			  if(device_parameters->signal_parameters.state == SET)
				{
				  //由通变成断则发送告警邮件
				  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
				  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
				  sms_mail->sms_mail_type = SMS_ALARM_FRAME_TYPE;
				  sms_alarm_mail = &(sms_mail->sms_alarm_frame);							
				  sms_alarm_mail->temperature = device_parameters->temperature;
				  sms_alarm_mail->state = RESET;
				  sms_alarm_mail->time = device_parameters->calender;

				  if(device_parameters->sms_on_off == 1)
					{
					  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
					}
				  else
					{
					  //短信邮件处理完成释放短信邮件所占用的空间
					  OSMemPut(MEM_BUF, (void *)sms_mail);
					  OSSemPost(SEM_MEM_PART_ALLOC);
					}
				}
			  else
				{
		
				}
			  device_parameters->signal_parameters.state = RESET;//设置信号状态为断开


			}
		  //信号连通
		  else
			{
			  led_on(LED_SIGNAL_STATE);
			  //信号的历史状态为连通
			  if(device_parameters->signal_parameters.state == SET)
				{

				}
			  else
				{
				  //由断变成通则发送告警邮件	  
				  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
				  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
				  sms_mail->sms_mail_type = SMS_ALARM_FRAME_TYPE;
				  sms_alarm_mail = &(sms_mail->sms_alarm_frame);							
				  sms_alarm_mail->temperature = device_parameters->temperature;
				  sms_alarm_mail->state = SET;
				  sms_alarm_mail->time = device_parameters->calender;

				  if(device_parameters->sms_on_off == 1)
					{
					  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
					}
				  else
					{
					  //短信邮件处理完成释放短信邮件所占用的空间
					  OSMemPut(MEM_BUF, (void *)sms_mail);
					  OSSemPost(SEM_MEM_PART_ALLOC);					  
					}
			  
				}
			  device_parameters->signal_parameters.state = SET;//设置信号状态为连通
			}
		  signal_freq_test(DISABLE);//信号频率检测
		  signal_send(DISABLE);//信号发送
		  //关闭信号收发电源
		  signal_send_power(DISABLE);
		  signal_receive_power(DISABLE);
		  
		}
	  else
		{
		  //打开信号收发电源
		  signal_receive_power(ENABLE);//接收电源打开
		  OSTimeDlyHMSM(0, 0, 0, 50);
		  signal_send_power(ENABLE);//发送电源打开
		  signal_frequency_set(device_parameters->signal_parameters.freq);//信号
		  signal_send_init();//信号发送初始化
		  signal_send(ENABLE);//信号发送
		  signal_freq_test(ENABLE);//信号频率检测
		  OSTimeDlyHMSM(0, 0, 3, 0);//等待信号稳定
		  while(1)
			{
			  //处理信号频率及幅值
			  signal_send_freq = 0;
			  signal_receive_freq = 0;
			  signal_amp = 0;
			  //由于参数需要作被除数所以确保这个数不为0
			  if(device_parameters->signal_parameters.process_counts < 1)
				{
				  device_parameters->signal_parameters.process_counts = 1;
				}
			  for(index = 0; index < device_parameters->signal_parameters.process_counts; index++)
				{
				  signal_send_freq += signal_send_freq_capture;
				  signal_receive_freq += signal_receive_freq_capture;
				  signal_amp += adc1_buf[SIGNAL_AMP];
				  OSTimeDlyHMSM(0, 0, device_parameters->signal_parameters.process_interval / 1000, \
								device_parameters->signal_parameters.process_interval % 1000);
				}
			  signal_send_freq /= device_parameters->signal_parameters.process_counts;
			  signal_receive_freq /= device_parameters->signal_parameters.process_counts;
			  signal_amp /= device_parameters->signal_parameters.process_counts;

			  device_parameters->signal_parameters.send_freq = signal_send_freq;
			  device_parameters->signal_parameters.receive_freq = signal_receive_freq;
			  device_parameters->signal_parameters.amp = signal_amp;
	  
			  if(signal_send_freq >= signal_receive_freq)
				{
				  signal_freq_spread = signal_send_freq - signal_receive_freq;
				}
			  else
				{
				  signal_freq_spread = signal_receive_freq - signal_send_freq;
				}
			  //信号断开	  
			  if(signal_freq_spread > device_parameters->signal_parameters.freq_spread \
				 || signal_amp < device_parameters->signal_parameters.amp_limit)
				{
				  led_off(LED_SIGNAL_STATE);
				  //信号的历史状态为连通
				  if(device_parameters->signal_parameters.state == SET)
					{
					  //由通变成断则发送告警邮件
					  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
					  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
					  sms_mail->sms_mail_type = SMS_ALARM_FRAME_TYPE;
					  sms_alarm_mail = &(sms_mail->sms_alarm_frame);							
					  sms_alarm_mail->temperature = device_parameters->temperature;
					  sms_alarm_mail->state = RESET;
					  sms_alarm_mail->time = device_parameters->calender;

					  if(device_parameters->sms_on_off == 1)
						{
						  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
						}
					  else
						{
						  //短信邮件处理完成释放短信邮件所占用的空间
						  OSMemPut(MEM_BUF, (void *)sms_mail);
						  OSSemPost(SEM_MEM_PART_ALLOC);

						}
					}
				  else
					{
		
					}
				  device_parameters->signal_parameters.state = RESET;//设置信号状态为断开
				}
			  //信号连通
			  else
				{
				  led_on(LED_SIGNAL_STATE);
				  //信号的历史状态为连通
				  if(device_parameters->signal_parameters.state == SET)
					{

					}
				  else
					{
					  //由断变成通则发送告警邮件
					  OSSemPend(SEM_MEM_PART_ALLOC, 0, &err);
					  sms_mail = (SMS_MAIL_FRAME *)OSMemGet(MEM_BUF, &err);
					  sms_mail->sms_mail_type = SMS_ALARM_FRAME_TYPE;
					  sms_alarm_mail = &(sms_mail->sms_alarm_frame);							
					  sms_alarm_mail->temperature = device_parameters->temperature;
					  sms_alarm_mail->state = SET;
					  sms_alarm_mail->time = device_parameters->calender;

					  if(device_parameters->sms_on_off == 1)
						{
						  OSQPostFront(Q_SMS_ALARM, (void *)sms_mail);
						}
					  else
						{
						  //短信邮件处理完成释放短信邮件所占用的空间
						  OSMemPut(MEM_BUF, (void *)sms_mail);
						  OSSemPost(SEM_MEM_PART_ALLOC);
						}
			  
					}
				  device_parameters->signal_parameters.state = SET;//设置信号状态为连通

				}
			  OSTimeDlyHMSM(0, 0, 1, 0);
			}
		  signal_freq_test(DISABLE);//信号频率检测
		  signal_send(DISABLE);//信号发送
		  //关闭信号收发电源
		  signal_send_power(DISABLE);
		  signal_receive_power(DISABLE);
		}
	}
}


typedef struct {
  
  uint16_t address;
  uint16_t length;
  uint16_t offset;
  void *data;
  
}RS485_ADDRESS_INFO;


#define nr_of_array(x) (sizeof((x)) / sizeof((x)[0]))

uint8_t function_id[] = {0x04, 0x10};
int comp_function_id(const void *m1, const void *m2)
{
  uint8_t *mi1 = (uint8_t *)m1;
  uint8_t *mi2 = (uint8_t *)m2;
  if(*mi1 > *mi2)
	{
	  return 1;
	}
  else if(*mi1 == *mi2)
	{
	  return 0;
	}
  else
	{
	  return -1;
	}
}

int comp_rs485_address_info(const void *m1, const void *m2)
{
  RS485_ADDRESS_INFO *mi1 = (RS485_ADDRESS_INFO *)m1;
  RS485_ADDRESS_INFO *mi2 = (RS485_ADDRESS_INFO *)m2;
  if(mi1->address > mi2->address)
	{
	  return 1;
	}
  else if(mi1->address == mi2->address)
	{
	  return 0;
	}
  else
	{
	  return -1;
	}
}



RS485_ADDRESS_INFO rs485_address_info[] = {
  {
	0x1100, 32,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, primary_device_name), &(device_init_paramaters.primary_device_name[0]),
  },
  {
	0x1200,	32,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, slave_device_name), &(device_init_paramaters.slave_device_name[0]),
  },
  {
	0x2100,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone_numbers), &(device_init_paramaters.alarm_telephone_numbers),
  },
  {
	0x2200,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[0]), &(device_init_paramaters.alarm_telephone[0]),
  },
  {
	0x2300,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[1]), &(device_init_paramaters.alarm_telephone[1]),
  },
  {
	0x2400,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[2]), &(device_init_paramaters.alarm_telephone[2]),
  },
  {
	0x2500,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[3]), &(device_init_paramaters.alarm_telephone[3]),
  },
  {
	0x2600,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[4]), &(device_init_paramaters.alarm_telephone[4]),
  },
  {
	0x2700,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[5]), &(device_init_paramaters.alarm_telephone[5]),
  },
  {
	0x2800,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[6]), &(device_init_paramaters.alarm_telephone[6]),
  },
  {
	0x2900,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[7]), &(device_init_paramaters.alarm_telephone[7]),
  },
  {
	0x3000,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[8]), &(device_init_paramaters.alarm_telephone[8]),
  },
  {
	0x3100,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, alarm_telephone[9]), &(device_init_paramaters.alarm_telephone[9]),
  },
  {
	0x3200,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, service_center_address), &(device_init_paramaters.service_center_address[0]),
  },
  {
	0x4100,	12,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, password), &(device_init_paramaters.password[0]),
  },
  {
	0x4200,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, device_id), &(device_init_paramaters.device_id),
  },
  {
	0x4300,	36,	0, &(device_init_paramaters.calender),
  },
  {
	0x5100,	32,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, gps), &(device_init_paramaters.gps[0]),
  },
  {
	0x6100,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, sms_on_off), &(device_init_paramaters.sms_on_off),
  },
  {
	0x7100,
	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, interval),
	&(device_init_paramaters.signal_parameters.interval),
  },
  {
	0x7200,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, freq),
	&(device_init_paramaters.signal_parameters.freq),
  },
  {
	0x7300,	4,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, freq_spread),
	&(device_init_paramaters.signal_parameters.freq_spread),
  },
  {
	0x7400,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, amp_limit),
	&(device_init_paramaters.signal_parameters.amp_limit),
  },
  {
	0x7500,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, process_counts),
	&(device_init_paramaters.signal_parameters.process_counts),
  },
  {
	0x7600,	2,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, process_interval),
	&(device_init_paramaters.signal_parameters.process_interval),
  },
  {
	0x7700,	10,
	OFF_SET_OF(DEVICE_INIT_PARAMATERS, signal_parameters) + OFF_SET_OF(SignalParameters, send_freq),
	&(device_init_paramaters.signal_parameters.send_freq),
  },
  {
	0x7900,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, rs485_baudrate), &(device_init_paramaters.rs485_baudrate),
  },
  {
	0x8000,	2,	OFF_SET_OF(DEVICE_INIT_PARAMATERS, gsm_signal_strength), &(device_init_paramaters.gsm_signal_strength),
  },
};


RS485_REQUEST_FRAME *receive_rs485_frame(RS485_REQUEST_FRAME *request_frame, DEVICE_INIT_PARAMATERS *device_parameters)
{
  RS485_ADDRESS_INFO key, *res;
  void *temp = NULL;
  uint16_t crc = 0;
  crc_16_init();
  //接收device_id字节
  if(receive_from_rs485((char *)&(request_frame->device_id), 1) == 1)
	{
	  //处理device_id
	  if(request_frame->device_id == device_parameters->device_id)
		{
		  crc = crc_16((uint8_t *)&(request_frame->device_id), 1);
		  //OSTimeDlyHMSM(0, 0, 0, 1);
		  //接收function_id
		  if(receive_from_rs485((char *)&(request_frame->function_id), 1) == 1)
			{
			  crc = crc_16((uint8_t *)&(request_frame->function_id), 1);
			  //处理function_id
			  temp = (void *)bsearch((void *)&(request_frame->function_id),
									 (void *)&function_id[0],
									 nr_of_array(function_id),
									 sizeof(function_id[0]),
									 comp_function_id);
			  if(temp != NULL)
				{
				  //接收地址码
				  if(receive_from_rs485((char *)&(request_frame->address), 2) == 2)
					{
					  crc = crc_16((uint8_t *)&(request_frame->address), 2);
					  //处理地址码
					  request_frame->address = __REV16(request_frame->address);
					  //					  request_frame->address = (request_frame->address >> 8) | (request_frame->address << 8);
					  key.address = request_frame->address;
					  res = (RS485_ADDRESS_INFO *)bsearch((void *)&key,
														  (void *)&rs485_address_info[0],
														  nr_of_array(rs485_address_info),
														  sizeof(rs485_address_info[0]),
														  comp_rs485_address_info);
					  if(res != NULL)
						{
						  //接收数据长度
						  if(receive_from_rs485((char *)&(request_frame->length_16), 2) == 2)
							{
							  crc = crc_16((uint8_t *)&(request_frame->length_16), 2);
							  //处理数据长度
							  request_frame->length_16 = __REV16(request_frame->length_16);
							  if(request_frame->length_16 <= 100)
								{
								  switch(request_frame->function_id)
									{
									case 0x04 : {
									  //接收CRC码
									  if(receive_from_rs485((char *)&(request_frame->rs485_read_request_frame.crc), 2) == 2)
										{
										  crc = crc_16((uint8_t *)&(request_frame->rs485_read_request_frame.crc), 2);
										  //校验帧CRC
										  if(crc == 0x0)
											{
											  return request_frame;
											}
										  else
											{
											  return NULL;
											}
										}
									  else
										{
										  return NULL;
										}
									  break;
									}
									case 0x10 : {
									  if((receive_from_rs485((char *)&(request_frame->rs485_set_request_frame.length_8), 1) == 1))
										{
										  crc = crc_16((uint8_t *)&(request_frame->rs485_set_request_frame.length_8), 1);
										  if((receive_from_rs485((char *)&(request_frame->rs485_set_request_frame.data),
																request_frame->rs485_set_request_frame.length_8) \
											  == request_frame->rs485_set_request_frame.length_8))
											{
											  crc = crc_16((uint8_t *)&(request_frame->rs485_set_request_frame.data), request_frame->rs485_set_request_frame.length_8);//校验帧CRC
											  if(receive_from_rs485((char *)&(request_frame->rs485_set_request_frame.crc), 2) == 2)
												{
												  crc = crc_16((uint8_t *)&(request_frame->rs485_set_request_frame.crc), 2);//校验帧CRC
												  if(crc == 0x0)
													{
													  return request_frame;
													}
												  else
													{
													  return NULL;
													}
												}
											  else
												{
												  return NULL;
												}

											}
										  else
											{
											  return NULL;
											}
										}
									  else
										{
										  return NULL;
										}
									  break;
									}
									default:{
									  
									  break;
									}
									}
								  return NULL;
								}
							  else
								{
								  return NULL;
								}
							}
						  else
							{
							  return NULL;
							}
						  
						}
					  else
						{
						  return NULL;
						}
					}
				  else
					{
					  return NULL;
					}
				}
			  else
				{
				  return NULL;
				}
			}
		  else
			{
			  return NULL;
			}
		}
	  else
		{
		  return NULL;
		}
	  
	}
  else
	{
	  return NULL;
	}
}

static void AppRS485Task(void *p_arg)
{
  (void)p_arg;
  uint8_t err;
  uint8_t length = 0;
  uint8_t index = 0;
  //  uint16_t crc;
  
  RS485_ADDRESS_INFO key, *res;
  DEVICE_INIT_PARAMATERS *device_parameters = &device_init_paramaters;
  RS485_REQUEST_FRAME *rs_request_frame = NULL;
  RS485_RESPONSE_FRAME *rs_response_frame = (RS485_RESPONSE_FRAME *)RS485_SEND_BUF;
  uint16_t *temp = NULL;
  int *time = NULL;
  
  
  
  while(1)
	{
	  if(CHARS(rs485_buf) >= 1)
		{
		  OSTimeDlyHMSM(0, 0, 0, 200);
		  rs_request_frame = receive_rs485_frame((RS485_REQUEST_FRAME *)RS485_RECEIVE_BUF,
												 device_parameters);
		  if(rs_request_frame != NULL)
			{
			  led_on(LED_RS485);
			  crc_16_init();
			  rs_response_frame->device_id = rs_request_frame->device_id;
			  crc_16((uint8_t *)&(rs_response_frame->device_id), 1);
			  rs_response_frame->function_id = rs_request_frame->function_id;
			  crc_16((uint8_t *)&(rs_response_frame->function_id), 1);
			  switch(rs_request_frame->function_id)
				{
				case 0x04 : {
				  rs_response_frame->rs485_read_response_frame.length_8 = rs_request_frame->length_16 << 1;
				  crc_16((uint8_t *)&(rs_response_frame->rs485_read_response_frame.length_8), 1);
				  key.address = rs_request_frame->address;
				  res = (RS485_ADDRESS_INFO *)bsearch((void *)&key,
													  (void *)&rs485_address_info[0],
													  nr_of_array(rs485_address_info),
													  sizeof(rs485_address_info[0]),
													  comp_rs485_address_info);
				  if(res != NULL)
					{
					  memset(RS485_PROCESS_BUF, 0, res->length + 1);
					  memcpy(RS485_PROCESS_BUF, res->data, res->length);
					  length = res->length >> 1;
					  temp = (uint16_t *)RS485_PROCESS_BUF;
					  for(index = 0; index < length; index++)
						{
						  *temp = __REV16(*temp);
						  temp++;

						}

					  memcpy(rs_response_frame->rs485_read_response_frame.data, RS485_PROCESS_BUF, res->length);
					  rs_response_frame->crc = crc_16((uint8_t *)&(rs_response_frame->rs485_read_response_frame.data), res->length);
					  rs_response_frame->crc = __REV16(rs_response_frame->crc);
					  OSMutexPend(MUTEX_RS485, 0, &err);

					  rs485_dir_set(ENABLE);
					  send_to_rs485((char *)&(rs_response_frame->device_id), 1);
					  send_to_rs485((char *)&(rs_response_frame->function_id), 1);
					  send_to_rs485((char *)&(rs_response_frame->rs485_read_response_frame.length_8), 1);
					  send_to_rs485((char *)&(rs_response_frame->rs485_read_response_frame.data), rs_response_frame->rs485_read_response_frame.length_8);
					  send_to_rs485((char *)&(rs_response_frame->crc), 2);
					  OSTimeDlyHMSM(0, 0, 0, 50);
					  rs485_dir_set(DISABLE);
				  
					  OSMutexPost(MUTEX_RS485);
					}
				  else
					{

					}
				  break;
				}
				case 0x10 : {
				  rs_response_frame->rs485_set_response_frame.address = __REV16(rs_request_frame->address);
				  crc_16((uint8_t *)&(rs_response_frame->rs485_set_response_frame.address), 2);
				  rs_response_frame->rs485_set_response_frame.length_16 = __REV16(rs_request_frame->length_16);
				  rs_response_frame->crc = crc_16((uint8_t *)&(rs_response_frame->rs485_set_response_frame.length_16), 2);
				  rs_response_frame->crc = __REV16(rs_response_frame->crc);
				  key.address = rs_request_frame->address;
				  res = (RS485_ADDRESS_INFO *)bsearch((void *)&key,
													  (void *)&rs485_address_info[0],
													  nr_of_array(rs485_address_info),
													  sizeof(rs485_address_info[0]),
													  comp_rs485_address_info);
				  if(res != NULL)
					{
					  memset(RS485_PROCESS_BUF, 0, res->length);
					  memcpy(RS485_PROCESS_BUF, rs_request_frame->rs485_set_request_frame.data, res->length);
					  length = res->length >> 1;
					  temp = (uint16_t *)RS485_PROCESS_BUF;

					  for(index = 0; index < length; index++)
						{
						  *temp = __REV16(*temp);
						  temp++;
						}
					  if(rs_request_frame->address == 0x4300)
						{
					  
						  OSMutexPend(MUTEX_CALENDER, 0, &err);
						  time = (int *)RS485_RECEIVE_BUF;
						  device_parameters->calender.tm_sec = *time++;
						  device_parameters->calender.tm_min = *time++;
						  device_parameters->calender.tm_hour = *time++;
						  device_parameters->calender.tm_mday = *time++;
						  device_parameters->calender.tm_mon = *time++;
						  device_parameters->calender.tm_year = *time++;
						  device_parameters->calender.tm_wday = 0;
						  device_parameters->calender.tm_yday = 0;
						  device_parameters->calender.tm_isdst = 0;
						  calender_set(&(device_parameters->calender));
						  OSMutexPost(MUTEX_CALENDER);

						}
					  else
						{
						  memcpy(res->data, RS485_PROCESS_BUF, res->length);

						  OSMutexPend(MUTEX_SFLASH, 0, &err);
						  /*
							Iron_Write(IRON_SERVICE_CENTER_ADDRESS, \
							device_init_paramaters.service_center_address, \
							ALARM_TELEPHONE_NUMBER_SIZE);*/
						  sFLASH_WriteBuffer(res->data, \
											 SFLASH_DEVICE_INIT_PARAMATERS_START +	\
											 res->offset, \
											 res->length);
						  OSMutexPost(MUTEX_SFLASH);

						}
					  OSMutexPend(MUTEX_RS485, 0, &err);

					  rs485_dir_set(ENABLE);
					  send_to_rs485((char *)&(rs_response_frame->device_id), 1);
					  send_to_rs485((char *)&(rs_response_frame->function_id), 1);
					  send_to_rs485((char *)&(rs_response_frame->rs485_set_response_frame.address), 2);
					  send_to_rs485((char *)&(rs_response_frame->rs485_set_response_frame.length_16), 2);
					  send_to_rs485((char *)&(rs_response_frame->crc), 2);
					  OSTimeDlyHMSM(0, 0, 0, 50);
					  rs485_dir_set(DISABLE);
				  
					  OSMutexPost(MUTEX_RS485);

					}
				  else
					{

					}
				  break;
				}
				}
			  led_off(LED_RS485);

			}
		  else
			{
			  OSTimeDlyHMSM(0, 0, 0, 200);
			}
		  
		}
	  else
		{
		  OSTimeDlyHMSM(0, 0, 0, 200);
		}
	}
}

