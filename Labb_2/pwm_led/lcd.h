/*
 * lcd.h
 *
 *  Created on: 19 sep. 2023
 *      Author: jjn20030
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"
#include "driverlib/debug.h"
#include "inc/hw_ints.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"
#include "driverlib/adc.h"
#include "CF128x128x16_ST7735S.h"

typedef struct _text_position
{
    uint32_t x;
    uint32_t y;
} text_position;

void draw_string(char* string, uint32_t length, text_position position);

#endif /* LCD_H_ */
