/*
 * CF128x128x16_ST7735S.h
 *
 *  Created on: 6 Sep 2021
 *      Author: User
 */

#ifndef DRIVERS_CF128X128X16_ST7735S_H_
#define DRIVERS_CF128X128X16_ST7735S_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "grlib/grlib.h"

typedef enum _LCD_Orientation
{
    LCD_ORIENTATION_UP      = 0,
    LCD_ORIENTATION_LEFT    = 1,
    LCD_ORIENTATION_DOWN    = 2,
    LCD_ORIENTATION_RIGHT   = 3
}LCD_Orientation;

extern const tDisplay g_sCF128x128x16_ST7735S;

void SysTickDelayMs(uint32_t ui32Ms);

void CF128x128x16_ST7735SInit(uint32_t sysClock);
void CF128x128x16_ST7735SSetOrientation(LCD_Orientation orientation);
void CF128x128x16_ST7735SClear(uint32_t colour);


#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_CF128X128X16_ST7735S_H_ */
