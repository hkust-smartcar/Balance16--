/** util.c **
 * peripheral init
 * setEncoder & getMotor
 * print data
 */

#include "board.h"
#include "global.h"
#include "util.h"
#include "main.h"

#if ( MAIN_DEBUG == 1 ) // new feature testing
void INIT(void) {
	// GPIO
	GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 8, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 9, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 4, 1);
	GPIO_WriteBit(HW_GPIOD, 5, 0);
	GPIO_WriteBit(HW_GPIOD, 8, 1);
	GPIO_WriteBit(HW_GPIOD, 9, 1);

	GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_IPU);
	GPIO_QuickInit(HW_GPIOA, 5, kGPIO_Mode_IPU);
	GPIO_QuickInit(HW_GPIOA, 6, kGPIO_Mode_IPU);
	// GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU);
	// GPIO_CallbackInstall(HW_GPIOA, Button_Handler);
	// GPIO_ITDMAConfig(HW_GPIOA, 4, kGPIO_IT_FallingEdge, true);
	// GPIO_ITDMAConfig(HW_GPIOA, 5, kGPIO_IT_FallingEdge, true);
	// GPIO_ITDMAConfig(HW_GPIOA, 6, kGPIO_IT_FallingEdge, true);
	// GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);

	//PIT
	PIT_InitTypeDef PIT_InitStruct;
	PIT_InitStruct.chl = HW_PIT_CH1;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	PIT_CallbackInstall(HW_PIT_CH1, PIT1_ISR);
	PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, false);

	// UART
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
	
	printFlag = 0;

	// OV7725
	ov7725_Init(I2C1_SCL_PC10_SDA_PC11);

	// DMA for img data
	DMA_InitTypeDef DMA_InitStruct;

	DMA_InitStruct.chl = HW_DMA_CH0;
	DMA_InitStruct.chlTriggerSource = MUX0_DMAREQ; // always enable
	DMA_InitStruct.minorLoopByteCnt = (int32_t)OV7725_H*(OV7725_W/8);
	DMA_InitStruct.majorLoopCnt = 1;
	DMA_InitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

	DMA_InitStruct.sAddr = (uint32_t)imgRaw;
	DMA_InitStruct.sAddrOffset = 1;
	DMA_InitStruct.sLastAddrAdj = (int32_t)(-OV7725_H*(OV7725_W/8)); // loop back
	DMA_InitStruct.sDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.sMod = kDMA_ModuloDisable;

	DMA_InitStruct.dAddr = (uint32_t)printBuffer;
	DMA_InitStruct.dAddrOffset = 1;
	DMA_InitStruct.dLastAddrAdj = (int32_t)(-OV7725_H*(OV7725_W/8)); // loop back
	DMA_InitStruct.dDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.dMod = kDMA_ModuloDisable;

	DMA_Init(&DMA_InitStruct);
	DMA_EnableAutoDisableRequest(HW_DMA_CH0, true); // auto disable
	DMA_DisableRequest(HW_DMA_CH0);
	// UART_ITDMAConfig(HW_UART0, kUART_DMA_Tx, true); // DMA req on finished

	// for (uint32_t i = 0; i < OV7725_H*(OV7725_W/8); i++)
	// 	imgRaw[i] = i;

	// st7735r
	st7735r_Init(SPI0_SCK_PC05_SOUT_PC06_SIN_PC07);
	st7735r_FillColor(BLACK);
	
	// st7735r_PlotImg(0,OV7725_W-1,0,OV7725_H-1,WHITE,BLACK,imgRaw,OV7725_H*(OV7725_W/8));
}
#else
void INIT(void) {
	// GPIO
	GPIO_QuickInit(HW_GPIOE, 12, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 11, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOE, 12, 1);
	GPIO_WriteBit(HW_GPIOE, 11, 1);

	// PIT
	PIT_InitTypeDef PIT_InitStruct;
	PIT_InitStruct.chl = HW_PIT_CH0;
	PIT_InitStruct.timeInUs = 1000*1;
	PIT_Init(&PIT_InitStruct);
	
	PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
	
	// UART
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
	
	printFlag = 0;
	
	// FTM (PWM)
	FTM_PWM_QuickInit(FTM3_CH0_PD00, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH1_PD01, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH2_PD02, kPWM_EdgeAligned, 10000);
	FTM_PWM_QuickInit(FTM3_CH3_PD03, kPWM_EdgeAligned, 10000);
	
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH0, 0);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH1, 0);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH2, 0);
	FTM_PWM_ChangeDuty(HW_FTM3, HW_FTM_CH3, 0);

	motorEnable = 0;
	
	// FTM (Decoder)
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	FTM_QD_QuickInit(FTM2_QD_PHA_PA10_PHB_PA11,
		kFTM_QD_NormalPolarity, kQD_PHABEncoding);
	
	// I2C, MPU6050
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
}
#endif // MAIN_DEBUG

