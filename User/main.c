#define IS_MAIN

#include "main.h"

#define PI 3.14159265358979323846f
#define RAD_TO_DEG  180.0f/PI
#define GYRO_SCALE 65.5f
#define GYRO_OFFSET 1.1142f
#define DELTA_T 0.005f
#define TG 0.25f
#define DEADZONE_L 1400.0f
#define DEADZONE_R 1000.0f
#define LEFT_MOTOR_SCALE 0.87f

enum {
	STABLE_ANGLE,
	ANGLE_P,
	ANGLE_D,
	SPEED_I,
	SPEED_P,
	CONST_CNT
};

float CTRL_CNST[CONST_CNT];
int32_t currentIndex;
const char CNST_NAME[CONST_CNT][20] = {
	"STABLE_ANGLE\r",
	"ANGLE_P\r",
	"ANGLE_D\r",
	"SPEED_I\r",
	"SPEED_P\r"
};

float theta, theta_raw, omega;
float angleErrorIntrgral, angleError;
float angleControlOut, speedControlOut;
int16_t speedSP;

int main(void) {

	INIT();
	
	speed_l = speed_r = 0;
	speedSP = 0;

	theta = 0.0f;
	angleErrorIntrgral = 0.0f;
	angleError = 0.0f;

	CTRL_CNST[STABLE_ANGLE] = -59.2f;
	CTRL_CNST[ANGLE_P] = 80.0f;
	CTRL_CNST[ANGLE_D] = 9.0f;
	CTRL_CNST[SPEED_I] = 0.0f;
	CTRL_CNST[SPEED_P] = 0.0f;
	currentIndex = -1;
	
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);

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
		updateAngle();
		angleControl();
		if (printFlag) {
			printf("%.3f %.3f\r", theta, omega);
			// enc_data_r = getEncoder(ENC_R);
			// printf("%d ", speed_r);
			// printEncoder(ENC_R);
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

	// motor control output
	setMotor(MOTOR_L, speedOut(MOTOR_L, LEFT_MOTOR_SCALE*angleControlOut));
	setMotor(MOTOR_R, speedOut(MOTOR_R, angleControlOut));
}

void UART_RX_ISR(uint16_t ch) {
	switch (ch) {
		case 0x1E: // up

		break; //0x1E

		case 0x1F: // down
		
		break; //0x1F

		case 0x1C: // left

		break; //0x1C

		case 0x1D: // right

		break; //0x1D
		
		case 'S': // send data
		printFlag = 1 - printFlag;
		break; //'S'

		case '&': // motor enable
		motorEnable = 1;
		angleErrorIntrgral = 0.0f;
		angleError = 0.0f;
		break; //'&'

		case ' ': // motor disable
		motorEnable = 0;
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0);
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0);
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);
		speed_l = speed_r = 0;
		break; //' '

		case 't': // motor test
		if (!motorEnable) {
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 3000);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 3000);
			for (int32_t i = 0; i < 1000000; i++);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 3000);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 3000);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);
			for (int32_t i = 0; i < 1000000; i++);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);
		}
		break;

		case 's': // show current CTRL_CNST
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else printf("%.4f\r", CTRL_CNST[currentIndex]);
		break; //'a'

		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6':
		currentIndex = ch - '0';
		printf(CNST_NAME[currentIndex]);
		break;

		case 'l': // CTRL_CNST-0.0001f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.0001f;
		break;

		case 'o': // CTRL_CNST+0.0001f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.0001f;
		break;

		case 'k': // CTRL_CNST-0.001f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.001f;
		break;

		case 'i': // CTRL_CNST+0.001f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.001f;
		break;

		case 'j': // CTRL_CNST-0.01f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.01f;
		break;

		case 'u': // CTRL_CNST+0.01f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.01f;
		break;

		case 'h': // CTRL_CNST-0.1f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.1f;
		break;

		case 'y': // CTRL_CNST+0.1f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.1f;
		break;

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

void angleControl(void) {
	angleError = theta - CTRL_CNST[STABLE_ANGLE];
	// angleErrorIntrgral += angleError;

	// angleControlOut_l = CTRL_CNST[ANGLE_P]*angleErrorIntrgral
	// 	+(CTRL_CNST[ANGLE_P]*CTRL_CNST[TM_L]+CTRL_CNST[ANGLE_D])*angleError
	// 	+CTRL_CNST[TM_L]*CTRL_CNST[ANGLE_D]*omega;

	// angleControlOut_r = CTRL_CNST[ANGLE_P]*angleErrorIntrgral
	// 	+(CTRL_CNST[ANGLE_P]*CTRL_CNST[TM_R]+CTRL_CNST[ANGLE_D])*angleError
	// 	+CTRL_CNST[TM_R]*CTRL_CNST[ANGLE_D]*omega;

	angleControlOut =
		CTRL_CNST[ANGLE_P]*angleError+CTRL_CNST[ANGLE_D]*omega;
}

void speedControl(void) {

}

int32_t speedOut(uint32_t id, float speedIn) {
	float output = speedIn;
	switch (id) {
		case MOTOR_L:
		if (output > 0.0f)
			output += DEADZONE_L;
		if (output < 0.0f)
			output -= DEADZONE_L;
		break; //MOTOR_L

		case MOTOR_R:
		if (output > 0.0f)
			output += DEADZONE_R;
		if (output < 0.0f)
			output -= DEADZONE_R;
		break; //MOTOR_R

		default:
			;
	}
	return (int32_t)output;
}
