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

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
TaskHandle_t Task_High_Priority_Handle = NULL;
TaskHandle_t Task_Medium_Priority_Handle = NULL;
TaskHandle_t Task_Low_Priority_Handle = NULL;

SemaphoreHandle_t semaphore = NULL;

#define HIGH_WORKLOAD 10000000 * 8
#define MEDIUM_WORKLOAD 10000000 * 6
#define LOW_WORKLOAD 10000000 * 10

//
//*****************************************************************************

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
// Configures the UART.
//
//*****************************************************************************
void ConfigureUART(void)
{
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
// Prints the name of the task that misses the deadline, and by how much time.
//
//*****************************************************************************
void PrintMissedDeadline(char* name, TickType_t time)
{
    UARTprintf("\n Task %s missed the deadline by %d milliseconds.", name, time);
}

//*****************************************************************************
//
// Task with high priority.
//
//*****************************************************************************
void Task_HighPriority(void* parameters)
{
    // Deadline of the task.
    TickType_t deadline = 3000;

    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(100));

        // Task is released for execution.
        UARTprintf("\n Task High started");

        // Time when the task begins.
        TickType_t timeBegan = xTaskGetTickCount();

        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 6000 ticks to see if it becomes free.
        while(xSemaphoreTake(semaphore, (TickType_t)pdMS_TO_TICKS(10000)) != pdTRUE)
        {
            UARTprintf("\n Task high take nothing");
        }

        UARTprintf("\n Task High takes semaphore");
        UARTprintf("\n Task High started its workload");

        SysCtlDelay(HIGH_WORKLOAD);

        // Time when the task finished.
        TickType_t timeFinished = xTaskGetTickCount();

        UARTprintf("\n Task High gives semaphore");
        xSemaphoreGive(semaphore);

        UARTprintf("\n Task High finished");

        // Check whether the task missed its deadline.
        TickType_t deltaTime = timeFinished - timeBegan;
        if(deltaTime > deadline)
        {
            PrintMissedDeadline("High", deltaTime);
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

//*****************************************************************************
//
// Task with medium priority.
//
//*****************************************************************************
void Task_MediumPriority(void* parameters)
{
    // Deadline of the task.
    TickType_t deadline = 1000;

    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(300));

        UARTprintf("\n Task Medium started");
        UARTprintf("\n Task Medium started its workload");

        // Time when the task begins.
        TickType_t timeBegan = xTaskGetTickCount();

        SysCtlDelay(MEDIUM_WORKLOAD);

        // Time when the task finished.
        TickType_t timeFinished = xTaskGetTickCount();

        UARTprintf("\n Task Medium finished");

        vTaskDelay(pdMS_TO_TICKS(8000));
    }
}

//*****************************************************************************
//
// Task with low priority.
//
//*****************************************************************************
void Task_LowPriority(void* parameters)
{
    // Deadline of the task.
    TickType_t deadline = 1500;

    for(;;)
    {
        UARTprintf("\n Task Low started");

        // Time when the task begins.
        TickType_t timeBegan = xTaskGetTickCount();

        while(xSemaphoreTake(semaphore, (TickType_t)pdMS_TO_TICKS(100)) != pdTRUE)
        {
            UARTprintf("\n Task Low has Nothing");
        }

        UARTprintf("\n Task Low takes semaphore");
        UARTprintf("\n Task Low started its workload");

        SysCtlDelay(LOW_WORKLOAD);

        // Time when the task finished.
        TickType_t timeFinished = xTaskGetTickCount();


        UARTprintf("\n Task Low gives semaphore");
        xSemaphoreGive(semaphore);

        UARTprintf("\n Task Low finished");

        // Check whether the task missed its deadline.
        TickType_t deltaTime = timeFinished - timeBegan;
        if(deltaTime > deadline)
        {
            PrintMissedDeadline("Low", deltaTime);
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
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

    // Create the semaphore.
    semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore);

    // Create the tasks.
    xTaskCreate(Task_HighPriority,   "task1", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY + 3, &Task_High_Priority_Handle);
    xTaskCreate(Task_MediumPriority, "task2", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY + 2, &Task_Medium_Priority_Handle);
    xTaskCreate(Task_LowPriority,    "task3", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY + 1, &Task_Low_Priority_Handle);
    vTaskStartScheduler();

    return 0;
}

