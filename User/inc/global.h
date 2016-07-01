/** global.h **
 * all global variable
 */

#ifndef _USER_GLOBAL_H_
#define _USER_GLOBAL_H_

#ifdef _GLOBAL_C_
 #define EXT
#else
 #define EXT extern
#endif // _GLOBAL_C_

#include "common.h"
#include "board.h"
#include "ov7725.h"
#include "flash.h"

// angle
EXT float theta, theta_raw, omega;
EXT float angleError;
EXT float angleControlOut;
EXT int16_t accel[3], gyro[3];

// speed
EXT int16_t speed_l, speed_r;
EXT int16_t speedSP, speedAverage;
EXT int16_t speedError;
EXT int32_t speedErrorIntegral;
EXT float speedControlAmount, speedControlAmountOld;
EXT float speedControlOut, speedControlAmountAverage;

EXT int16_t enc_data_l, enc_data_r;

// steering
EXT int16_t steeringError, steeringSP;
EXT int32_t steeringErrorIntegral;
EXT float steeringRegulateOut;

// flags in main
EXT int8_t printFlag;
EXT int8_t motorEnable;

// control parameters
EXT float CTRL_CNST[CONST_CNT];
EXT int8_t currentIndex;

//ov7725
EXT uint8_t* imgRaw;
EXT uint8_t* imgBuffer;
EXT uint8_t imgBuffer1[OV7725_H*(OV7725_W/8)];
EXT uint8_t imgBuffer2[OV7725_H*(OV7725_W/8)];
EXT uint8_t printBuffer[OV7725_H*(OV7725_W/8)];
EXT uint8_t img[OV7725_H*OV7725_W];
EXT bool imgReady;

// flash
EXT uint8_t buf[SECTOR_SIZE];

#endif // _USER_GLOBAL_H_
