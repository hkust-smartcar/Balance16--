/**
  ******************************************************************************
  * @file    flash.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2015.8.28
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "flash.h"
#include "common.h"

/* flash commands */
#define RD1BLK    0x00  /* read 1 block */
#define RD1SEC    0x01  /* read 1 section */
#define PGMCHK    0x02  /* program check */
#define RDRSRC    0x03  /* read resource */
#define PGM4      0x06  /* program phase program 4 byte */
#define PGM8      0x07  /* program phase program 8 byte */
#define ERSBLK    0x08  /* erase flash block */
#define ERSSCR    0x09  /* erase flash sector */
#define PGMSEC    0x0B  /* program section */
#define RD1ALL    0x40  /* read 1s all block */
#define RDONCE    0x41  /* read once */
#define PGMONCE   0x43  /* program once */
#define ERSALL    0x44  /* erase all blocks */
#define VFYKEY    0x45  /* verift backdoor key */
#define PGMPART   0x80  /* program paritition */
#define SETRAM    0x81  /* set flexram function */
#define NORMAL_LEVEL 0x0


volatile uint8_t s_flash_command_run[] = {0x00, 0xB5, 0x80, 0x21, 0x01, 0x70, 0x01, 0x78, 0x09, 0x06, 0xFC, 0xD5,0x00, 0xBD};
typedef void (*flash_run_entry_t)(volatile uint8_t *reg);
flash_run_entry_t s_flash_run_entry;
    
static inline void FlashWaitCmdExecution(void) {
    while (!(FTFE->FSTAT & FTFE_FSTAT_CCIF_MASK));
}
    
static inline uint8_t FlashCmdStart(void)
{   
    uint8_t ret = CH_OK;

    /* start cmd execution */
    FTF->FSTAT |= FTFE_FSTAT_CCIF_MASK;

    /* wait for cmd execution */
    FlashWaitCmdExecution();

    if (FTFE->FSTAT & FTFE_FSTAT_MGSTAT0_MASK)          // memory controller command finished
        ret |= FTFE_FSTAT_MGSTAT0_MASK;
    else if (FTFE->FSTAT & FTFE_FSTAT_FPVIOL_MASK)      // flash protection violation
    {
        FTFE->FSTAT |= FTFE_FSTAT_FPVIOL_MASK;
        ret |= FTFE_FSTAT_FPVIOL_MASK;
    }
    else if (FTFE->FSTAT & FTFE_FSTAT_ACCERR_MASK)      // flash access error
    {
        FTFE->FSTAT |= FTFE_FSTAT_ACCERR_MASK;
        ret |= FTFE_FSTAT_ACCERR_MASK;
    }
    else if (FTFE->FSTAT & FTFE_FSTAT_RDCOLERR_MASK)    // FTFE read collision
    {
        FTFE->FSTAT |= FTFE_FSTAT_RDCOLERR_MASK;
        ret |= FTFE_FSTAT_RDCOLERR_MASK;
    }

    return ret;
}

 /**
 * @brief  get flash sector size
 * @retval flash sector size
 */
uint32_t FLASH_GetSectorSize(void)
{
    return SECTOR_SIZE;
}

void FLASH_Init(void)
{
    /* clear status */
    FTF->FSTAT |= FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK;
}

 /**
 * @brief  flash erase sector
 * @note   this function will erase a flash sector
 * @param  addr: start addr
 * @retval Flash return code
 */
uint8_t FLASH_EraseSector(uint32_t addr)
{
    uint8_t ret;

    /* wait for last cmd execution */
    FlashWaitCmdExecution();

    /* set cmd */
	FTF->FCCOB0 = ERSSCR; 
	FTF->FCCOB1 = (uint8_t)((addr>>16)&0xFF);
	FTF->FCCOB2 = (uint8_t)((addr>>8)&0xFF);
	FTF->FCCOB3 = (uint8_t)(addr&0xFF);
    __disable_irq();
    ret = FlashCmdStart();
    __enable_irq();
    
    return ret;
}

 /**
 * @brief  flash write sector
 * @note   len must = sector size
 * @param  addr: start addr
 * @param  buf : buffer pointer
 * @param  len : len
 * @retval Flash return code
 */
uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    uint8_t ret = CH_OK;
    uint8_t temp[PROGRAM_CMD_SIZE];        // temp data

    /* wait for last cmd execution */
    FlashWaitCmdExecution();

    /* length error */
    if (len == 0 || len > SECTOR_SIZE) return CH_ERR;

	FTF->FCCOB0 = PROGRAM_CMD;
    
    while (ret == CH_OK && len)
	{
        /* set address */
        FTF->FCCOB1 = (uint8_t)((addr>>16)&0xFF);
        FTF->FCCOB2 = (uint8_t)((addr>>8)&0xFF);
        FTF->FCCOB3 = (uint8_t)(addr&0xFF);

        memset(temp, 0xFF, PROGRAM_CMD_SIZE);

        for (uint8_t i = 0; i < PROGRAM_CMD_SIZE && len; len--, i++) {
            temp[i] = buf[i];
        }

		FTF->FCCOB4 = temp[3];
		FTF->FCCOB5 = temp[2];
		FTF->FCCOB6 = temp[1];
		FTF->FCCOB7 = temp[0];
        
        if(PROGRAM_CMD_SIZE == 8)
        {
            FTF->FCCOB8 = temp[7];
            FTF->FCCOB9 = temp[6];
            FTF->FCCOBA = temp[5];
            FTF->FCCOBB = temp[4];
        }

        buf += PROGRAM_CMD_SIZE;
        addr += PROGRAM_CMD_SIZE;

        __disable_irq();
        ret = FlashCmdStart();
        __enable_irq();
        
    }
    return ret;
}


uint32_t FLASH_SetcorTest(uint32_t addr)
{
    uint32_t ret, i,j;
    uint8_t *p;
    ALIGN(8) uint8_t buf[32];
    
    LIB_TRACE("program addr:0x%X(%dKB) ...", addr, addr/1024);
    ret = FLASH_EraseSector(addr);
    
    for(i=0; i<sizeof(buf); i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for(i=0; i<(SECTOR_SIZE/sizeof(buf)); i++)
    {
        ret += FLASH_WriteSector(addr + sizeof(buf)*i, buf, sizeof(buf));  
        if(ret)
        {
            LIB_TRACE("issue command failed\r\n");
            return CH_ERR;
        }
    }
    
    LIB_TRACE("varify addr:0x%X ...", addr);
    for(i=0; i<(SECTOR_SIZE/sizeof(buf)); i++)
    {
        p = (uint8_t*)(addr + sizeof(buf)*i);
        for(j=0; j<sizeof(buf); j++)
        {
            if(p[j] != (j%0xFF))
            {
                ret++;
                LIB_TRACE("ERR:[%d]:0x%02X ", i, *p); 
            }
        }
    }
    return ret;
}

 /**
 * @brief  flash self test
 * @note   make sure to have enough stack size
 * @retval FLASH_OK: succ, other flash test fail
 */
uint32_t FLASH_Test(uint32_t addr, uint32_t len)
{
    int i, ret;
    FLASH_Init();
    
    for(i=0; i<(len/SECTOR_SIZE); i++)
    {
        ret = FLASH_SetcorTest(addr + i*SECTOR_SIZE);
        if(ret == CH_OK)
        {
            LIB_TRACE("OK\r\n");
        }
        else
        {
            LIB_TRACE("ERR\r\n");
            return ret;
        }
    }
    return ret;
}
