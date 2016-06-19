/** main.c **
 * main function
 * parameter init
 * PIT and UART handler
 */

#define IS_MAIN

#include "board.h"
#include "global.h"
#include "control.h"
#include "util.h"
#include "main.h"

const char CNST_NAME[CONST_CNT][20] = {
	"STABLE_ANGLE\r",
	"TG\r",
	"ANGLE_P\r",
	"ANGLE_D\r",
	"SPEED_I\r",
	"SPEED_P\r",
	"STR_REG_I\r",
	"STR_REG_P\r"
};

int main(void) {

	INIT();

#if ( MAIN_DEBUG == 0 )
	controlInit();
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
#else
	PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, true);
#endif // MAIN_DEBUG

	// enable interrupt & DMA for ov7725
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_IT_FallingEdge, true);

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
			printf("%.3f %.0f\r",
				angleError, angleControlOut);
		}
		break; //case 2

		case 3: // steering regulation
		steeringRegulate();
		break; //case 3

		case 4:

		break; //case 4
		
		default:
			;
	}

	// motor control output
	setMotor(MOTOR_L, speedOut(MOTOR_L, 
		LEFT_MOTOR_SCALE*(angleControlOut-speedControlOut
			-steeringRegulateOut)));
	setMotor(MOTOR_R, speedOut(MOTOR_R, 
		(angleControlOut-speedControlOut+steeringRegulateOut)));
}

void PIT1_ISR(void) {
	static uint16_t TIME = 0;
	TIME++;
	if (TIME == 500) {
		TIME = 0;
		LED1 = !LED1;
		LED2 = !LED2;
		// DMA_EnableRequest(HW_DMA_CH0);
	}
}

#if ( MAIN_DEBUG == 1 )
void UART_RX_ISR(uint16_t ch) {

}
#else
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
		
		case 'S': // toggle printFlag
		printFlag = 1 - printFlag;
		break; //'S'

		case '&': // motor enable
		motorEnable = 1;
		angleError = 0.0f;
		speedSP = 0;
		speedError = speedErrorIntegral = 0;
		speedControlAmountOld = speedControlAmount = 0.0f;
		speedControlOut = 0.0f;
		steeringError = steeringErrorIntegral = 0;
		steeringRegulateOut = 0.0f;
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
		case '4': case '5': case '6': case '7':
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
#endif // MAIN_DEBUG
