/*
 * stopwatch.h
 *
 *  Created on: 12 sep. 2023
 *      Author: jjn20030
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"

#define STOPWATCH TIMER_A
#define STOPWATCH_CONFIGURATION TIMER_CFG_A_PERIODIC
#define STOPWATCH_INTERRUPT TIMER_TIMA_TIMEOUT
#define TIMER_BASE TIMER0_BASE

typedef struct stopwatch
{
    uint8_t initial_seconds1;
    uint8_t initial_seconds2;
    uint8_t initial_minutes1;
    uint8_t initial_minutes2;
    uint8_t initial_hours1;
    uint8_t initial_hours2;

    uint8_t seconds1;
    uint8_t seconds2;
    uint8_t minutes1;
    uint8_t minutes2;
    uint8_t hours1;
    uint8_t hours2;

    // System clock rate in Hz.
    uint32_t clock_rate;

} stopwatch;

void create_stopwatch();
void start_stopwatch();
void stop_stopwatch();
void reset_stopwatch(uint8_t hours1, uint8_t hours2, uint8_t minutes1, uint8_t minutes2, uint8_t seconds1, uint8_t seconds2);
void initialize_stopwatch(uint8_t hours1, uint8_t hours2, uint8_t minutes1, uint8_t minutes2, uint8_t seconds1, uint8_t seconds2);


#endif /* STOPWATCH_H_ */
