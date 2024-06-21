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
// Global variables for all tasks.
//
//*****************************************************************************
TaskHandle_t LED1_task;
TaskHandle_t LED2_task;
TaskHandle_t LED3_task = NULL;
TaskHandle_t LED4_task = NULL;
TaskHandle_t HoldLED1On_task = NULL;
TaskHandle_t HoldLED2On_task = NULL;

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
// Task 1, blink every 1 second.
//
//*****************************************************************************
 void LED1 (void *x){
     for (;;){
         UARTprintf("\n Task 1: Turn on LED 1.");
         LEDWrite(CLP_D1, 1);
         vTaskDelay(1000);
         LEDWrite(CLP_D1, 0);
         UARTprintf("\n Task 1: Turn off LED 1.");
         vTaskDelay(1000);
     }
 }

 //*****************************************************************************
 //
 // Task 2, blink every 2 seconds.
 //
 //*****************************************************************************
 void LED2 (void *x){
     for (;;){
         UARTprintf("\n Task 2: Turn on LED 2.");
         LEDWrite(CLP_D2, 2);
         vTaskDelay(2000);
         LEDWrite(CLP_D2, 0);
         UARTprintf("\n Task 2: Turn off LED 2.");
         vTaskDelay(2000);
     }
 }

 //*****************************************************************************
 //
 // Task 3, blink every 3 seconds.
 //
 //*****************************************************************************
 void LED3 (void *x){
     for (;;){
         UARTprintf("\n Task 3: Turn on LED 3.");
         LEDWrite(CLP_D3, 4);
         vTaskDelay(3000);
         LEDWrite(CLP_D3, 0);
         UARTprintf("\n Task 3: Turn off LED 3.");
         vTaskDelay(3000);

     }
 }

 //*****************************************************************************
 //
 // Task 4, blink every 4 seconds.
 //
 //*****************************************************************************
 void LED4 (void *x){
     for (;;){
         UARTprintf("\n Task 4: Turn on LED 4.");
         LEDWrite(CLP_D4, 8);
         vTaskDelay(4000);
         LEDWrite(CLP_D4, 0);
         UARTprintf("\n Task 4: Turn off LED 4.");
         vTaskDelay(4000);
     }
 }

//*****************************************************************************
//
// Task 5, handle keeping LED 1 turned on for 10 seconds.
//
//*****************************************************************************
void HoldLED1On (void *x)
{
     unsigned char ucDelta, ucState;
     ButtonsInit();

     for (;;)
     {
         ucState = ButtonsPoll(&ucDelta, 0);
         if(BUTTON_PRESSED (RIGHT_BUTTON, ucState, ucDelta))
         {
              UARTprintf("\n Task 5: Stop Task 1 and turn on LED 1 for 10 seconds.");
              vTaskSuspend(LED1_task);

              LEDWrite(CLP_D1, 1);

              vTaskDelay(10000);

              vTaskResume(LED1_task);
              UARTprintf("\n Task 5: Restart Task 1 and set LED 1 to its original state");
         }

  }
}

//*****************************************************************************
//
// Task 6, handle keeping LED 2 turned on for 10 seconds.
//
//*****************************************************************************
void HoldLED2On (void *x)
{
     unsigned char ucDelta, ucState;
     ButtonsInit();
     for (;;)
     {
         ucState = ButtonsPoll(&ucDelta, 0);
         if (BUTTON_PRESSED (LEFT_BUTTON, ucState, ucDelta))
         {
           UARTprintf("\n Task 6: Stop Task 2 and turn on LED 2 for 10 seconds.");
           vTaskSuspend(LED2_task);

           LEDWrite(CLP_D2, 2);
           vTaskDelay(10000);

           vTaskResume(LED2_task);
           UARTprintf("\n Task 6: Restart Task 2 and set LED 2 to its original state.");
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

    // Create the tasks.
    xTaskCreate(LED1, "task1", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &LED1_task);
    xTaskCreate(LED2, "task2", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &LED2_task);
    xTaskCreate(LED3, "task3", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &LED3_task);
    xTaskCreate(LED4, "task4", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &LED4_task);
    xTaskCreate(HoldLED1On, "task5", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &HoldLED1On_task);
    xTaskCreate(HoldLED2On, "task6", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &HoldLED2On_task);

    // Start the scheduler.
    vTaskStartScheduler();

    return 0;
}

