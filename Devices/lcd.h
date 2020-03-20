/*
 * lcd.h
 *
 *  Created on: Mar 4, 2020
 *      Author: efryters
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l4xx_hal.h"
#include "stdbool.h"

// MCP23008 port expander registers
#define MCP_IODIR_ADDR 0x00
#define MCP_GPIO_ADDR 0x09

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

/* Exported structs */
typedef struct {
	I2C_HandleTypeDef* handle;	// HAL handle for i2c device
	uint8_t addr;				// Address of LCD on i2c bus
	bool initialized;			// true if initialized
} lcd_device;


/* Exported Functions */
void lcd_init(lcd_device* dev);
void lcd_send_command(uint8_t cmd, bool print);
void lcd_write_string(const char* str);
void lcd_set_cursor(uint8_t row, uint8_t col);

#endif /* INC_LCD_H_ */
