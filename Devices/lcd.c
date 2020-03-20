/*
 * lcd.cpp
 *
 *  Created on: Mar 5, 2020
 *      Author: efryters
 */


#include "lcd.h"
#include "stm32l4xx_hal.h"
#include "string.h"

/* Private function prototypes */
void _send(uint8_t data, I2C_HandleTypeDef* i2c_dev, uint8_t addr, uint8_t mem_addr);


static lcd_device* device;
static bool interface_4bits = false;

/**
 * Initialize the LCD display
 * @param dev lcd_device struct containing where to find lcd
 */
void lcd_init(lcd_device* dev)
{
	device = dev;

	// Minimum required delay to complete POR on LCD
	HAL_Delay(55);

	/* Setup port expander to output mode */
	_send(0x00, device->handle, device->addr, MCP_IODIR_ADDR);

	/* Setup LCD for 4-bit mode. */
	// Send Function set command 4-bits mode
	lcd_send_command((LCD_FUNCTIONSET | LCD_8BITMODE), false);
	// Wait minimum 4.1ms
	HAL_Delay(6);
	// Send same command
	lcd_send_command((LCD_FUNCTIONSET | LCD_8BITMODE), false);
	// Wait minimum 4.1ms
	HAL_Delay(6);
	// Send same command one last time
	lcd_send_command((LCD_FUNCTIONSET | LCD_8BITMODE), false);
	HAL_Delay(1);

	// Set 4 bit mode now.
	lcd_send_command((LCD_FUNCTIONSET | LCD_4BITMODE), false);

	// Activate 4-bit sending
	interface_4bits = true;

	/* Setup the display */
	// Function set
	lcd_send_command((LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS), false);
	HAL_Delay(2);

	// Clear display
	lcd_send_command(LCD_CLEARDISPLAY, false);
	HAL_Delay(2);

	// Turn on display, and cursor on.
	lcd_send_command((LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON), false);
	HAL_Delay(1);

	// Entry mode
	lcd_send_command((LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT), false);
	HAL_Delay(1);

	// Return home
	lcd_send_command(LCD_RETURNHOME, false);
	HAL_Delay(1);

	// Success
	device->initialized = true;
}

/**
 * Send a command to the given LCD device. Uses 4 bit mode
 * @param dev lcd_device struct containing where to find lcd
 * @param cmd Command to send to the LCD
 */
void lcd_send_command(uint8_t cmd, bool print)
{
	/* DATA IS CLOCKED IN ON En FALLING EDGE */

	uint8_t send = 0x00;
	// Break upper and lower
	uint8_t upper = (cmd & 0xf0) >> 4;
	uint8_t lower = cmd & 0x0f;


	/* Send Upper Data */
	// move the 4 bits up by 1
	send = 	upper << 3;
	send |= 0x80;			// Add backlight

	if (print)
		send |= 0x02;
	// Send upper data
	_send(send, device->handle, device->addr, MCP_GPIO_ADDR);
	HAL_Delay(1);
	// Toggle EN pin
	send |= 0x04;	// Add EN
	_send(send, device->handle, device->addr, MCP_GPIO_ADDR);
	HAL_Delay(1);
	send &= 0xfb;	// Deattach EN pin
	_send(send, device->handle, device->addr, MCP_GPIO_ADDR);
	HAL_Delay(1);

	if (interface_4bits)
	{
		/* Send Lower Data */
		// move the 4 bits up by 1
		send = 	lower << 3;
		send |= 0x80;			// Add backlight

		if (print)
			send |= 0x02;
		// Send upper data
		_send(send, device->handle, device->addr, MCP_GPIO_ADDR);
		HAL_Delay(1);
		// Toggle EN pin
		send |= 0x04;	// Add EN
		_send(send, device->handle, device->addr, MCP_GPIO_ADDR);
		HAL_Delay(1);
		send &= 0xfb;	// Deattach EN pin
		_send(send, device->handle, device->addr, MCP_GPIO_ADDR);
		HAL_Delay(1);
	}
	HAL_Delay(1);
}

void lcd_write_string(const char* str)
{
	uint8_t len = strlen(str);

	for(int i = 0; i < len; i++)
	{
		lcd_send_command((uint8_t)*(str+i), true);
	}

}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 0)
        address = 0;
    else if (row==1)
        address = 0x40;
    else if (row==2)
        address = 0x14;
    else if (row==3)
        address = 0x54;
    else
        address = 0;

    address += col;

    lcd_send_command(LCD_SETDDRAMADDR | address, false);
    HAL_Delay(1);
}

/**
 * LL send 4-bit mode to lcd
 * @param value
 */
void _send(uint8_t data, I2C_HandleTypeDef* i2c_dev, uint8_t addr, uint8_t mem_addr)
{
	HAL_I2C_Mem_Write(i2c_dev, addr, mem_addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}
