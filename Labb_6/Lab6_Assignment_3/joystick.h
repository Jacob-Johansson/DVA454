/*
 * joystick.h
 *
 *  Created on: 19 sep. 2023
 *      Author: jjn20030
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

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


uint32_t read_joystick_x(void);

uint32_t read_joystick_y(void);


#endif /* JOYSTICK_H_ */
