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

	steeringSP = 0;
	steeringRegulateOut = 0.0f;

	CTRL_CNST[STABLE_ANGLE] = -78.8f;
	CTRL_CNST[TG] = 0.6f;
	CTRL_CNST[ANGLE_P] = 500.0f;
	CTRL_CNST[ANGLE_D] = 26.0f;
	CTRL_CNST[SPEED_I] = 0.20f;
	CTRL_CNST[SPEED_P] = 0.50f;
	CTRL_CNST[STR_REG_I] = 0.20f;
	CTRL_CNST[STR_REG_P] = 0.30f;
	currentIndex = -1;
}

void updateAngle(void) {
	mpu6050_read_accel(accel);
	mpu6050_read_gyro(gyro);
	theta_raw = (float)atan2(-(double)accel[AZ & 0x0F],
		(double)accel[AX & 0x0F])*RAD_TO_DEG;
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
	steeringError = -(gyro[GZ & 0x0F]-GYRO_Z_OFFSET)-steeringSP;	// CW when looking downwards
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

void imageProcessing(void) {
	float mean = 0.0f, rowMean = 0.0f;
	uint32_t x = 0, rowCounter = 0, pixelCounter = 0;
	uint32_t i, j, k;
	for (j = 0; j < 600; j += 10) {
		pixelCounter = 0;
		rowMean = 0.0f;
		for (i = 0; i < 10; i++) {
			x = imgRaw[i+j];
			for (k = 0; k < 8; k++) {
				if (!((x>>(7-k))&1u)) {
					rowMean += (float)((i<<3) + k);
					pixelCounter++;
				}
			}
		}
		if (pixelCounter) {
			mean += rowMean/(float)pixelCounter;
			rowCounter++;
		}
	}
	if (rowCounter) {
		mean = mean / ((float)rowCounter);
		st7735r_Print(0, 9, GREEN, BLACK, "%.2f", mean);
		steeringSP = (mean-39.5f)*200.0f;
	}
	else {
		// steeringSP = 4000;
	}
}
