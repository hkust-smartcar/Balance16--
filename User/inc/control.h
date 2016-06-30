#ifndef _USER_CONTROL_H_
#define _USER_CONTROL_H_

#include <math.h>
#include "common.h"

#define PI 3.14159265358979323846f
#define RAD_TO_DEG  180.0f/PI
#define GYRO_SCALE 65.5f
#define GYRO_X_OFFSET -60
#define GYRO_Z_OFFSET -44
#define GYRO_Y_OFFSET 1.1142f
#define DELTA_T 0.005f
#define DEADZONE_L 300.0f
#define DEADZONE_R 300.0f
#define LEFT_MOTOR_SCALE 1.00f

void controlInit(void);
void updateAngle(void);
void angleControl(void);
void updateSpeed(void);
void speedControl(void);
void speedControlAverage(uint16_t count);
void steeringRegulate(void);
int32_t speedOut(uint32_t id, float speedIn);

#endif // _USER_CONTROL_H_
