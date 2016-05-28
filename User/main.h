#ifdef IS_MAIN
	#define ISEXTERNAL extern
#else
	#define ISEXTERNAL
#endif // IS_MAIN

#include <math.h>
#include "common.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "pit.h"
#include "ftm.h"
#include "i2c.h"
#include "mpu6050.h"

//GPIO
#define LED1 PEout(12)
#define LED2 PEout(11)

//Encoder & Motor
#define ENC_L HW_FTM2
#define ENC_R HW_FTM1
#define MOTOR_L 0
#define MOTOR_R 1

//mpu6050
#define AX 0x00
#define AY 0x01
#define AZ 0x02
#define GX 0x10
#define GY 0x11
#define GZ 0x12

void INIT(void);
void PIT0_ISR(void);
void UART_RX_ISR(uint16_t);

int16_t getEncoder(uint32_t);
void setMotor(uint32_t, int32_t);

void updateAngle(void);

void printEncoder(uint32_t);
void printMPU(uint32_t);


ISEXTERNAL int32_t speed;
ISEXTERNAL int16_t enc_data_l, enc_data_r;
ISEXTERNAL int8_t printFlag;
ISEXTERNAL int16_t accel[3], gyro[3];
