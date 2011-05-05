/*********************************************************************
 * Filename:      sflash.c
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Wed Apr 27 13:28:35 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Thu May  5 17:24:38 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#include "includes.h"

#define sFLASH_SPI                       SPI2
#define sFLASH_SPI_CLK                   RCC_APB1Periph_SPI2
#define sFLASH_SPI_SCK_PIN               GPIO_Pin_13                 // PB.13
#define sFLASH_SPI_SCK_GPIO_PORT         GPIOB                       // GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOB
#define sFLASH_SPI_MISO_PIN              GPIO_Pin_14                 // PB.14
#define sFLASH_SPI_MISO_GPIO_PORT        GPIOB                       // GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOB
#define sFLASH_SPI_MOSI_PIN              GPIO_Pin_15                 // PB.15
#define sFLASH_SPI_MOSI_GPIO_PORT        GPIOB                       // GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOB
#define sFLASH_CS_PIN                    GPIO_Pin_12                 // PB.12
#define sFLASH_CS_GPIO_PORT              GPIOB                       // GPIOB
#define sFLASH_CS_GPIO_CLK               RCC_APB2Periph_GPIOB


#define sFLASH_CMD_WRITE          0x82  // Write to Memory instruction

#define sFLASH_CMD_READ           0x03  // Read from Memory instruction
#define sFLASH_CMD_RDSR           0xD7  // Read Status Register instruction
#define sFLASH_CMD_RDID           0x9F  // Read identification

#define sFLASH_CMD_PE             0x81  // Page Erase instruction
#define sFLASH_CMD_SE             0x7C  // Sector Erase instruction
#define sFLASH_CMD_BE             0x50  // Chip Erase instruction
#define sFLASH_CMD_CE             0x9A8094C7  // Chip Erase instruction

#define sFLASH_CMD_DP             0xB9  // Deep power mode
#define sFLASH_CMD_DP_RS          0xAB  // Resume deep power mode

#define sFLASH_CMD_PAGE_512      0xA6802A3D // Set 512 bytes page size 

#define sFLASH_BUSY_FLAG           0x80  // Busy flag mask
#define sFLASH_PAGE_FLAG           0x01  // Page flag mask

#define sFLASH_DUMMY_BYTE         0xA5
#define sFLASH_SPI_PAGESIZE       0x200  // Page size is 512 bytes

#define sFLASH_M25P128_ID         0x202018
#define sFLASH_M25P64_ID          0x202017

#define sFLASH_CS_LOW()       GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN) // sFLASH chip select
#define sFLASH_CS_HIGH()      GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN) // sFLASH chip deselect

static void sFLASH_WaitForWriteEnd(void);
static uint16_t sFLASH_SendHalfWord(uint16_t HalfWord);
static uint8_t sFLASH_SendByte(uint8_t byte);
static uint8_t sFLASH_ReadByte(void);
static void sFLASH_StartReadSequence(uint32_t ReadAddr);
static void sFLASH_LowLevel_DeInit(void);
static void sFLASH_LowLevel_Init(void);

/*
 * Function sFLASH_DeInit ()
 *
 *    sFLASH 关闭初始化
 *
 */
void sFLASH_DeInit(void)
{

  sFLASH_LowLevel_DeInit();

}

/*
 * Function sFLASH_Init ()
 *
 *    sFLASH 初始化
 *    1. SPI 初始化
 *
 */
void sFLASH_Init(void)
{

  SPI_InitTypeDef  SPI_InitStructure;

  sFLASH_LowLevel_Init();
  sFLASH_CS_HIGH();// Deselect the FLASH: Chip Select high

  // SPI configuration
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
#else
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
#endif

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sFLASH_SPI, &SPI_InitStructure);

  SPI_Cmd(sFLASH_SPI, ENABLE);//Enable the sFLASH_SPI

}
void sFLASH_DP(void)
{
  sFLASH_CS_LOW();
  sFLASH_SendByte(sFLASH_CMD_DP);
  sFLASH_CS_HIGH();
}
void sFLASH_DP_RS(void)
{
  sFLASH_CS_LOW();
  sFLASH_SendByte(sFLASH_CMD_DP_RS);
  sFLASH_CS_HIGH();
}
void sFLASH_PageSizeSet(void)
{
/*!< Select the FLASH: Chip Select low */
  uint8_t flashstatus = 0;
  sFLASH_CS_LOW();
  sFLASH_SendByte(sFLASH_CMD_RDSR);
  flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
  sFLASH_CS_HIGH();
  /*!< Loop as long as the memory is busy with a write cycle */
  // Write in progress
  while ((flashstatus & sFLASH_PAGE_FLAG) != 0x01)
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
	  and put the value of the status register in FLASH_Status variable */
	sFLASH_CS_LOW();
	/*!< Send "RDID " instruction */
	sFLASH_SendByte((uint8_t)sFLASH_CMD_PAGE_512);
	sFLASH_SendByte((uint8_t)(sFLASH_CMD_PAGE_512 >> 8));
	sFLASH_SendByte((uint8_t)(sFLASH_CMD_PAGE_512 >> 16));
	sFLASH_SendByte((uint8_t)(sFLASH_CMD_PAGE_512 >> 24));
	/*!< Send "Read Status Register" instruction */
	sFLASH_CS_HIGH();
	sFLASH_WaitForWriteEnd();// Wait the end of Flash writing
	sFLASH_CS_LOW();
	/*!< Send "RDID " instruction */
	sFLASH_SendByte(sFLASH_CMD_RDSR);
	flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	sFLASH_CS_HIGH();
	sFLASH_WaitForWriteEnd();// Wait the end of Flash writing
  }
  
}

