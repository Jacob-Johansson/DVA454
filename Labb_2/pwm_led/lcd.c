/*
 * lcd.c
 *
 *  Created on: 19 sep. 2023
 *      Author: jjn20030
 */

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
#include "grlib/grlib.h"
#include "CF128x128x16_ST7735S.h"
#include "lcd.h"

void draw_string(char* string, uint32_t length, const text_position position)
{
    // Initialize the grlib library.
    tContext sContext;
    GrContextInit(&sContext, &g_sCF128x128x16_ST7735S);

    // Set the font, size, and color of the text.
    GrContextFontSet(&sContext, &g_sFontCmtt16);

    // Set foreground color
    GrContextForegroundSet(&sContext, ClrWhite);

    // Draw string centered on the LCD display.
    GrStringDraw(&sContext, string, length, position.x, position.y, 255);

    // Update the LCD display.
    GrFlush(&sContext);
}
