/** util.h **
 * peripheral init
 */

#include "common.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "pit.h"
#include "ftm.h"
#include "i2c.h"
#include "mpu6050.h"
#include "dma.h"
#include "ov7725.h"

void INIT(void);

int16_t getEncoder(uint32_t);
void setMotor(uint32_t, int32_t);

void printEncoder(uint32_t);
void printMPU(uint32_t);

uint8_t ov7725_Init(uint32_t);
void extractImage(void);
