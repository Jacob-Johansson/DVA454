/*
 * accelerometer.h
 *
 *  Created on: 19 sep. 2023
 *      Author: jjn20030
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

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

uint32_t read_accelerometer_x(void);
uint32_t read_accelerometer_y(void);
uint32_t read_accelerometer_z(void);

#endif /* ACCELEROMETER_H_ */
