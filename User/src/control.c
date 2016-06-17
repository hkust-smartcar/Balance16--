/** control.c **
 * control parameter init
 * control algorithm
 */

#include "board.h"
#include "global.h"
#include "control.h"
#include "util.h"

void controlInit(void) {
	theta  = theta_raw = CTRL_CNST[STABLE_ANGLE];
	omega = 0.0f;
	angleError = 0.0f;
	angleControlOut = 0.0f;

	speedSP = 0;
	speedError = speedErrorIntegral = 0;
	speedControlAmount = speedControlAmountOld = 0.0f;
	speedControlOut = 0.0f;

	steeringRegulateOut = 0.0f;

	CTRL_CNST[STABLE_ANGLE] = -63.5f;
	CTRL_CNST[TG] = 1.0f;
	CTRL_CNST[ANGLE_P] = 320.0f;
	CTRL_CNST[ANGLE_D] = 35.0f;
	CTRL_CNST[SPEED_I] = 0.20f;
	CTRL_CNST[SPEED_P] = 0.60f;
	CTRL_CNST[STR_REG_I] = 0.05f;
	CTRL_CNST[STR_REG_P] = 0.0f;
	currentIndex = -1;
}

void updateAngle(void) {
	mpu6050_read_accel(accel);
	mpu6050_read_gyro(gyro);
	theta_raw = (float)atan2((double)accel[AZ & 0x0F],
		-(double)accel[AX & 0x0F])*RAD_TO_DEG;
	omega = -(float)gyro[GY & 0x0F]/GYRO_SCALE-GYRO_Y_OFFSET;
	theta += (omega+(theta_raw-theta)/CTRL_CNST[TG])*DELTA_T;
}

void angleControl(void) {
	angleError = theta - CTRL_CNST[STABLE_ANGLE];
	angleControlOut = CTRL_CNST[ANGLE_P]*angleError
		+CTRL_CNST[ANGLE_D]*omega;
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

void steeringRegulate(void) {
	steeringError = gyro[GX & 0x0F]-GYRO_X_OFFSET;
	steeringErrorIntegral = steeringErrorIntegral*0.9f+steeringError;
	steeringRegulateOut = (float)steeringErrorIntegral*CTRL_CNST[STR_REG_I]
		+(float)steeringError*CTRL_CNST[STR_REG_P];
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
