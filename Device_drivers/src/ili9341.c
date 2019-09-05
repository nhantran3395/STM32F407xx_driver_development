/**
*@file ili9341.h
*@brief provide functions for interfacing with ILI9341.LCD controller.
*
*This implementation file provide functions for interfacing with ILI9341 LCD controller.
*The functions in this library is mainly based on Tilen Majerle work.  I just modified so that these functions  will use my SPI driver library functions instead.
*
*@author Tran Thanh Nhan
*@date 03/09/2019
*
*@reference	https://stm32f4-discovery.net/2014/04/library-08-ili9341-lcd-on-stm32f429-discovery-board/
*/

#include "../inc/ili9341.h"

static void ILI9341_HW_init (void);
static void ILI9341_LCD_init (void);
static void ILI9341_send_command (uint8_t cmd);
static void ILI9341_send_parameter (uint8_t  param);
static void ILI9341_send_parameter_16_bits (uint16_t param);
static void ILI9341_set_active_area (uint16_t startColum, uint16_t startPage, uint16_t endColumn, uint16_t endPage);
static void ILI9341_delay(volatile uint32_t delay);
static void ILI9341_fill_area (uint16_t startColumn, uint16_t endColumn, uint16_t startPage, uint16_t endPage, uint16_t color);

ILI9341_Config_t ILI9341_config;
uint16_t ILI9341_x;
uint16_t ILI9341_y;

/***********************************************************************
Initilaize related hardware (GPIO pins, SPI peripheral and initilize display with default settings
***********************************************************************/
void ILI9341_init (void)
{
	ILI9341_HW_init();
	ILI9341_LCD_init();
	
	ILI9341_config.width = ILI9341_WIDTH;
	ILI9341_config.height = ILI9341_HEIGHT;
	ILI9341_config.orientation = ILI9341_orientation_landscape_1;
}

/***********************************************************************
Turn on display
***********************************************************************/
void ILI9341_display_on (void)
{
	ILI9341_send_command(ILI9341_DISPLAY_ON);
}

/***********************************************************************
Turn off display
***********************************************************************/
void ILI9341_display_off (void)
{
	ILI9341_send_command(ILI9341_DISPLAY_OFF);
}

/***********************************************************************
Draw pixel in specified (x,y) position
***********************************************************************/
void ILI9341_draw_pixel (uint16_t x, uint16_t y, uint16_t color)
{
	ILI9341_set_active_area(x,x,y,y);
	ILI9341_send_command(ILI9341_MEM_WRITE);
	ILI9341_send_parameter_16_bits(color);
}

/***********************************************************************
Fill entire display with specified color
***********************************************************************/
void ILI9341_fill_display (uint16_t color)
{
	ILI9341_fill_area(0x00,ILI9341_config.width - 1,0x00,ILI9341_config.height - 1,color);
}

/***********************************************************************
Draw monochrome image starting from specified (x,y) position
@note Based on drawBitmap function in Adafruit GFX Library
***********************************************************************/
void ILI9341_draw_bitmap (uint16_t x, uint16_t y, uint8_t *bitmapPtr, uint16_t w, uint16_t h, uint16_t color)
{
	uint8_t bytesInScanLine = (w+7)/8;
	uint8_t byte = 0;
	
	for(uint16_t i = 0; i < h; i++){
		for(uint16_t j = 0; j< w; j++){
			
			/*keep reading value of bits in byte. if finished reading previous byte, move to next byte*/
			if(j & 0x07){
				byte <<= 1;
			}else{
				byte = *(bitmapPtr + i*bytesInScanLine + j/8);
			}
			
			if(byte & 0x80){
				ILI9341_draw_pixel(x+j,y+i,color);
			}
		}
	}
}

/***********************************************************************
Draw 16-bit color image starting from specified (x,y) position
@note Based on drawRGBBitmap function in Adafruit GFX Library
***********************************************************************/
void ILI9341_draw_RGB_bitmap (uint16_t x, uint16_t y, uint16_t *bitmapPtr, uint16_t w, uint16_t h)
{	
	for(uint16_t i = 0; i < h; i++){
		for(uint16_t j = 0; j< w; j++){
			ILI9341_draw_pixel(x+i,y+j,*(bitmapPtr+i*w+j));
		}
	}
}

/***********************************************************************
Rotate LCD in specified orientation
***********************************************************************/
void ILI9341_rotate (ILI9341_Orientation_e orientation)
{
	ILI9341_send_command(ILI9341_MAC);
	if (orientation == ILI9341_orientation_portrait_1) {
		ILI9341_send_parameter(0x58);
	} else if (orientation == ILI9341_orientation_portrait_2) {
		ILI9341_send_parameter(0x88);
	} else if (orientation == ILI9341_orientation_landscape_1) {
		ILI9341_send_parameter(0x28);
	} else if (orientation == ILI9341_orientation_landscape_2) {
		ILI9341_send_parameter(0xE8);
	}
	
	if (orientation == ILI9341_orientation_portrait_1 || orientation == ILI9341_orientation_portrait_2) {
		ILI9341_config.width = ILI9341_WIDTH;
		ILI9341_config.height = ILI9341_HEIGHT;
	} else {
		ILI9341_config.width = ILI9341_HEIGHT;
		ILI9341_config.height = ILI9341_WIDTH;
	}
}