/*
 * Function sFLASH_WriteBuffer (pBuffer, WriteAddr, NumByteToWrite)
 *
 *    Write a buffer to sFLASH
 *
 */
static void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  sFLASH_CS_LOW();  // Select the FLASH: Chip Select low
  sFLASH_SendByte(sFLASH_CMD_WRITE);  // Send "Write to Memory " instruction

  sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);  // Send WriteAddr high nibble address byte to write to
  sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);  // Send WriteAddr medium nibble address byte to write to
  sFLASH_SendByte(WriteAddr & 0xFF);// Send WriteAddr low nibble address byte to write to

  while (NumByteToWrite--)  // while there is data to be written on the FLASH
	{
	  sFLASH_SendByte(*pBuffer);	  // Send the current byte
	  pBuffer++;// Point on the next byte to be written
	}

  sFLASH_CS_HIGH();//  Deselect the FLASH: Chip Select high
  sFLASH_WaitForWriteEnd();// Wait the end of Flash writing
}

void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % sFLASH_SPI_PAGESIZE;
  count = sFLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        sFLASH_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        sFLASH_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

      sFLASH_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/*
 * Function sFLASH_ReadBuffer (pBuffer, ReadAddr, NumByteToRead)
 *
 *    Write a buffer to sFLASH
 *
 */
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_READ);

  /*!< Send ReadAddr high nibble address byte to read from */
  sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  sFLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}

/*
 * Function sFLASH_ReadID ()
 *
 *    Read sFLASH ID
 *
 */
uint32_t sFLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "RDID " instruction */
  sFLASH_SendByte(sFLASH_CMD_RDID);

  /*!< Read a byte from the FLASH */
  Temp0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/*
 * Function sFLASH_ReadByte ()
 *
 *    Receive a byte data from sFLASH.
 *
 */
static uint8_t sFLASH_ReadByte(void)
{
  return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
}

/*
 * Function sFLASH_SendByte (byte)
 *
 *    Send a byte to sFLASH return a received byte.
 *
 */
static uint8_t sFLASH_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sFLASH_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(sFLASH_SPI);
}

/*
 * Function sFLASH_WaitForWriteEnd ()
 *
 *    Waiting for sFlash idle
 *
 */
static void sFLASH_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sFLASH_SendByte(sFLASH_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

  }
  while ((flashstatus & sFLASH_BUSY_FLAG) != 0x80); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();
}


/*
 * Function sFLASH_LowLevel_DeInit ()
 *
 *    sFLASH 底层关闭初始化
 *    1. GPIO时钟,SPI2时钟
 *    2. GPIO配置
 *
 */
static void sFLASH_LowLevel_DeInit(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;

  // Disable the sFLASH_SPI
  SPI_Cmd(sFLASH_SPI, DISABLE);

  // DeInitializes the sFLASH_SPI
  SPI_I2S_DeInit(sFLASH_SPI);
  
  // sFLASH_SPI Periph clock disable
  RCC_APB1PeriphClockCmd(sFLASH_SPI_CLK, DISABLE);

  // Configure sFLASH_SPI pins: SCK
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  // Configure sFLASH_SPI pins: MISO
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  // Configure sFLASH_SPI pins: MOSI
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  // Configure sFLASH_CS_PIN pin: sFLASH Card CS pin
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);

}

/*
 * Function sFLASH_LowLevel_Init ()
 *
 *    sFLASH 底层初始化
 *    1. GPIO时钟,SPI2时钟
 *    2. GPIO配置
 *
 */
static void sFLASH_LowLevel_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;

  // sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO 
  // and sFLASH_SPI_SCK_GPIO Periph clock enable
  RCC_APB2PeriphClockCmd(sFLASH_CS_GPIO_CLK | sFLASH_SPI_MOSI_GPIO_CLK | \
						 sFLASH_SPI_MISO_GPIO_CLK |	sFLASH_SPI_SCK_GPIO_CLK, \
						 ENABLE);

  // sFLASH_SPI Periph clock enable
  RCC_APB1PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);

  // Configure sFLASH_SPI pins: SCK
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  // Configure sFLASH_SPI pins: MOSI
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
  
  // Configure sFLASH_SPI pins: MISO
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  // Configure sFLASH_CS_PIN pin: sFLASH Card CS pin
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);

}

