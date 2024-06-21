#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "drivers/pinout.h"
#include "drivers/buttons.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "inc/hw_memmap.h"
#include "inc/tm4c129encpdt.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

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

//
//*****************************************************************************
// Sets the clock frequency to System clock rate, 120 MHz.
void SetClockFrequency() {

    uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                                 SYSCTL_OSC_MAIN |
                                                 SYSCTL_USE_PLL |
                                                 SYSCTL_CFG_VCO_240), SYSTEM_CLOCK);
}
//
//*****************************************************************************

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
#define MAX_TEXT_SIZE 15
#define STATUS_DISPLAY_TIME 10000 // Measured in milliseconds.

// Text
char gTextBuffer[MAX_TEXT_SIZE] = "stems is so fun";
int gCharCount = 26;

// Task handlers
TaskHandle_t gUARTTaskHandler;
TaskHandle_t gStatusTaskHandler;

// Semaphore for printing.
SemaphoreHandle_t semPrint;

//*****************************************************************************
//
// Configures the UART.
//
//*****************************************************************************
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
// Task for send strings to UART.
//
//*****************************************************************************
void UARTTask(void* parameters)
{
    while(1)
    {
        if(xSemaphoreTake(semPrint, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            if(UARTCharsAvail(UART0_BASE))
            {
                char value = UARTCharGet(UART0_BASE);

                // Shift existing characters in the text buffer
                int i;
                for (i = 0; i < MAX_TEXT_SIZE - 1; i++)
                {
                    gTextBuffer[i] = gTextBuffer[i + 1];
                }

                // Append the new character
                gTextBuffer[MAX_TEXT_SIZE - 1] = value;

                gCharCount = gCharCount + 1;

                UARTprintf("\033[1;1H\r %s", gTextBuffer);
            }

            xSemaphoreGive(semPrint);
        }
    }
}

void StatusTask(void* parameters)
{
    // Variables for polling the button.
    unsigned char ucDelta, ucState;

    // Variable for displaying the char count.
    bool shouldDisplayCharCount = false;

    // Variable for caching time when button was pressed.
    TickType_t lastDisplayedTime = 0;

    while(1)
    {
        ucState = ButtonsPoll(&ucDelta, 0);
        if(BUTTON_PRESSED (RIGHT_BUTTON, ucState, ucDelta))
        {
            shouldDisplayCharCount = true;
            lastDisplayedTime = xTaskGetTickCount();
        }

        if(shouldDisplayCharCount)
        {
            if(xSemaphoreTake(semPrint, pdMS_TO_TICKS(10)) == pdTRUE)
            {
                UARTprintf("\033[2;1H\r %d", gCharCount);

                if(xTaskGetTickCount() - lastDisplayedTime > pdMS_TO_TICKS(STATUS_DISPLAY_TIME))
                {
                    shouldDisplayCharCount = false;
                    UARTprintf("\033[2;1H\r");
                    UARTprintf("           ");
                    UARTprintf("\033[2;1H\r");
                }

                xSemaphoreGive(semPrint);
            }
        }
    }
}

int main(void)
{
    //Configure the device pins.
    PinoutSet(false,false);

    //Initialize the button driver.
    ButtonsInit();

    //Enable the GPIO pin for the LED.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    // Configure the UART.
    ConfigureUART();

    // Set the clock frequency.
    SetClockFrequency();

    // print out the initial text.
    UARTprintf("\r %s", gTextBuffer);

    // Create semaphore.
    semPrint = xSemaphoreCreateBinary();
    xSemaphoreGive(semPrint);

    // Create the tasks.
    xTaskCreate(UARTTask, "UART", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gUARTTaskHandler);
    xTaskCreate(StatusTask, "UART", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gStatusTaskHandler);

    // Start the scheduler.
    vTaskStartScheduler();

    return 0;
}

