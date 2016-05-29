#define IS_MAIN

#include "main.h"

#define PI 3.14159265358979323846f
#define RAD_TO_DEG  180.0f/PI
#define GYRO_SCALE 65.5f
#define GYRO_OFFSET 1.1142f
#define DELTA_T 0.005f
// #define TG 0.25f
#define DEADZONE_L 600.0f
#define DEADZONE_R 450.0f
#define LEFT_MOTOR_SCALE 0.98f

enum {
	STABLE_ANGLE,
	TG,
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
	"TG\r",
	"ANGLE_P\r",
	"ANGLE_D\r",
	"SPEED_I\r",
	"SPEED_P\r"
};

float theta, theta_raw, omega;
float angleError;
float angleControlOut;

float speedControlAmount, speedControlAmountOld;
float speedControlOut, speedControlAmountAverage;
int16_t speedError;
int32_t speedErrorIntegral;
int16_t speedSP, speedAverage;

int main(void) {

	INIT();
	
	speed_l = speed_r = 0;

	theta = 0.0f;
	angleError = 0.0f;

	speedSP = 0;
	speedError = speedErrorIntegral = 0;
	speedControlAmount = speedControlAmountOld = 0.0f;
	speedControlOut = 0.0f;

	CTRL_CNST[STABLE_ANGLE] = -63.5f;
	CTRL_CNST[TG] = 2.0f;
	CTRL_CNST[ANGLE_P] = 345.0f;
	CTRL_CNST[ANGLE_D] = 40.0f;
	CTRL_CNST[SPEED_I] = 0.18f;
	CTRL_CNST[SPEED_P] = 0.05f;
	currentIndex = -1;
	
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);

	while (1) {
		
	}
	
}

void PIT0_ISR(void) {
	static uint16_t TIME = 0, TIM_CNT = 0, SPEED_CNT = 0;
	TIME++; TIM_CNT++;
	if (TIME == 500) {
		TIME = 0;
		LED1 = !LED1;
	}
	if (TIM_CNT == 5) TIM_CNT = 0;

	switch (TIM_CNT) {
		case 0: // angle control
		updateAngle(); // obtain current angle with filter
		angleControl(); // calculate angleControlOut
		break; // case 0

		case 1: // speed control and average
		SPEED_CNT++;
		if (SPEED_CNT == 20) {
			SPEED_CNT = 0;
			updateSpeed(); // obtain current speed
			speedControl(); // calculate speedControlAmount
		}
		speedControlAverage(SPEED_CNT); // smooth speedControlOut
		break; // case 1

		case 2: // send data
		if (printFlag) {
			printf("%.3f %.3f %.3f\r",
				angleControlOut, speedControlOut, angleError);
		}
		break; //case 2

		case 3:

		break; //case 3

		case 4:

		break; //case 4
		
		default:
			;
	}

	// motor control output
	setMotor(MOTOR_L, speedOut(MOTOR_L, 
		LEFT_MOTOR_SCALE*(angleControlOut-speedControlOut)));
	setMotor(MOTOR_R, speedOut(MOTOR_R, 
		(angleControlOut-speedControlOut)));
}

void UART_RX_ISR(uint16_t ch) {
	switch (ch) {
		case 0x1E: // up
		if (speedSP < 10000) {
			speedSP += 100;
			printf("set speed %d\r", speedSP);
		}
		break; //0x1E

		case 0x1F: // down
		if (speedSP > -10000) {
			speedSP -= 100;
			printf("set speed %d\r", speedSP);
		}
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
		angleError = 0.0f;
		speedSP = 0;
		speedError = speedErrorIntegral = 0;
		speedControlAmountOld = speedControlAmount = 0.0f;
		speedControlOut = 0.0f;
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
			for (int32_t i = 0; i < 10000000; i++);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 3000);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 3000);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);
			for (int32_t i = 0; i < 10000000; i++);
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
		case '4': case '5':
		currentIndex = ch - '0';
		printf(CNST_NAME[currentIndex]);
		break;

		case 'l': // CTRL_CNST-0.001f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.001f;
		break;

		case 'o': // CTRL_CNST+0.001f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.001f;
		break;

		case 'k': // CTRL_CNST-0.01f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.01f;
		break;

		case 'i': // CTRL_CNST+0.01f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.01f;
		break;

		case 'j': // CTRL_CNST-0.1f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 0.1f;
		break;

		case 'u': // CTRL_CNST+0.1f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 0.1f;
		break;

		case 'h': // CTRL_CNST-1.0f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] -= 1.0f;
		break;

		case 'y': // CTRL_CNST+1.0f
		if (currentIndex == -1)
			printf("invalid currentIndex\r");
		else CTRL_CNST[currentIndex] += 1.0f;
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

	angleControlOut = 
		CTRL_CNST[ANGLE_P]*angleError+CTRL_CNST[ANGLE_D]*omega;
}

void updateSpeed(void) {
	enc_data_l = getEncoder(ENC_L);
	enc_data_r = getEncoder(ENC_R);
	speedAverage = (enc_data_l + enc_data_r) / 2;
}

void speedControl(void) {
	speedError = speedSP - speedAverage;
	speedErrorIntegral = speedErrorIntegral*0.9f+speedError;
	speedControlAmountOld = speedControlAmount;
	speedControlAmount = CTRL_CNST[SPEED_I]*(float)speedErrorIntegral
		+CTRL_CNST[SPEED_P]*(float)speedError;
	speedControlAmountAverage =
		(speedControlAmount-speedControlAmountOld)/20.0f;
}

void speedControlAverage(uint16_t count) {
	speedControlOut =
		speedControlAmountAverage*count+speedControlAmountOld;
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
