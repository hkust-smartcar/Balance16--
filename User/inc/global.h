/** global.h **
 * all global variable
 */

#ifdef _GLOBAL_C_
 #define EXT
#else
 #define EXT extern
#endif // _GLOBAL_C_

#include "common.h"
#include "board.h"

EXT float theta, theta_raw, omega;
EXT float angleError;
EXT float angleControlOut;
EXT int16_t accel[3], gyro[3];

EXT int16_t speed_l, speed_r;
EXT int16_t speedSP, speedAverage;
EXT int16_t speedError;
EXT int32_t speedErrorIntegral;
EXT float speedControlAmount, speedControlAmountOld;
EXT float speedControlOut, speedControlAmountAverage;

EXT int16_t enc_data_l, enc_data_r;

EXT int16_t steeringError;
EXT int32_t steeringErrorIntegral;
EXT float steeringRegulateOut;

EXT int8_t printFlag;
EXT int8_t motorEnable;

EXT float CTRL_CNST[CONST_CNT];
EXT int8_t currentIndex;

EXT uint8_t imgRaw[OV7725_H*(OV7725_W/8)];
EXT uint8_t img[OV7725_H*OV7725_W];
