/*
 * stopwatch.c
 *
 *  Created on: 13 sep. 2023
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
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "inc/hw_ints.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"
#include "stopwatch.h"
#include "driverlib/timer.h"

stopwatch global_stopwatch; // Define a global stopwatch variable

///--------------------------------------------------
/// Interrupt handler for the stop watch
///--------------------------------------------------
static void stopwatch_interrupt_handler(void)
{
    // Increment seconds (assuming a one-second timer)
    global_stopwatch.seconds2++;

    // Check for carry-over to minutes
    if(global_stopwatch.seconds2 >= 10)
    {
        global_stopwatch.seconds2 = 0;
        global_stopwatch.seconds1++;

        if(global_stopwatch.seconds1 >= 6)
        {
            global_stopwatch.seconds1 = 0;
            global_stopwatch.minutes2++;

            if(global_stopwatch.minutes2 >= 10)
            {
                global_stopwatch.minutes2 = 0;
                global_stopwatch.minutes1++;

                if(global_stopwatch.minutes1 >= 6)
                {
                    global_stopwatch.minutes1 = 0;
                    global_stopwatch.hours2++;

                    if(global_stopwatch.hours2 >= 10)
                    {
                        global_stopwatch.hours2 = 0;
                        global_stopwatch.hours1++;
                    }
                }
            }
        }
    }

    if(global_stopwatch.hours1 == 2 && global_stopwatch.hours2 == 4)
    {
        reset_stopwatch(0, 0, 0, 0, 0, 0);
    }

        // Format the updated time manually
        char timeString[9]; // "hh:mm:ss" + '\0'
        timeString[0] = '0' + global_stopwatch.hours1;
        timeString[1] = '0' + global_stopwatch.hours2;
        timeString[2] = ':';
        timeString[3] = '0' + global_stopwatch.minutes1;
        timeString[4] = '0' + global_stopwatch.minutes2;
        timeString[5] = ':';
        timeString[6] = '0' + global_stopwatch.seconds1;
        timeString[7] = '0' + global_stopwatch.seconds2;
        timeString[8] = '\0';


        //Move the cursor to the beginning of the line
        UARTSendString("\r");

        // Send the formatted time over UART
        UARTSendString(timeString);

        // Clear the timer interrupt flag
        TimerIntClear(TIMER_BASE, STOPWATCH_INTERRUPT);
}

///--------------------------------------------------
/// Configure the timer the stop watch is using
///--------------------------------------------------
static void configure_stopwatch()
{
    //
    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    //
    global_stopwatch.clock_rate = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                         SYSCTL_OSC_MAIN |
                                         SYSCTL_USE_PLL |
                                         SYSCTL_CFG_VCO_240), 120000000);

    // Enable the Timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Wait for the Timer0 module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}

    // Configure stop watch as a half-width periodic timer
    TimerConfigure(TIMER_BASE, STOPWATCH_CONFIGURATION);

    // Set the count time for the stop watch.
    TimerLoadSet(TIMER_BASE, STOPWATCH, global_stopwatch.clock_rate);

    // Register the timer interrupt handler
    TimerIntRegister(TIMER_BASE, STOPWATCH, stopwatch_interrupt_handler);

    // Enable timer interrupt
    TimerIntEnable(TIMER_BASE, STOPWATCH_INTERRUPT);
}

///--------------------------------------------------
/// Creates a new stop watch
///--------------------------------------------------
void create_stopwatch()
{
    configure_stopwatch();
    initialize_stopwatch(0, 0, 0, 0, 0, 0);
}

///--------------------------------------------------
/// Starts the stop watch
///--------------------------------------------------
void start_stopwatch()
{
    // Enable the timers.
    TimerEnable(TIMER_BASE, STOPWATCH);
}

///--------------------------------------------------
/// Resets the stop watch
///--------------------------------------------------
void reset_stopwatch(const uint8_t hours1, const uint8_t hours2, const uint8_t minutes1, const uint8_t minutes2, const uint8_t seconds1, const uint8_t seconds2)
{
    global_stopwatch.seconds1 = seconds1;
    global_stopwatch.minutes1 = minutes1;
    global_stopwatch.hours1 = hours1;
    global_stopwatch.seconds2 = seconds2;
    global_stopwatch.minutes2 = minutes2;
    global_stopwatch.hours2 = hours2;
}

///--------------------------------------------------
/// Initializes the stop watch
///--------------------------------------------------
void initialize_stopwatch(const uint8_t hours1, const uint8_t hours2, const uint8_t minutes1, const uint8_t minutes2, const uint8_t seconds1, const uint8_t seconds2)
{
    global_stopwatch.initial_seconds1 = seconds1;
    global_stopwatch.initial_minutes1 = minutes1;
    global_stopwatch.initial_hours1 = hours1;
    global_stopwatch.initial_seconds2 = seconds2;
    global_stopwatch.initial_minutes2 = minutes2;
    global_stopwatch.initial_hours2 = hours2;

    global_stopwatch.seconds1 = seconds1;
    global_stopwatch.minutes1 = minutes1;
    global_stopwatch.hours1 = hours1;
    global_stopwatch.seconds2 = seconds2;
    global_stopwatch.minutes2 = minutes2;
    global_stopwatch.hours2 = hours2;
}

///--------------------------------------------------
/// Stops the stop watch
///--------------------------------------------------
void stop_stopwatch()
{
    TimerDisable(TIMER_BASE, STOPWATCH);
}
