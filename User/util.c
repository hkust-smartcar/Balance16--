#include "main.h"

void INIT(void) {
	//GPIO
	GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOE, 12, 1);
	GPIO_WriteBit(HW_GPIOE, 11, 1);
	
	//PIT
	PIT_InitTypeDef PIT_InitStruct;
	PIT_InitStruct.chl = HW_PIT_CH0;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);

#if ( MAIN_DEBUG == 1) // new feature testing
	PIT_InitStruct.chl = HW_PIT_CH1;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	PIT_CallbackInstall(HW_PIT_CH1, PIT1_ISR);
	PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, false);
#endif // MAIN_DEBUG
	
	//UART
	UART_InitTypeDef UART_InitStruct;
	UART_InitStruct.instance = HW_UART0;
	UART_InitStruct.baudrate = 115200;
	UART_InitStruct.parityMode = kUART_ParityDisabled;
	UART_InitStruct.bitPerChar = kUART_8BitsPerChar;
	UART_Init(&UART_InitStruct);
	
	PORT_PinMuxConfig(HW_GPIOD, 6, kPinAlt3);
	PORT_PinMuxConfig(HW_GPIOD, 7, kPinAlt3);
	
	// UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
	// UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	printFlag = 0;
	
	//PWM (PWM)
	FTM_PWM_QuickInit(FTM3_CH0_PD00, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH1_PD01, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH2_PD02, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH3_PD03, kPWM_EdgeAligned, 10000);
	
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);

	motorEnable = 0;
	
	//FTM (Decoder)
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	FTM_QD_QuickInit(FTM2_QD_PHA_PA10_PHB_PA11,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	
	//I2C, MPU6050
	DelayInit();
	uint8_t instance = I2C_QuickInit(I2C0_SCL_PB00_SDA_PB01, 100*1000);
	mpu6050_init((uint32_t) instance);
	struct mpu_config mpuConfig;
	mpuConfig.afs = AFS_4G;
	mpuConfig.gfs = GFS_500DPS;
	mpuConfig.aenable_self_test = false;
	mpuConfig.genable_self_test = false;
	mpuConfig.gbypass_blpf = false;
	mpu6050_config(&mpuConfig);

	// DMA, uart loop
	// rx DMA
	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.chl = HW_DMA_CH0;
	DMA_InitStruct.chlTriggerSource = UART0_REV_DMAREQ;
	DMA_InitStruct.minorLoopByteCnt = 1;
	DMA_InitStruct.majorLoopCnt = 1;
	DMA_InitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

	DMA_InitStruct.sAddrOffset = 0;
	DMA_InitStruct.sAddr = (uint32_t)&UART0->D;
	DMA_InitStruct.sDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.sLastAddrAdj = 0;
	DMA_InitStruct.sMod = kDMA_ModuloDisable;

	DMA_InitStruct.dAddrOffset = 0;
	DMA_InitStruct.dAddr = (uint32_t)&ch_buffer;
	DMA_InitStruct.dDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.dLastAddrAdj = 0;
	DMA_InitStruct.dMod = kDMA_ModuloDisable;

	DMA_Init(&DMA_InitStruct);

	//tx DMA
	DMA_InitStruct.chl = HW_DMA_CH1;
	DMA_InitStruct.chlTriggerSource = MUX0_DMAREQ; // always enable
	DMA_InitStruct.minorLoopByteCnt = 1;
	DMA_InitStruct.majorLoopCnt = 1;
	DMA_InitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

	DMA_InitStruct.sAddrOffset = 0;
	DMA_InitStruct.sAddr = (uint32_t)&ch_buffer;
	DMA_InitStruct.sDataWidth =  kDMA_DataWidthBit_8;
	DMA_InitStruct.sLastAddrAdj = 0;
	DMA_InitStruct.sMod = kDMA_ModuloDisable;

	DMA_InitStruct.dAddrOffset = 0;
	DMA_InitStruct.dAddr = (uint32_t)&UART0->D;
	DMA_InitStruct.dDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.dLastAddrAdj = 0;
	DMA_InitStruct.dMod = kDMA_ModuloDisable;

	DMA_Init(&DMA_InitStruct);

	//UART DMA config
	UART_ITDMAConfig(HW_UART0, kUART_DMA_Rx, true);

	//chl-chl link
	DMA_EnableMajorLink(HW_DMA_CH0, HW_DMA_CH1, true);

	//config auto-disable
	DMA_EnableAutoDisableRequest(HW_DMA_CH0, false);
	DMA_EnableAutoDisableRequest(HW_DMA_CH1, true);

	//Enable channel
	DMA_EnableRequest(HW_DMA_CH0);
	DMA_EnableRequest(HW_DMA_CH1);
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
	if (!motorEnable) {
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0);
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0);
		FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);
		return;
	}

	if (pwmDuty > 9000) pwmDuty = 9000;
	if (pwmDuty < -9000) pwmDuty = -9000;

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

void printEncoder(uint32_t id) {
	switch (id) {
		case ENC_L:
		// UART_WriteByte(HW_UART0, ((enc_data_l >> 8) & 0xFF));
		// UART_WriteByte(HW_UART0, (enc_data_l & 0xFF));
		printf("%d\r", enc_data_l);
		break; // ENC_L

		case ENC_R:
		// UART_WriteByte(HW_UART0, ((enc_data_r >> 8) & 0xFF));
		// UART_WriteByte(HW_UART0, (enc_data_r & 0xFF));
		printf("%d\r", enc_data_r);
		break; // ENC_R

		default:
			;
	}
}

void printMPU(uint32_t id) {
	// int16_t data;
	if (id >> 4) { // gyro
		printf("%d ", gyro[id & 0x0F]);
	}
	else printf("%d ", accel[id & 0x0F]); //accel
}
