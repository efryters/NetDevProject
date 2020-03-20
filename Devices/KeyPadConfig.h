/**
 * https://github.com/nimaltd/KeyPad
 * Keypad library for matrix keypad
 * Credit goes to Nima Askari of Github
 */


#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H
#include "stm32l4xx_hal.h"
#include "main.h"

#define           _KEYPAD_DEBOUNCE_TIME_MS        20
#define           _KEYPAD_USE_FREERTOS            0

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] =
{
  ARD_D4_GPIO_Port,
  ARD_D6_GPIO_Port,
  ARD_D2_GPIO_Port
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
  ARD_D4_Pin,
  ARD_D6_Pin,
  ARD_D2_Pin
};

const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] =
{
  ARD_D5_GPIO_Port,
  ARD_D0_GPIO_Port,
  ARD_D1_GPIO_Port,
  ARD_D3_GPIO_Port
};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
  ARD_D5_Pin,
  ARD_D0_Pin,
  ARD_D1_Pin,
  ARD_D3_Pin
};

#endif
