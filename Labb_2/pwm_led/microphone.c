/*
 * microphone.c
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

uint32_t read_microphone(void)
{
    // Enable the GPIO port that the ADC pin is connected to (e.g., GPIO_PORTD_BASE)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) {}

    // Configure the GPIO pin as an analog input
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH8);

    ADCSequenceEnable(ADC0_BASE, 3);
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Wait for the conversion to complete
    while(!ADCIntStatus(ADC0_BASE, 3, false)){}

    // Read the ADC value
    uint32_t value;
    ADCSequenceDataGet(ADC0_BASE, 3, &value);

    // Clear the ADC interrupt
    ADCIntClear(ADC0_BASE, 3);

    return value;
}