static void Button_Handler(uint32_t array) {
	GPIO_ITDMAConfig(HW_GPIOA, 4, kGPIO_IT_FallingEdge, false);
	GPIO_ITDMAConfig(HW_GPIOA, 5, kGPIO_IT_FallingEdge, false);
	GPIO_ITDMAConfig(HW_GPIOA, 6, kGPIO_IT_FallingEdge, false);
	// GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, false);

	if ((array>>4)&1U) {
		st7735r_FillColor(BLACK);
		LED3 = !LED3;
	}
	else if ((array>>5)&1U) {
		GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, false);
		GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);
		ov7725BufferState = OV7725_BUFFER_LOCKED;
		ov7725TransferState = OV7725_PENDING;
		st7735r_PlotImg(0,OV7725_W-1,0,OV7725_H-1,WHITE,BLACK,imgRaw,OV7725_H*(OV7725_W/8));
		LED4 = !LED4;
		ov7725BufferState = OV7725_BUFFER_UNLOCKED;
		GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
		GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_IT_FallingEdge, true);
	}
	// else if ((array>>6)&1U) {
	// 	// ov7725BufferState = OV7725_BUFFER_LOCKED;
	// 	DMA_SetSourceAddress(HW_DMA_CH0, (uint32_t)imgRaw);
	// 	DMA_EnableRequest(HW_DMA_CH0);
	// }

	GPIO_ITDMAConfig(HW_GPIOA, 4, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(HW_GPIOA, 5, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(HW_GPIOA, 6, kGPIO_IT_FallingEdge, true);
	// GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
}

uint8_t ov7725_Init(uint32_t I2C_MAP) {
	// sccb bus init
	uint32_t instance = I2C_QuickInit(I2C_MAP, 100*1000);
	uint8_t err = ov7725_probe(instance);
	if (err) return err;

	// set image size
	ov7725_set_image_size(H_80_W_60);

	// buffer state and transfer state init
	ov7725BufferState = OV7725_BUFFER_UNLOCKED;
	ov7725TransferState = OV7725_PENDING;

	// ctrl pin init
	GPIO_QuickInit(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_Mode_IFT);
	GPIO_QuickInit(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_Mode_IFT);
	GPIO_QuickInit(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_Mode_IFT);

	// interrupt & DMA config
	GPIO_CallbackInstall(OV7725_CTRL_PORT, ov7725_ISR);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, false);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);

	//data pin init
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
	DMA_InitStruct.chlTriggerSource = PORTB_DMAREQ; // OV7725_CTRL_PORT DMA request
	DMA_InitStruct.minorLoopByteCnt = 1;
	DMA_InitStruct.majorLoopCnt = (int32_t)OV7725_W/8;
	DMA_InitStruct.triggerSourceMode = kDMA_TriggerSource_Normal;

	DMA_InitStruct.sAddr = (uint32_t)&PTB->PDIR + OV7725_DATA_PIN_OFFSET/8; // PDIR of OV7725_CTRL_PORT
	DMA_InitStruct.sAddrOffset = 0;
	DMA_InitStruct.sLastAddrAdj = 0;
	DMA_InitStruct.sDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.sMod = kDMA_ModuloDisable;

	DMA_InitStruct.dAddr = (uint32_t)imgBuffer;
	DMA_InitStruct.dAddrOffset = 1;
	DMA_InitStruct.dLastAddrAdj = 0;
	DMA_InitStruct.dDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct.dMod = kDMA_ModuloDisable;

	DMA_Init(&DMA_InitStruct);
	DMA_EnableAutoDisableRequest(HW_DMA_CH1, true);
	DMA_DisableRequest(HW_DMA_CH1);

	return 0;
}

