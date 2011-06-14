/*********************************************************************
 * Filename:      sflash.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu Apr 28 11:54:00 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Tue Jun 14 16:04:36 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __SFLASH_H__
#define __SFLASH_H__

#define OFF_SET(addr, type, step)		((addr) + (step) * sizeof(type))
#define OFF_SET_OF(TYPE, MEMBER) 		((uint32_t)&((TYPE *)0)->MEMBER)

#define SFLASH_DEVICE_INIT_PARAMATERS_START		0
#define SFLASH_DEVICE_INIT_PARAMATERS_END \
  OFF_SET(SFLASH_DEVICE_INIT_PARAMATERS_START, DEVICE_INIT_PARAMATERS, 1)

void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
uint32_t sFLASH_ReadID(void);
void sFLASH_PageSizeSet(void);
void sFLASH_Init(void);
void sFLASH_DeInit(void);
void sFLASH_DP_RS(void);
void sFLASH_DP(void);

//#define sflash_read(addr, data, len) sFLASH_ReadBuffer((data), (addr), (len))
//#define sflash_write(addr, data, len) sFLASH_ReadBuffer((data), (addr), (len))

#endif
