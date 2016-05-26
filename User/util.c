#include "main.h"

void INIT(void) {
	GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOE, 12, 0);
	
	PIT_InitTypeDef PIT_InitStruct;
	PIT_InitStruct.chl = HW_PIT_CH0;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
	
	UART_InitTypeDef UART_InitStruct;
	UART_InitStruct.instance = HW_UART0;
	UART_InitStruct.baudrate = 115200;
	UART_InitStruct.parityMode = kUART_ParityDisabled;
	UART_InitStruct.bitPerChar = kUART_8BitsPerChar;
	UART_Init(&UART_InitStruct);
	
	PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
	PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
	
	UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
	UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
		
	FTM_PWM_QuickInit(FTM3_CH0_PD00, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH1_PD01, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH2_PD02, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH3_PD03, kPWM_EdgeAligned, 10000);
	speed = 0;
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, speed);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, speed);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, speed);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, speed);
	
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	FTM_QD_QuickInit(FTM2_QD_PHA_PA10_PHB_PA11,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
}

int16_t getEncoder(uint32_t instance) {
	int16_t v = 0;
	uint8_t dir;
	switch (instance) {
		case ENC_L:
		FTM_QD_GetData(instance, &v, &dir);
		FTM_QD_ClearCount(instance);
		break; // ENC_L

		case ENC_R:
		FTM_QD_GetData(instance, &v, &dir);
		FTM_QD_ClearCount(instance);
		v = -v;
		break; // ENC_R

		default:
		;
	}
	return v;
}

void setMotor(uint32_t id, int32_t pwmDuty) {
	switch (id) {
		case MOTOR_L:
		if (pwmDuty > 0) {
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, (uint32_t)pwmDuty);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0u);
		}
		else {
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0u);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, (uint32_t)(-pwmDuty));
		}
		break; // MOTOR_L

		case MOTOR_R:
		if (pwmDuty > 0) {
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, (uint32_t)pwmDuty);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0u);
		}
		else {
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0u);
			FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, (uint32_t)(-pwmDuty));
		}
		break;  // MOTOR_R

		default:
		;
	}
}