void ov7725_ISR(uint32_t array) {
	static uint8_t rowCnt = 0;

	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, false);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_IT_FallingEdge, false);

	// if ((array>>OV7725_VSYNC_PIN)&1U) {			// VSYNC
	// 	// printf("\r\rV");
	// 	rowCnt = 0;
	// 	uint8_t* tmp = imgRaw;
	// 	imgRaw = imgBuffer;
	// 	imgBuffer = tmp;
	// 	DMA_SetDestAddress(HW_DMA_CH1, (uint32_t)imgBuffer);
	// }
	// else if ((array>>OV7725_HREF_PIN)&1U) {		// HREF
	// 	// printf("H");
	// 	if (rowCnt < OV7725_H) {
	// 		rowCnt++;
	// 		DMA_EnableRequest(HW_DMA_CH1);
	// 	}
	// }

	// switch (ov7725TransferState) {
	// 	case OV7725_PENDING:
	// 	switch (ov7725BufferState) {
	// 		case OV7725_BUFFER_LOCKED:
	// 		// do nothing
	// 		break; // OV7725_BUFFER_LOCKED

	// 		case OV7725_BUFFER_UNLOCKED:
	// 		if ((array>>OV7725_VSYNC_PIN)&1U) {
	// 			// update state
	// 			ov7725TransferState = OV7725_IN_PROCESS;

	// 			// clear rowCnt
	// 			rowCnt = 0;

	// 			// // exchange back and front buffer
	// 			// uint8_t* tmp = imgRaw;
	// 			// imgRaw = imgBuffer;
	// 			// imgBuffer = tmp;

	// 			// set DMA dest addr to new buffer
	// 			DMA_SetDestAddress(HW_DMA_CH1, (uint32_t)imgBuffer);
	// 		}
	// 		else if ((array>>OV7725_HREF_PIN)&1U) {
	// 			// do nothing
	// 		}
	// 		break; //OV7725_BUFFER_UNLOCKED

	// 		default:
	// 		;
	// 	}
	// 	break; // OV7725_PENDING

	// 	case OV7725_IN_PROCESS:
	// 	// switch (ov7725BufferState) {
	// 	// 	case OV7725_BUFFER_LOCKED:
	// 	// 	if ((array>>OV7725_VSYNC_PIN)&1U) {
				
	// 	// 	}
	// 	// 	else if ((array>>OV7725_HREF_PIN)&1U) {
				
	// 	// 	}
	// 	// 	break; // OV7725_BUFFER_LOCKED

	// 	// 	case OV7725_BUFFER_UNLOCKED:
	// 	// 	if ((array>>OV7725_VSYNC_PIN)&1U) {
				
	// 	// 	}
	// 	// 	else if ((array>>OV7725_HREF_PIN)&1U) {
				
	// 	// 	}
	// 	// 	break; //OV7725_BUFFER_UNLOCKED

	// 	// 	default:
	// 	// 	;
	// 	// }
	// 	if ((array>>OV7725_VSYNC_PIN)&1U) {
	// 		// do nothing
	// 	}
	// 	else if ((array>>OV7725_HREF_PIN)&1U) {
	// 		// increase rowCnt and start DMA
	// 		rowCnt++;
	// 		DMA_EnableRequest(HW_DMA_CH1);

	// 		// finished?
	// 		if (rowCnt == OV7725_H) {
	// 			// update state
	// 			ov7725TransferState = OV7725_PENDING;

	// 			st7735r_PlotImg(0,OV7725_W-1,0,OV7725_H-1,WHITE,BLACK,imgRaw,OV7725_H*(OV7725_W/8));
	// 		}
	// 	}
	// 	break; // OV7725_IN_PROCESS

	// 	default:
	// 	;
	// }
	switch (ov7725TransferState) {
		case OV7725_PENDING:
			if ((array>>OV7725_VSYNC_PIN)&1U) {
				// update state
				ov7725TransferState = OV7725_IN_PROCESS;

				// clear rowCnt
				rowCnt = 0;

				// set DMA dest addr to new buffer
				DMA_SetDestAddress(HW_DMA_CH1, (uint32_t)imgBuffer);
			}
			else if ((array>>OV7725_HREF_PIN)&1U) {
				// do nothing
			}
		break; // OV7725_PENDING

		case OV7725_IN_PROCESS:
		if ((array>>OV7725_VSYNC_PIN)&1U) {
			// do nothing
		}
		else if ((array>>OV7725_HREF_PIN)&1U) {
			// increase rowCnt and start DMA
			rowCnt++;
			DMA_EnableRequest(HW_DMA_CH1);

			// finished?
			if (rowCnt == OV7725_H) {
				// update state
				ov7725TransferState = OV7725_PENDING;
				
				st7735r_PlotImg(0,OV7725_W-1,0,OV7725_H-1,WHITE,BLACK,imgBuffer,OV7725_H*(OV7725_W/8));
			}
		}
		break; // OV7725_IN_PROCESS

		default:
		;
	}


	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_VSYNC_PIN, kGPIO_IT_FallingEdge, true);
	GPIO_ITDMAConfig(OV7725_CTRL_PORT, OV7725_HREF_PIN, kGPIO_IT_FallingEdge, true);
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
