// st7735r library
// under testing

#ifndef _ST7735R_H_
#define _ST7735R_H_

#include "common.h"
#include "gpio.h"
#include "spi.h"
#include "font.h"
#include "board.h"

#include <stdio.h>
#include <stdarg.h>


// Color
#define	BGR888_MODE		1

#if (!BGR888_MODE)
 #define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))
#else 
 #define	RGB888TO565(BGR888)  (((BGR888 >> 19) & 0x001F) |((BGR888 >> 5) & 0x07E0) | (((u32)BGR888 << 8) & 0xF800))
#endif

// minimize the MCU calculation
#define WHITE               0xFFFF//    (RGB888TO565(0xFFFFFF))
#define BLACK               0x0000//    (RGB888TO565(0x000000))
#define DARK_GREY           0x52AA//    (RGB888TO565(0x555555))
#define RED                 0x001F//    (RGB888TO565(0xAAAAAA))
#define GREY                0xAD55//    (RGB888TO565(0xFF0000))
#define ORANGE              0x04DF//    (RGB888TO565(0xFF9900))
#define YELLOW              0x07FF//    (RGB888TO565(0xFFFF00))
#define GREEN               0x07E0//    (RGB888TO565(0x00FF00))
#define DARK_GREEN          0x0660//    (RGB888TO565(0x00CC00))
#define BLUE                0xF800//    (RGB888TO565(0x0000FF))
#define BLUE2               0x6104//    (RGB888TO565(0x202060))
#define SKY_BLUE            0xFE62//    (RGB888TO565(0x11CFFF))
#define CYAN                0xFC51//    (RGB888TO565(0x8888FF))
#define PURPLE              0xAD40//    (RGB888TO565(0x00AAAA))
#define PINK                0xC5BF//    (RGB888TO565(0xFFB6C1))

#define MAX_WIDTH				128
#define MAX_HEIGHT				160
#define CHAR_WIDTH				8
#define CHAR_HEIGHT				16

#define CHAR_MAX_X_VERTICAL		16
#define CHAR_MAX_Y_VERTICAL		10

#define CHAR_MAX_X_HORIZONTAL	20
#define CHAR_MAX_Y_HORIZONTAL	8

#define CHAR_MAX_X				20		// max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
#define CHAR_MAX_Y				10		// max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL


// API functions
void st7735r_Init(uint32_t instance);
void st7735r_FillColor(uint16_t color);
void st7735r_PutPixel(uint8_t x, uint8_t y, uint16_t color);
void st7735r_FillRegion(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye, uint16_t color);
void st7735r_PlotImg(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye,
	uint16_t color_t, uint16_t color_f, uint8_t* data, uint32_t len);

// st7735r internal functions
void st7735r_SetPixelPos(uint8_t x, uint8_t y);

// st7735r control
void st7735r_WriteCmd(uint8_t cmd);
void st7735r_WriteData(uint8_t data);
void st7735r_Reset(void);
void st7735r_Startup(void);

#endif // _ST7735R_H_
