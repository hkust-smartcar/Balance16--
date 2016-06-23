// st7735r library
// under testing

#include "common.h"
#include "st7735r.h"
#include "font.h"

#include <stdio.h>
#include <stdarg.h>

static uint8_t CURR_XS = 0;
static uint8_t CURR_XE = MAX_WIDTH-1;
static uint8_t CURR_YS = 0;
static uint8_t CURR_YE = MAX_HEIGHT-1;

void st7735r_Init(uint32_t instance) {
	// GPIO Init
	GPIO_QuickInit(ST7735R_DC_PORT, ST7735R_DC_PIN, kGPIO_Mode_OPP);
	GPIO_QuickInit(ST7735R_RST_PORT, ST7735R_RST_PIN, kGPIO_Mode_OPP);

	// SPI CS PinMux
	PORT_PinMuxConfig(ST7735R_CS_PORT, ST7735R_CS_PIN, kPinAlt2);

	// SPI init
	// maximum clk frequency 15MHz, see datasheet
	SPI_QuickInit(instance, kSPI_CPOL1_CPHA1, 15*1000*1000);

	// reset st7735r
	st7735r_Reset();

	// send startup command
	st7735r_Startup();
}

// clear the screen
void st7735r_FillColor(uint16_t color) {
	st7735r_WriteCmd(0x2A);		// Column addr set
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);	// X START
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x7F);	// X END

	st7735r_WriteCmd(0x2B);		// Row addr set
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);	// Y START
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x9F);	// Y END

	st7735r_WriteCmd(0x2C);		// write to RAM
	
	for (uint16_t i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		st7735r_WriteData(color >> 8);
		st7735r_WriteData(color);
	}
}

// (x, y) is global coordinate, not relative to current active region
void st7735r_PutPixel(uint8_t x, uint8_t y, uint16_t color) {
	st7735r_SetPixelPos(x, y);
	st7735r_WriteData(color >> 8);
	st7735r_WriteData(color);
}

void st7735r_SetPixelPos(uint8_t x, uint8_t y) {
	st7735r_WriteCmd(0x2A);		// Column addr set
	st7735r_WriteData(0x00);
	st7735r_WriteData(x);		// X START
	st7735r_WriteData(0x00);
	st7735r_WriteData(x);		// X END

	st7735r_WriteCmd(0x2B);		// Row addr set
	st7735r_WriteData(0x00);
	st7735r_WriteData(y);		// Y START
	st7735r_WriteData(0x00);
	st7735r_WriteData(y);		// Y END

	st7735r_WriteCmd(0x2C);		// write to RAM
}

void st7735r_PlotImg(uint16_t color_f, uint16_t color_t, uint8_t* data, uint32_t len) {

	st7735r_WriteCmd(0x2C);		// write to RAM

	for (uint32_t i = 0; i < len; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			if (!((data[i]>>(7-j))&1u)) {
				st7735r_WriteData(color_f >> 8);
				st7735r_WriteData(color_f);
			}
			else {
				st7735r_WriteData(color_t >> 8);
				st7735r_WriteData(color_t);
			}
		}
	}
}

// fill the current active region
void st7735r_FillRegion(uint16_t color) {
	st7735r_WriteCmd(0x2C);		// write to RAM

	uint8_t w = CURR_XE-CURR_XS+1;
	uint8_t h = CURR_YE-CURR_YS+1;
	
	for (uint16_t i = 0; i < w*h; i++) {
		st7735r_WriteData(color >> 8);
		st7735r_WriteData(color);
	}
}

void st7735r_SetActiveRegion(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye) {
	if (xe >= MAX_WIDTH) xe = MAX_WIDTH-1;
	if (ye >= MAX_HEIGHT) ye = MAX_HEIGHT-1;
	if (xs > xe) xs = xe;
	if (ys > ye) ys = ye;

	CURR_XS = xs;
	CURR_XE = xe;
	CURR_YS = ys;
	CURR_YE = ye;

	st7735r_UpdateActiveRegion();
}

// (x, y) is global
void st7735r_PutChar(uint8_t x, uint8_t y, uint8_t ch, uint16_t textColor, uint16_t bgColor) {
	// record last active region
	uint8_t last_xs = CURR_XS;
	uint8_t last_xe = CURR_XE;
	uint8_t last_ys = CURR_YS;
	uint8_t last_ye = CURR_YE;

	// set new active region
	if (x >= CHAR_MAX_X) x = CHAR_MAX_X-1;
	if (y >= CHAR_MAX_Y) y = CHAR_MAX_Y-1;
	uint16_t xs = x*CHAR_WIDTH;
	uint16_t ys = y*CHAR_HEIGHT;
	st7735r_SetActiveRegion(xs, xs+CHAR_WIDTH-1, ys, ys+CHAR_HEIGHT-1);
	uint32_t index = (uint32_t)ch;
	index <<= 4;
	st7735r_PlotImg(bgColor, textColor, (uint8_t*)&(ascii_8x16[index]), CHAR_HEIGHT);

	// set active region to original
	st7735r_SetActiveRegion(last_xs, last_xe, last_ys, last_ye);
}

// print a const str on a line
void st7735r_PutLine(uint8_t x, uint8_t y, uint8_t* str, uint16_t textColor, uint16_t bgColor) {
	uint8_t i = x, j = 0;
	while (i < CHAR_MAX_X && str[j]) {
		st7735r_PutChar(i, y, str[j], textColor, bgColor);
		i++;
		j++;
	}
}

