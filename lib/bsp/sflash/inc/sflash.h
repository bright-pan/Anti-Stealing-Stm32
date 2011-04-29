/*********************************************************************
 * Filename:      sflash.h
 * Version:       
 *                
 * Author:        Bright Pan <loststriker@gmail.com>
 * Created at:    Thu Apr 28 11:54:00 2011
 *                
 *                
 * Modified by:   Bright Pan <loststriker@gmail.com>
 * Modified at:   Fri Apr 29 14:07:20 2011
 *                
 * Description:   
 * Copyright (C) 2010-2011,  Bright Pan
 ********************************************************************/

#ifndef __SFLASH_H__
#define __SFLASH_H__

void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
uint32_t sFLASH_ReadID(void);
void sFLASH_PageSizeSet(void);
void sFLASH_Init(void);
void sFLASH_DeInit(void);
void sFLASH_DP_RS(void);
void sFLASH_DP(void);

#endif
