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

#define BUFFER_SIZE 10

typedef struct
{
    // Buffer.
    uint32_t Buffer[BUFFER_SIZE];

    // Index of the next place in the buffer.
    int NextIndex;
} Data;

//*****************************************************************************
//
// Global shared data.
//
//*****************************************************************************
Data gData;

//*****************************************************************************
//
// Global task handlers.
//
//*****************************************************************************
TaskHandle_t gProducerTask;
TaskHandle_t gConsumerTask;



//*****************************************************************************
//
// Producer task.
//
//*****************************************************************************
void Producer(void* parameters)
{
    while(1)
    {
        if(gData.NextIndex < BUFFER_SIZE)
        {
            uint32_t value = rand() % 100;

            UARTprintf("\n Producer produced: %d", value);

            vTaskDelay(1000);

            gData.Buffer[gData.NextIndex] = value;
            gData.NextIndex = gData.NextIndex + 1;
        }
        else
        {
            // Random delay between 1-2 seconds.
            vTaskDelay(pdMS_TO_TICKS((rand() % 1000) + 1000));
        }
    }
}

//*****************************************************************************
//
// Consumer task.
//
//*****************************************************************************
void Consumer(void* parameters)
{
    while(1)
    {
        if(gData.NextIndex > 0)
        {
            uint32_t value = gData.Buffer[gData.NextIndex - 1];
            UARTprintf("\n Consumer consumed: %d", value);

            vTaskDelay(1000);
            gData.NextIndex = gData.NextIndex - 1;
        }
        else
        {
            // Random delay between 1-2 seconds.
            vTaskDelay(pdMS_TO_TICKS((rand() % 1000) + 1000));
        }
    }
}

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

    // Set the seed.
    srand(100);

    gData.NextIndex = 0;

    // Create the tasks.
    xTaskCreate(Producer, "Producer", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gProducerTask);
    xTaskCreate(Consumer, "Consumer", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gConsumerTask);

    // Start the scheduler.
    vTaskStartScheduler();

    return 0;
}

