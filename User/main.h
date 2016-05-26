#ifdef IS_MAIN
	#define ISEXTERNAL extern
#else
	#define ISEXTERNAL
#endif // IS_MAIN

#include "common.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "pit.h"
#include "ftm.h"

#define LED1 PEout(12)
#define ENC_L HW_FTM2
#define ENC_R HW_FTM1
#define MOTOR_L 0
#define MOTOR_R 1

void INIT(void);
void PIT0_ISR(void);
void UART_RX_ISR(uint16_t);
int16_t getEncoder(uint32_t);
void setMotor(uint32_t, int32_t);

ISEXTERNAL int32_t speed;
ISEXTERNAL int16_t enc_data_l, enc_data_r;
