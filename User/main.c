#define IS_MAIN

#include "main.h"

int main(void) {

	INIT();
	
	while (1) {
		
	}
	
}

void PIT0_ISR(void) {
	static uint16_t TIME = 0, TIM_CNT = 0;
	TIME++; TIM_CNT++;
	if (TIME == 500) {
		TIME = 0;
		LED1 = !LED1;
	}
	if (TIM_CNT == 5) TIM_CNT = 0;

	switch (TIM_CNT) {
		case 0:
		// enc_data_l = getEncoder(ENC_L);
		// enc_data_r = getEncoder(ENC_R);
		mpu6050_read_accel(accel);
		mpu6050_read_gyro(gyro);
		if (printFlag) {
			printMPU(AX);
			printMPU(AY);
			printMPU(AZ);
			printMPU(GX);
			printMPU(GY);
			printMPU(GZ);
			UART_WriteByte(HW_UART0, '\r');
		}
		break; // case 0

		case 1:

		break; // case 1

		case 2:

		break; //case 2

		case 3:

		break; //case 3

		case 4:

		break; //case 4
		
		default:
			;
	}
}

void UART_RX_ISR(uint16_t ch) {
	switch (ch) {
		case 0x1E: // up
		if (speed < 9000) speed += 1000;
		setMotor(MOTOR_L, speed);
		break; //0x1E

		case 0x1F: // down
		if (speed > -9000) speed -= 1000;
		setMotor(MOTOR_L, speed);
		break; //0x1F

		case 0x1C: // left

		break; //0x1C

		case 0x1D: // right

		break; //0x1D
		
		case 'S': //send data
		printFlag = 1 - printFlag;
		break; //'S'

		default:
		UART_WriteByte(HW_UART0, ch);
	}
}
