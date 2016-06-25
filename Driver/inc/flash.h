/**
  ******************************************************************************
  * @file    flash.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2015.8.28
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_IFLASH_H__
#define __CH_LIB_IFLASH_H__

#include <stdint.h>
#include <string.h>
#include "common.h"

/* function return type */
#define FLASH_OK                    0x00
#define FLASH_OVERFLOW              0x01
#define FLASH_BUSY                  0x02
#define FLASH_ERROR                 0x04
#define FLASH_TIMEOUT               0x08
#define FLASH_NOT_ERASED            0x10
#define FLASH_CONTENTERR            0x11

// device specific information
#if defined(FTFL)
	#define FTF    FTFL
	#define SECTOR_SIZE     (2048)
	#define PROGRAM_CMD      PGM4
  	#define PROGRAM_CMD_SIZE 4
#elif defined(FTFE)
	#define FTF    FTFE
	#define SECTOR_SIZE     (4096)
	#define PROGRAM_CMD      PGM8
  	#define PROGRAM_CMD_SIZE 8
#elif defined(FTFA)
    #if (__CORTEX_M == 0)
        #if defined(MKL28Z7)
	        #define SECTOR_SIZE     (2048)
        #else
	        #define SECTOR_SIZE     (1024)
        #endif
    #else
	    #define SECTOR_SIZE     (2048)
    #endif

	#define PROGRAM_CMD      PGM4
  	#define PROGRAM_CMD_SIZE 4
	#define FTF    FTFA
#endif

//!< API functions
void FLASH_Init(void);
uint32_t FLASH_GetSectorSize(void);
uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len);
uint8_t FLASH_EraseSector(uint32_t addr);
uint32_t FLASH_Test(uint32_t startAddr, uint32_t len);

#endif