/***********************************************************************
Put character starting from (x,y) position
***********************************************************************/
void ILI9341_put_character (uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	
	if ((ILI9341_x + font->FontWidth) > ILI9341_config.width) {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}
	
	/* Draw rectangle for background */
	ILI9341_fill_area(ILI9341_x, ILI9341_x + font->FontWidth,ILI9341_y, ILI9341_y + font->FontHeight, background);
	
	/* Draw font data */
	for (i = 0; i < font->FontHeight; i++) {
		b = font->data[(c - 32) * font->FontHeight + i];
		for (j = 0; j < font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				ILI9341_draw_pixel(ILI9341_x + j, (ILI9341_y + i), foreground);
			}
		}
	}
	
	/* Set new pointer */
	ILI9341_x += font->FontWidth;
}

/***********************************************************************
Put string starting from (x,y) position
***********************************************************************/
void ILI9341_put_string(uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint16_t startX = x;
	
	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	
	while (*str) {
		/* New line */
		if (*str == '\n') {
			ILI9341_y += font->FontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				ILI9341_x = 0;
				str++;
			} else {
				ILI9341_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}
		
		/* Put character to LCD */
		ILI9341_put_character(ILI9341_x, ILI9341_y, *str++, font, foreground, background);
	}
}

/***********************************************************************
Private function: Initilize related hardware (SPI peripheral and GPIO pins)
***********************************************************************/
void ILI9341_HW_init (void)
{
	/*Initilize SPI peripheral*/
	SPI_general_init(ILI9341_SPI,ILI9341_SPI_PINS_PACK,SPI_MODE_MASTER,SPI_BUS_FULL_DUPLEX,SPI_DATA_8BITS,SPI_CLK_PHASE_1ST_E,SPI_CLK_POL_LIDLE,SPI_SSM_EN,SPI_CLK_SPEED_DIV2);
	SPI_SSI_ctr(ILI9341_SPI,ENABLE);
	
	/*Initilize CSX pin*/
	GPIO_init_direct(ILI9341_CSX_PORT,ILI9341_CSX_PIN,GPIO_MODE_OUT,GPIO_OUTPUT_TYPE_PP,GPIO_OUTPUT_LOW_SPEED,GPIO_PU,0);
	
	/*Initilize DCX pin*/
	GPIO_init_direct(ILI9341_DCX_PORT,ILI9341_DCX_PIN,GPIO_MODE_OUT,GPIO_OUTPUT_TYPE_PP,GPIO_OUTPUT_LOW_SPEED,GPIO_PU,0);

	/*Initilize RST pin*/
	GPIO_init_direct(ILI9341_RST_PORT,ILI9341_RST_PIN,GPIO_MODE_OUT,GPIO_OUTPUT_TYPE_PP,GPIO_OUTPUT_LOW_SPEED,GPIO_PU,0);

}