// printf like function, only one line, the rest are truncated
void st7735r_Print(uint8_t x, uint8_t y, uint16_t textColor, uint16_t bgColor, const uint8_t* str, ...){
	uint8_t buffer[32];

	va_list arglist;
	va_start(arglist, str);
	vsprintf((char*)buffer, (const char*)str, arglist);
	va_end(arglist);

	st7735r_PutLine(x, y, buffer, textColor, bgColor);
}

void st7735r_UpdateActiveRegion(void) {
	st7735r_WriteCmd(0x2A);			// Column addr set
	st7735r_WriteData(0x00);
	st7735r_WriteData(CURR_XS);		// X START
	st7735r_WriteData(0x00);
	st7735r_WriteData(CURR_XE);		// X END

	st7735r_WriteCmd(0x2B);			// Row addr set
	st7735r_WriteData(0x00);
	st7735r_WriteData(CURR_YS);		// Y START
	st7735r_WriteData(0x00);
	st7735r_WriteData(CURR_YE);		// Y END
}

void st7735r_WriteCmd(uint8_t cmd) {
	// set dc to cmd
	GPIO_ResetBit(ST7735R_DC_PORT, ST7735R_DC_PIN);

	// send cmd
	SPI_ReadWriteByte(ST7735R_SPI, HW_CTAR0, (uint16_t)cmd,
		ST7735R_SPI_CS, kSPI_PCS_ReturnInactive);
}

void st7735r_WriteData(uint8_t data) {
	// set dc to data
	GPIO_SetBit(ST7735R_DC_PORT, ST7735R_DC_PIN);

	// send data
	SPI_ReadWriteByte(ST7735R_SPI, HW_CTAR0, (uint16_t)data,
		ST7735R_SPI_CS, kSPI_PCS_ReturnInactive);
}

void st7735r_Reset(void) {
	GPIO_ResetBit(ST7735R_RST_PORT, ST7735R_RST_PIN);
	for (uint16_t i = 0; i < 10000; i++);
	GPIO_SetBit(ST7735R_RST_PORT, ST7735R_RST_PIN);
	for (uint16_t i = 0; i < 10000; i++);
}

void st7735r_Startup(void) {
	st7735r_WriteCmd(0x01);	//Software setting
	// DelayMs(10);
	st7735r_WriteCmd(0x11);	//Sleep out
	// DelayMs(120);
	
	//ST7735R Frame Rate
	st7735r_WriteCmd(0xB1);
	st7735r_WriteData(0x01);
	st7735r_WriteData(0x2C);
	st7735r_WriteData(0x2D);
	st7735r_WriteCmd(0xB2);
	st7735r_WriteData(0x01);
	st7735r_WriteData(0x2C);
	st7735r_WriteData(0x2D);
	st7735r_WriteCmd(0xB3);
	st7735r_WriteData(0x01);
	st7735r_WriteData(0x2C);
	st7735r_WriteData(0x2D);
	st7735r_WriteData(0x01);
	st7735r_WriteData(0x2C);
	st7735r_WriteData(0x2D);
	//------------------------------------End ST7735R Frame Rate-----------------------------------------//
	st7735r_WriteCmd(0xB4);//Column inversion
	st7735r_WriteData(0x07);
	//------------------------------------ST7735R Power Sequence-----------------------------------------//
	st7735r_WriteCmd(0xC0);
	st7735r_WriteData(0xA2);
	st7735r_WriteData(0x02);
	st7735r_WriteData(0x84);
	st7735r_WriteCmd(0xC1);
	st7735r_WriteData(0xC5);
	st7735r_WriteCmd(0xC2);
	st7735r_WriteData(0x0A);
	st7735r_WriteData(0x00);
	st7735r_WriteCmd(0xC3);
	st7735r_WriteData(0x8A);
	st7735r_WriteData(0x2A);
	st7735r_WriteCmd(0xC4);
	st7735r_WriteData(0x8A);
	st7735r_WriteData(0xEE);
	//---------------------------------End ST7735R Power Sequence-------------------------------------//
	st7735r_WriteCmd(0xC5);	//VCOM
	st7735r_WriteData(0x0E);
	st7735r_WriteCmd(0x36);	//MX, MY, RGB mode
	st7735r_WriteData(0xC8);
	//------------------------------------ST7735R Gamma Sequence-----------------------------------------//
	st7735r_WriteCmd(0xe0);
	st7735r_WriteData(0x02);
	st7735r_WriteData(0x1c);
	st7735r_WriteData(0x07);
	st7735r_WriteData(0x12);
	st7735r_WriteData(0x37);
	st7735r_WriteData(0x32);
	st7735r_WriteData(0x29);
	st7735r_WriteData(0x2d);
	st7735r_WriteData(0x29);
	st7735r_WriteData(0x25);
	st7735r_WriteData(0x2b);
	st7735r_WriteData(0x39);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x01);
	st7735r_WriteData(0x03);
	st7735r_WriteData(0x10);
	st7735r_WriteCmd(0xe1);
	st7735r_WriteData(0x03);
	st7735r_WriteData(0x1d);
	st7735r_WriteData(0x07);
	st7735r_WriteData(0x06);
	st7735r_WriteData(0x2e);
	st7735r_WriteData(0x2c);
	st7735r_WriteData(0x29);
	st7735r_WriteData(0x2d);
	st7735r_WriteData(0x2e);
	st7735r_WriteData(0x2e);
	st7735r_WriteData(0x37);
	st7735r_WriteData(0x3f);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x02);
	st7735r_WriteData(0x10);
	st7735r_WriteCmd(0x2A);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x7f);

	st7735r_WriteCmd(0x2B);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x00);
	st7735r_WriteData(0x9f);
	//------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

	st7735r_WriteCmd(0x3A);
	st7735r_WriteData(0x05);  
	st7735r_WriteCmd(0x29);	//Display on
}
