#define IS_MAIN

#include "main.h"

#define PI 3.14159265358979323846f
#define RAD_TO_DEG  180.0f/PI
#define GYRO_SCALE 65.5f
#define GYRO_OFFSET 1.1142f
#define DELTA_T 0.005f
#define TG 0.25f

float theta, theta_raw, omega;

int main(void) {

	INIT();
	
	theta = 0.0f;
	
	UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
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
		updateAngle();
		if (printFlag) {
			printf("%.3f %.3f %.3f\r", theta, theta_raw, omega);
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
			;
	}
}

void updateAngle(void) {
	mpu6050_read_accel(accel);
	mpu6050_read_gyro(gyro);
	theta_raw = (float)atan2((double)accel[AZ & 0x0F],
		-(double)accel[AX & 0x0F])*RAD_TO_DEG;
	omega = -(float)gyro[GY & 0x0F]/GYRO_SCALE-GYRO_OFFSET;
	theta += (omega+(theta_raw-theta)/TG)*DELTA_T;
}