/***********************************************************************
Private function: Initilize display with default settings
***********************************************************************/
void ILI9341_LCD_init (void)
{
	/* Force reset */
	ILI9341_RST_CLEAR;
	ILI9341_delay(20000);
	ILI9341_RST_SET;
	
	/* Delay for RST response */
	ILI9341_delay(20000);
	
	/* Software reset */
	ILI9341_send_command(ILI9341_RESET);
	ILI9341_delay(50000);
	
	ILI9341_send_command(ILI9341_POWERA);
	ILI9341_send_parameter(0x39);
	ILI9341_send_parameter(0x2C);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x34);
	ILI9341_send_parameter(0x02);
	ILI9341_send_command(ILI9341_POWERB);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0xC1);
	ILI9341_send_parameter(0x30);
	ILI9341_send_command(ILI9341_DTCA);
	ILI9341_send_parameter(0x85);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x78);
	ILI9341_send_command(ILI9341_DTCB);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x00);
	ILI9341_send_command(ILI9341_POWER_SEQ);
	ILI9341_send_parameter(0x64);
	ILI9341_send_parameter(0x03);
	ILI9341_send_parameter(0x12);
	ILI9341_send_parameter(0x81);
	ILI9341_send_command(ILI9341_PRC);
	ILI9341_send_parameter(0x20);
	ILI9341_send_command(ILI9341_POWER1);
	ILI9341_send_parameter(0x23);
	ILI9341_send_command(ILI9341_POWER2);
	ILI9341_send_parameter(0x10);
	ILI9341_send_command(ILI9341_VCOM1);
	ILI9341_send_parameter(0x3E);
	ILI9341_send_parameter(0x28);
	ILI9341_send_command(ILI9341_VCOM2);
	ILI9341_send_parameter(0x86);
	ILI9341_send_command(ILI9341_MAC);
	ILI9341_send_parameter(0x48);
	ILI9341_send_command(ILI9341_PIXEL_FORMAT);
	ILI9341_send_parameter(0x55);
	ILI9341_send_command(ILI9341_FRC);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x18);
	ILI9341_send_command(ILI9341_DFC);
	ILI9341_send_parameter(0x08);
	ILI9341_send_parameter(0x82);
	ILI9341_send_parameter(0x27);
	ILI9341_send_command(ILI9341_3GAMMA_EN);
	ILI9341_send_parameter(0x00);
	ILI9341_send_command(ILI9341_COLUMN_ADDR);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0xEF);
	ILI9341_send_command(ILI9341_PAGE_ADDR);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x01);
	ILI9341_send_parameter(0x3F);
	ILI9341_send_command(ILI9341_GAMMA);
	ILI9341_send_parameter(0x01);
	ILI9341_send_command(ILI9341_PGAMMA);
	ILI9341_send_parameter(0x0F);
	ILI9341_send_parameter(0x31);
	ILI9341_send_parameter(0x2B);
	ILI9341_send_parameter(0x0C);
	ILI9341_send_parameter(0x0E);
	ILI9341_send_parameter(0x08);
	ILI9341_send_parameter(0x4E);
	ILI9341_send_parameter(0xF1);
	ILI9341_send_parameter(0x37);
	ILI9341_send_parameter(0x07);
	ILI9341_send_parameter(0x10);
	ILI9341_send_parameter(0x03);
	ILI9341_send_parameter(0x0E);
	ILI9341_send_parameter(0x09);
	ILI9341_send_parameter(0x00);
	ILI9341_send_command(ILI9341_NGAMMA);
	ILI9341_send_parameter(0x00);
	ILI9341_send_parameter(0x0E);
	ILI9341_send_parameter(0x14);
	ILI9341_send_parameter(0x03);
	ILI9341_send_parameter(0x11);
	ILI9341_send_parameter(0x07);
	ILI9341_send_parameter(0x31);
	ILI9341_send_parameter(0xC1);
	ILI9341_send_parameter(0x48);
	ILI9341_send_parameter(0x08);
	ILI9341_send_parameter(0x0F);
	ILI9341_send_parameter(0x0C);
	ILI9341_send_parameter(0x31);
	ILI9341_send_parameter(0x36);
	ILI9341_send_parameter(0x0F);
	ILI9341_send_command(ILI9341_SLEEP_OUT);

	ILI9341_delay(1000000);

	ILI9341_send_command(ILI9341_DISPLAY_ON);
}

/***********************************************************************
Private function: Send command
***********************************************************************/
void ILI9341_send_command (uint8_t cmd)
{
	ILI9341_DCX_CLEAR;
	ILI9341_CSX_CLEAR;
	SPI_send_8_bits(ILI9341_SPI,cmd);
//	ILI9341_Delay(10);
//	ILI9341_CSX_SET;
}

/***********************************************************************
Private function: Send parameter
***********************************************************************/
void ILI9341_send_parameter (uint8_t param)
{
	ILI9341_DCX_SET;
	ILI9341_CSX_CLEAR;
	SPI_send_8_bits(ILI9341_SPI,param);
//	ILI9341_Delay(10);
//	ILI9341_CSX_SET;
}

/***********************************************************************
Private function: Send parameter
***********************************************************************/
void ILI9341_send_parameter_16_bits (uint16_t param)
{
	ILI9341_DCX_SET;
	ILI9341_CSX_CLEAR;
	SPI_send_16_bits(ILI9341_SPI,param);
//	ILI9341_Delay(10);	
//	ILI9341_CSX_SET;
}

/***********************************************************************
Private function: Set active area
***********************************************************************/
void ILI9341_set_active_area (uint16_t startColumn, uint16_t endColumn, uint16_t startPage, uint16_t endPage)
{
	ILI9341_send_command(ILI9341_COLUMN_ADDR);
	ILI9341_send_parameter_16_bits(startColumn);
	ILI9341_send_parameter_16_bits(endColumn);
	ILI9341_send_command(ILI9341_PAGE_ADDR);
	ILI9341_send_parameter_16_bits(startPage);
	ILI9341_send_parameter_16_bits(endPage);
}

/***********************************************************************
Private function: Create delay
***********************************************************************/
void ILI9341_delay(volatile uint32_t delay) {
	for (; delay != 0; delay--); 
}

/***********************************************************************
Private function: Fill area
***********************************************************************/
void ILI9341_fill_area (uint16_t startColumn, uint16_t endColumn, uint16_t startPage, uint16_t endPage, uint16_t color)
{
	ILI9341_set_active_area(startColumn,endColumn,startPage,endPage);
	ILI9341_send_command(ILI9341_MEM_WRITE);
	uint16_t areaWidth = endColumn - startColumn +1 ;
	uint16_t areaHeight  = endPage - startPage + 1; 
	uint32_t areaPixelCount = areaHeight*areaWidth;
	uint32_t count = 0 ;
	while(count < areaPixelCount){
		ILI9341_send_parameter_16_bits(color);
		count++;
	}
}
