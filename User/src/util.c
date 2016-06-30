/** util.c **
 * peripheral init
 * setEncoder & getMotor
 * print data
 */

#include "board.h"
#include "global.h"
#include "util.h"
#include "main.h"
#include "control.h"

#if ( MAIN_DEBUG == 1 ) // new feature testing
void INIT(void) {
	// delay init
	DelayInit();

	// GPIO
	GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 8, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 9, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 4, 1);
	GPIO_WriteBit(HW_GPIOD, 5, 0);
	GPIO_WriteBit(HW_GPIOD, 8, 1);
	GPIO_WriteBit(HW_GPIOD, 9, 1);

	//PIT
	PIT_InitTypeDef PIT_InitStruct;
	PIT_InitStruct.chl = HW_PIT_CH1;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	PIT_CallbackInstall(HW_PIT_CH1, PIT1_ISR);
	PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, false);

	// UART
	UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
	UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	printFlag = 0;

	// OV7725
	ov7725_Init(OV7725_I2C_INSTANCE);

	// // DMA for img data
	// DMA_InitTypeDef DMA_InitStruct;

	// DMA_InitStruct.chl = HW_DMA_CH0;
	// DMA_InitStruct.chlTriggerSource = MUX0_DMAREQ; // always enable
	// DMA_InitStruct.minorLoopByteCnt = (int32_t)OV7725_H*(OV7725_W/8);
	// DMA_InitStruct.majorLoopCnt = 1;
	// DMA_InitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

	// DMA_InitStruct.sAddr = (uint32_t)imgRaw;
	// DMA_InitStruct.sAddrOffset = 1;
	// DMA_InitStruct.sLastAddrAdj = (int32_t)(-OV7725_H*(OV7725_W/8)); // loop back
	// DMA_InitStruct.sDataWidth = kDMA_DataWidthBit_8;
	// DMA_InitStruct.sMod = kDMA_ModuloDisable;

	// DMA_InitStruct.dAddr = (uint32_t)printBuffer;
	// DMA_InitStruct.dAddrOffset = 1;
	// DMA_InitStruct.dLastAddrAdj = (int32_t)(-OV7725_H*(OV7725_W/8)); // loop back
	// DMA_InitStruct.dDataWidth = kDMA_DataWidthBit_8;
	// DMA_InitStruct.dMod = kDMA_ModuloDisable;

	// DMA_Init(&DMA_InitStruct);
	// DMA_EnableAutoDisableRequest(HW_DMA_CH0, true); // auto disable
	// DMA_DisableRequest(HW_DMA_CH0);
	// UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx, true); // DMA req on finished

	// for (uint32_t i = 0; i < OV7725_H*(OV7725_W/8); i++)
	// 	imgRaw[i] = i;

	// st7735r
	st7735r_Init(ST7735R_SPI_INSTANCE);
	st7735r_FillColor(BLACK);

	st7735r_SetActiveRegion(0, 79, 0, 59);
}
#else
void INIT(void) {
	// LED
	GPIO_QuickInit(LED_PORT, LED1_PIN, kGPIO_Mode_OPP);
	GPIO_QuickInit(LED_PORT, LED2_PIN, kGPIO_Mode_OPP);
	GPIO_QuickInit(LED_PORT, LED3_PIN, kGPIO_Mode_OPP);
	GPIO_QuickInit(LED_PORT, LED4_PIN, kGPIO_Mode_OPP);
	GPIO_WriteBit(LED_PORT, LED1_PIN, 1);
	GPIO_WriteBit(LED_PORT, LED2_PIN, 1);
	GPIO_WriteBit(LED_PORT, LED3_PIN, 1);
	GPIO_WriteBit(LED_PORT, LED4_PIN, 1);

	// Button
	GPIO_QuickInit(BUTTON_PORT, BUTTON1_PIN, kGPIO_Mode_IPU);
	GPIO_QuickInit(BUTTON_PORT, BUTTON2_PIN, kGPIO_Mode_IPU);
	GPIO_QuickInit(BUTTON_PORT, BUTTON3_PIN, kGPIO_Mode_IPU);
	GPIO_QuickInit(BUTTON_PORT, BUTTON4_PIN, kGPIO_Mode_IPU);

	GPIO_ITDMAConfig(BUTTON_PORT, BUTTON1_PIN, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(BUTTON_PORT, BUTTON2_PIN, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(BUTTON_PORT, BUTTON3_PIN, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(BUTTON_PORT, BUTTON4_PIN, kGPIO_IT_FallingEdge, true);

	// PIT
	PIT_InitTypeDef PIT_InitStruct;
	PIT_InitStruct.chl = HW_PIT_CH0;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	// PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
	
	// UART
	UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
	UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	printFlag = 0;
	
	// FTM (PWM)
	FTM_PWM_QuickInit(MOTOR_R_F_INSTANCE, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(MOTOR_R_B_INSTANCE, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(MOTOR_L_B_INSTANCE, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(MOTOR_L_F_INSTANCE, kPWM_EdgeAligned, 10000);
	
	FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
	FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
	FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
	FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);

	motorEnable = 0;
	
	// FTM (Decoder)
	FTM_QD_QuickInit(ENC_R_INSTANCE,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	FTM_QD_QuickInit(ENC_L_INSTANCE,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	
	// I2C, MPU6050
	DelayInit();
	uint8_t instance = I2C_QuickInit(MPU6050_I2C_INSTANCE, 100*1000);
	mpu6050_init((uint32_t) instance);
	struct mpu_config mpuConfig;
	mpuConfig.afs = AFS_4G;
	mpuConfig.gfs = GFS_500DPS;
	mpuConfig.aenable_self_test = false;
	mpuConfig.genable_self_test = false;
	mpuConfig.gbypass_blpf = false;
	mpu6050_config(&mpuConfig);

	// st7735r
	st7735r_Init(ST7735R_SPI_INSTANCE);
	st7735r_FillColor(BLACK);
}

void systemTest(void) {
	// LED test
	LED1 = 0;
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;

	// Button test
	GPIO_CallbackInstall(BUTTON_PORT, buttonTest_ISR);

	// UART test
	printf("UART init successfully\r");

	// PIT & mpu6050 & encoder test
	PIT_CallbackInstall(HW_PIT_CH0, PIT_test);
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);

}

void buttonTest_ISR(uint32_t array) {
	static int32_t state = -4;
	if ((array>>BUTTON1_PIN)&1U) {			// button1
		LED1 = !LED1;
		if (state == -4) state++;
		else if (state < 0) state = -4;
		else state = 0;
	}
	else if ((array>>BUTTON2_PIN)&1U) {		// button2
		LED2 = !LED2;
		if (state == -3) state++;
		else if (state < 0) state = -4;
		else if (state == 0) {				// motor test
			motorTest(MOTOR_L);
			DelayMs(500);
			motorTest(MOTOR_R);
			DelayMs(500);
		}
	}
	else if ((array>>BUTTON3_PIN)&1U) {		// button3
		LED3 = !LED3;
		if (state == -2) state++;
		else if (state < 0) state = -4;
	}
	else if ((array>>BUTTON4_PIN)&1U) {		// button4
		LED4 = !LED4;
		if (state == -1) state++;
		else if (state < 0) state = -4;
		else if (state == 0 || state == 1 || state == 2) state++;
		else if (state == 3) {				// end of test
			state++;
			PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
			PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
			GPIO_CallbackInstall(BUTTON_PORT, GPIO_DUMMY);
			st7735r_FillColor(BLACK);

			// start main loop
			controlInit();
			PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
		}
	}
	if (!((array>>BUTTON4_PIN)&1U) || state != 4) st7735r_Print(0, 9, GREEN, BLACK, "status %d", state);		// st7735r test
}

void PIT_test(void) {
	static uint16_t TIME_CNT = 0;
	TIME_CNT++;
	if (TIME_CNT == 20) {
		TIME_CNT = 0;
	}
	switch (TIME_CNT) {
		case 0:		// print encoder data
		mpu6050_read_accel(accel);
		mpu6050_read_gyro(gyro);
		st7735r_Print(0, 0, GREEN, BLACK, "GX%6d", gyro[GX&0x0F]);
		st7735r_Print(0, 1, GREEN, BLACK, "GY%6d", gyro[GY&0x0F]);
		st7735r_Print(0, 2, GREEN, BLACK, "GZ%6d", gyro[GZ&0x0F]);
		st7735r_Print(0, 3, GREEN, BLACK, "AX%6d", accel[AX&0x0F]);
		st7735r_Print(0, 4, GREEN, BLACK, "AY%6d", accel[AY&0x0F]);
		st7735r_Print(0, 5, GREEN, BLACK, "AZ%6d", accel[AZ&0x0F]);
		break;

		case 10:		// print mpu data
		st7735r_Print(0, 6, GREEN, BLACK, "L%6d", getEncoder(ENC_L));
		st7735r_Print(0, 7, GREEN, BLACK, "R%6d", getEncoder(ENC_R));
		break;

		default:
			;
	}
}
#endif // MAIN_DEBUG

uint8_t ov7725_Init(uint32_t I2C_MAP) {
	// set DMA channel interrupt to higher priority
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_SetPriority(DMA1_DMA17_IRQn, NVIC_EncodePriority(NVIC_PriorityGroup_1, 1, 1));
	NVIC_SetPriority(OV7725_VSYNC_IRQ, NVIC_EncodePriority(NVIC_PriorityGroup_2, 2, 2));

	// sccb bus init
	uint32_t instance = I2C_QuickInit(I2C_MAP, 100*1000);
	uint8_t err = ov7725_probe(instance);
	if (err) return err;

	// set image size
	ov7725_set_image_size(H_80_W_60);

	// ctrl pin init
	GPIO_QuickInit(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_Mode_IPD);
	GPIO_QuickInit(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_Mode_IPD);
	PORT_PinPassiveFilterConfig(OV7725_CTRL_PORT, OV7725_PCLK_PIN, true);
	PORT_PinPassiveFilterConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, true);

	// interrupt & DMA config
	GPIO_CallbackInstall(OV7725_CTRL_PORT, ov7725_ISR);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_DMA_FallingEdge, false);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, false);

	// data pin init
	for (uint8_t i = 0; i < 8; i++) {
		GPIO_QuickInit(OV7725_DATA_PORT,
			OV7725_DATA_PIN_OFFSET+i, kGPIO_Mode_IFT);
	}

	// init pointers to buffers
	imgRaw = imgBuffer1;
	imgBuffer = imgBuffer2;

	// DMA for data transmit
	DMA_InitTypeDef DMA_InitStruct;

	DMA_InitStruct.chl = HW_DMA_CH1;
	DMA_InitStruct.chlTriggerSource = OV7725_DMAREQ_SRC;
	DMA_InitStruct.minorLoopByteCnt = 1;
	DMA_InitStruct.majorLoopCnt = (int32_t)(OV7725_H*OV7725_W/8);
	DMA_InitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

	DMA_InitStruct.sAddr = (uint32_t)&OV7725_DATA_PT->PDIR + OV7725_DATA_PIN_OFFSET/8; // PDIR of OV7725_CTRL_PORT
	DMA_InitStruct.sAddrOffset = 0;
	DMA_InitStruct.sLastAddrAdj = 0;
	DMA_InitStruct.sDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.sMod = kDMA_ModuloDisable;

	DMA_InitStruct.dAddr = (uint32_t)imgBuffer;
	DMA_InitStruct.dAddrOffset = 1;
	DMA_InitStruct.dLastAddrAdj = -(int32_t)(OV7725_H*OV7725_W/8);// 0;
	DMA_InitStruct.dDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.dMod = kDMA_ModuloDisable;

	DMA_Init(&DMA_InitStruct);
	DMA_EnableAutoDisableRequest(HW_DMA_CH1, true);
	DMA_DisableRequest(HW_DMA_CH1);

	DMA_CallbackInstall(HW_DMA_CH1, ov7725_DMA_Complete_ISR);
	DMA_ITConfig(HW_DMA_CH1, kDMA_IT_Major, true);

	return 0;
}

void ov7725_ISR(uint32_t array) {
	// start transfer
	DMA_EnableRequest(HW_DMA_CH1);
	// disable intterrupt
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, false);

	GPIO_ToggleBit(HW_GPIOD, 8);

}

void ov7725_DMA_Complete_ISR(void) {
	// disable transfer
	DMA_DisableRequest(HW_DMA_CH1);

	st7735r_PlotImg(WHITE,BLACK,imgBuffer,OV7725_H*(OV7725_W/8));
	GPIO_ToggleBit(HW_GPIOD, 9);

	// enable port interrupt for next transfer
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, true);
}

// void extractImage(void) {
// 	for (uint32_t i = 0; i < OV7725_H*(OV7725_W/8); i++) {
// 		uint32_t val = i << 3;
// 		for (uint8_t j = 0; j < 8; j++) {
// 			img[val+j] = ((imgRaw[i] >> (7-j)) & 0x01) + '0';
// 		}
// 	}
// }

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
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		return;
	}

	if (pwmDuty > 9000) pwmDuty = 9000;
	if (pwmDuty < -9000) pwmDuty = -9000;

	switch (id) {
		case MOTOR_L:
		if (pwmDuty > 0) {
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, (uint32_t)pwmDuty);
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0u);
		}
		else {
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0u);
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, (uint32_t)(-pwmDuty));
		}
		break; // MOTOR_L

		case MOTOR_R:
		if (pwmDuty > 0) {
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, (uint32_t)pwmDuty);
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0u);
		}
		else {
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0u);
			FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, (uint32_t)(-pwmDuty));
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

void motorTest(uint32_t id) {
	switch (id) {
		case MOTOR_L:
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 3000);
		for (int32_t i = 0; i < 10000000; i++);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 3000);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		for (int32_t i = 0; i < 10000000; i++);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		break;

		case MOTOR_R:
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 3000);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		for (int32_t i = 0; i < 10000000; i++);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 3000);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		for (int32_t i = 0; i < 10000000; i++);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_F, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_R_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_B, 0);
		FTM_PWM_ChangeDuty(MOTOR_FTM_MODULE, MOTOR_L_F, 0);
		break;

		default:
			;
	}
}

void GPIO_DUMMY(uint32_t array) {
	return;
}
