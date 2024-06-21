//*****************************************************************************
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
#include "stdio.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif




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
//
// The UART interrupt handler.
//
//*****************************************************************************
void IntHandler(void){

}
// Implementation of UARTSendString
void UARTSendString(const char *string) {
    while (*string) {
        UARTCharPut(UART0_BASE, *string);
        string++;
    }
}

uint8_t CharChange(char myChar)
{
    uint8_t myUint8 = (uint8_t)(myChar - '0'); // Convert character to uint8_t
    return myUint8;
}

// A simple atoi() function
int myAtoi(volatile char* str)
{
    // Initialize result
    int res = 0;

    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corresponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    int i;
    for (i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}

 void InitialTime()
 {
     UARTSendString("Choose the time in the format hh:mm:ss.\n");

     // Read hours (tens digit)
     uint8_t buffer[6];
     buffer[0] = 0;
     buffer[1] = 0;
     buffer[2] = 0;
     buffer[3] = 0;
     buffer[4] = 0;
     buffer[5] = 0;

     int32_t value = 0;
     int index = 0;
     do{
         value = UARTCharGet(UART0_BASE);
         buffer[index++] = value;
     }while(value != '\r');

     value = 0;
     index = 2;
     do{
         value =  UARTCharGet(UART0_BASE);
         buffer[index++] = value;
     }while(value != '\r');

     value = 0;
     index = 4;
     do{
          value =  UARTCharGet(UART0_BASE);
          buffer[index++] = value;
      }while(value != '\r');

     // Send the chosen time back to the user
     UARTSendString("Initial time set to: ");
     UARTCharPut(UART0_BASE, '0' + CharChange(buffer[0]));
     UARTCharPut(UART0_BASE, '0' + CharChange(buffer[1]));
     UARTCharPut(UART0_BASE, ':');
     UARTCharPut(UART0_BASE, '0' + CharChange(buffer[2]));
     UARTCharPut(UART0_BASE, '0' + CharChange(buffer[3]));
     UARTCharPut(UART0_BASE, ':');
     UARTCharPut(UART0_BASE, '0' + CharChange(buffer[4]));
     UARTCharPut(UART0_BASE, '0' + CharChange(buffer[5]));
     UARTSendString("\n");

     // Initialize the stopwatch with the chosen time
     initialize_stopwatch(CharChange(buffer[0]), CharChange(buffer[1]),CharChange(buffer[2]), CharChange(buffer[3]), CharChange(buffer[4]), CharChange(buffer[5]));

 }

int main(void)
{
    ConfigureUART();

    //
    // Register the interrupt handler function for UART 0.
    //
    IntRegister(INT_UART0, IntHandler);
    //
    // Enable the interrupt for UART 0.
    //
    IntEnable(INT_UART0);
    //
    // Enable UART 0.
    //
    IntMasterEnable();

    create_stopwatch();
    UARTSendString("\r\nMain:\r\n 1) Set Initial Time \r\n 2) Start \r\n 3) Stop \r\n 4) Reset \r\n");

    while(1)
    {
        switch(UARTCharGet(UART0_BASE))
        {
        case '1':
            InitialTime();
            break;

        case '2':
            start_stopwatch();
            break;
        case '3':
            stop_stopwatch();
           break;
        case '4':
            // Implement reset functionality if needed.
            UARTSendString("\r\nReset command selected.\r\n 1) Set Initial Time \r\n 2) Default ");
            switch(UARTCharGet(UART0_BASE))
            {
                case '1':
                    InitialTime();
                break;

                case'2':
                    UARTSendString("\r\nYou have choosen time 00:00:00.\r\n Main:\r\n 1) Set Initial Time \r\n 2) Start \r\n 3) Stop \r\n 4) Reset \r\n");
                    reset_stopwatch(0, 0, 0, 0, 0, 0);
                break;
            }

           break;
        case '\r':
            break;
        case '\n':
            break;
        default:
            //UARTSendString("Invalid input. Please select a valid option.\n");
           break;
        }
    }

}
