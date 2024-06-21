//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/tm4c129encpdt.h"


//***********************************************************************
//                       Configurations
//***********************************************************************
// Configure the UART.
void ConfigureUART(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//                      Main
//*****************************************************************************
int main(void)
{
    ConfigureUART();

    float pwm_word;
    uint32_t systemClock;
    systemClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 16000);



       pwm_word = systemClock / 200;
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
       SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
       SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
           SysCtlPeripheralReset(SYSCTL_PERIPH_PWM0);
           SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);


       GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
       GPIOPinConfigure(GPIO_PF2_M0PWM2);

       UARTprintf("%d\n", 1);

       PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
       UARTprintf("2\n");
       PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, pwm_word);
       UARTprintf("3\n");
       PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm_word/1000);
       UARTprintf("4\n");
       PWMGenEnable(PWM0_BASE, PWM_GEN_1);
       UARTprintf("5\n");
       PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);


    UARTprintf("%d\n",systemClock);

    while(1)
    {
    }
}
