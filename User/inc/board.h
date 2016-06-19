/** board.h **
 * general information of mainboard
 */
#ifndef _USER_BOARD_H_
#define _USER_BOARD_H_

#define MAIN_DEBUG 1

// GPIO
#define LED1 PDout(4)
#define LED2 PDout(5)
#define LED3 PDout(8)
#define LED4 PDout(9)

// Encoder & Motor
#define ENC_L HW_FTM2
#define ENC_R HW_FTM1
#define MOTOR_L 0
#define MOTOR_R 1

// mpu6050
#define AX 0x00
#define AY 0x01
#define AZ 0x02
#define GX 0x10
#define GY 0x11
#define GZ 0x12

#define STABLE_ANGLE	0
#define TG				1
#define ANGLE_P			2
#define ANGLE_D			3
#define SPEED_I			4
#define SPEED_P			5
#define STR_REG_I		6
#define STR_REG_P		7
#define CONST_CNT		8

// ov7725
// image size
// 0: 60X80
// 1: 120X160
// 2: 180X240
// 3: 240X320
#define OV7725_IMG_SIZE 0

#if (OV7725_IMG_SIZE == 0)
	#define OV7725_W 80
	#define OV7725_H 60
#elif (OV7725_IMG_SIZE == 1)
	#define OV7725_W 160
	#define OV7725_H 120
#elif (OV7725_IMG_SIZE == 2)
	#define OV7725_W 240
	#define OV7725_H 180
#elif (OV7725_IMG_SIZE == 3)
	#define OV7725_W 320
	#define OV7725_H 240
#else
	#error "Invalid image size!"
#endif // OV7725_IMG_SIZE

// signal description
#define OV7725_CTRL_PORT HW_GPIOB
#define OV7725_PCLK_PIN 9
#define OV7725_VSYNC_PIN 10
#define OV7725_HREF_PIN 11
#define OV7725_DATA_PORT HW_GPIOB
#define OV7725_DATA_PIN_OFFSET 0

// st7735r signal description
#define ST7735R_DC_PORT HW_GPIOC
#define ST7735R_DC_PIN 2
#define ST7735R_CS_PORT HW_GPIOC
#define ST7735R_CS_PIN 4
#define ST7735R_SPI HW_SPI0
#define ST7735R_SPI_CS HW_SPI_CS0
#define ST7735R_RST_PORT HW_GPIOC
#define ST7735R_RST_PIN 3

#endif // _USER_BOARD_H_
