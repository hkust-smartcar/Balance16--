/** util.h **
 * peripheral init
 */

#ifndef _USER_UTIL_H_
#define _USER_UTIL_H_

#include "common.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "pit.h"
#include "ftm.h"
#include "i2c.h"
#include "mpu6050.h"
#include "dma.h"
#include "spi.h"
#include "st7735r.h"
#include "ov7725.h"

void INIT(void);

// test modules
void systemTest(void);
void buttonTest_ISR(uint32_t array);
void GPIO_DUMMY(uint32_t array);		// dummy GPIO ISR

int16_t getEncoder(uint32_t instance);
void setMotor(uint32_t id, int32_t pwmDuty);

void printEncoder(uint32_t id);
void printMPU(uint32_t id);

uint8_t ov7725_Init(uint32_t I2C_MAP);
// void extractImage(void);
void ov7725_ISR(uint32_t array);
void ov7725_DMA_Complete_ISR(void);

// static void Button_Handler(uint32_t array);

#endif // _USER_UTIL_H_
