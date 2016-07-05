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
	// systemTest();
	controlInit();
	PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);

	// enable interrupt & DMA for ov7725
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_DMA_FallingEdge, true);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, true);


#else
	PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, true);
	// // enable interrupt & DMA for ov7725
	// GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_DMA_FallingEdge, true);
	// GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, true);

#endif // MAIN_DEBUG

	
	while (1) {
#if ( MAIN_DEBUG == 0 )
		if (imgReady) {
			imgLocked = true;
			st7735r_PlotImg(WHITE,BLACK,imgRaw,OV7725_H*(OV7725_W/8));
			imageProcessing();
			imgReady = false;
			imgLocked = false;
		}
#endif // MAIN_DEBUG
	}
	
}

void PIT0_ISR(void) {
	GPIO_WriteBit(HW_GPIOA, 16, 1);

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
			printf("%.3f %.0f %4d %.0f\r",
				angleError, angleControlOut, speedAverage, speedControlOut);
			// printMPU(AX);
			// printMPU(AY);
			// printMPU(AZ);
			// printMPU(GX);
			// printMPU(GY);
			// printMPU(GZ);
			// printf("\r");
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

	GPIO_WriteBit(HW_GPIOA, 16, 0);
}

void PIT1_ISR(void) {
	GPIO_ToggleBit(HW_GPIOA, 17);
	static uint16_t TIME = 0, TIM_CNT = 0;
	TIME++; TIM_CNT++;
	if (TIME == 500) {
		TIME = 0;
		LED1 = !LED1;
		LED2 = !LED2;
		// DMA_EnableRequest(HW_DMA_CH0);
	}
	if (TIM_CNT == 500) {
		TIM_CNT = 0;
		UART_WriteByte(HW_UART4, (uint16_t)0x55);
		ultraState = 0;		
	}
	
}

#if ( MAIN_DEBUG == 1 )
void BT_RX_ISR(uint16_t ch) {
	switch (ch) {
		case 'S':
		printFlag = 1 - printFlag;
		break;

		default:
			;
	}
}

void US100_RX_ISR(uint16_t ch) {
	if (ultraState == 0) {
		ultraDis = ch<<8;
		ultraState = 1;
	}
	else {
		ultraDis |= (ch&0xFF);
		st7735r_Print(0, 8, GREEN, BLACK, "%d", ultraDis);
		ultraState = 0;
	}
}
#else
void BT_RX_ISR(uint16_t ch) {
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
		if (steeringSP > -16000) {
			steeringSP -= 1000;
			printf("set steering %d\r", steeringSP);
		}
		break; //0x1C

		case 0x1D: // right
		if (steeringSP < 16000) {
			steeringSP += 1000;
			printf("set steering %d\r", steeringSP);
		}
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

		steeringSP = 0;
		steeringError = steeringErrorIntegral = 0;
		steeringRegulateOut = 0.0f;
		break; //'&'

		case ' ': // motor disable
		motorEnable = 0;
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		speed_l = speed_r = 0;
		break; //' '

		case 't': // motor test
		if (!motorEnable) {
			motorTest(MOTOR_L);DelayMs(500);
			motorTest(MOTOR_R);DelayMs(500);
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
