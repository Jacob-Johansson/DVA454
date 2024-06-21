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

#include "joystick.h"
#include "microphone.h"
#include "accelerometer.h"

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
// Typdef struct for analog data.
//
//*****************************************************************************
typedef struct
{
    uint32_t X;
    uint32_t Y;
} JoystickData;

typedef struct
{
    uint32_t X;
    uint32_t Y;
    uint32_t Z;
} AccelerometerData;

typedef struct
{
    uint32_t Value;
} MicrophoneData;

//*****************************************************************************
//
// Global Queue handles.
//
//*****************************************************************************
xQueueHandle gJoystickQueueHandle;
xQueueHandle gAccelerometerQueueHandle;
xQueueHandle gMicrophoneQueueHandle;
xQueueHandle gGatekeeperQueueHandle;

//*****************************************************************************
//
// Global Task handles.
//
//*****************************************************************************
TaskHandle_t gJoystickTaskHandle;
TaskHandle_t gAccelerometerTaskHandle;
TaskHandle_t gMicrophoneTaskHandle;
TaskHandle_t gGateKeeperTaskHandle;

#define MAXIMUM_QUEUE_SIZE 10
#define MAXIMUM_JOYSTICK_QUEUE_SIZE 4
#define MAXIMUM_ACCELEROMETER_QUEUE_SIZE 2
#define MAXIMUM_MICROPHONE_QUEUE_SIZE 8


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
// Joystick task.
//
//*****************************************************************************
void JoystickTask(void* parameters)
{
    int numCollectedSamples = 0;

    while(1)
    {
        if(uxQueueSpacesAvailable(gJoystickQueueHandle) == 0)
        {
            vTaskSuspend(gJoystickTaskHandle);
        }
        else
        {
            JoystickData data;
            data.X = read_joystick_x();
            data.Y = read_joystick_y();

            // Send to the message queue for the joystick, don't wait if full.
            xQueueSend(gJoystickQueueHandle, (void*) &data, ( TickType_t ) 0);
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

//*****************************************************************************
//
// Accelerometer task.
//
//*****************************************************************************
void AccelerometerTask(void* parameters)
{
    while(1)
    {
        if(uxQueueSpacesAvailable(gAccelerometerQueueHandle) == 0)
        {
            vTaskSuspend(gAccelerometerTaskHandle);
        }
        else
        {
            AccelerometerData data;
            data.X = read_accelerometer_x();
            data.Y = read_accelerometer_y();
            data.Z = read_accelerometer_z();

            // Send to the message queue for the accelerometer, don't wait if full.
            xQueueSend(gAccelerometerQueueHandle, (void*) &data, ( TickType_t ) 0);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//*****************************************************************************
//
// Microphone task.
//
//*****************************************************************************
void MicrophoneTask(void* parameters)
{
    while(1)
    {
        if(uxQueueSpacesAvailable(gMicrophoneQueueHandle) == 0)
        {
            vTaskSuspend(gMicrophoneTaskHandle);
        }
        else
        {
            MicrophoneData data;
            data.Value = read_microphone();

            // Send to the message queue for the microphone, don't wait if full.
            xQueueSend(gMicrophoneQueueHandle, (void*) &data, ( TickType_t ) 0);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

//*****************************************************************************
//
// Gate-keeper task.
//
//*****************************************************************************
void GateKeeperTask(void* parameters)
{
    int i;

    while(1)
    {

        uint32_t numAvailableJoystickData = uxQueueSpacesAvailable(gJoystickQueueHandle);
        uint32_t numAvailableAccelerometerData = uxQueueSpacesAvailable(gAccelerometerQueueHandle);
        uint32_t numAvailableMicrophoneData = uxQueueSpacesAvailable(gMicrophoneQueueHandle);

        if(numAvailableJoystickData == 0 && numAvailableAccelerometerData == 0 && numAvailableMicrophoneData == 0)
        {
            JoystickData joystickBuffer[MAXIMUM_JOYSTICK_QUEUE_SIZE];
            AccelerometerData accelerometerBuffer[MAXIMUM_ACCELEROMETER_QUEUE_SIZE];
            MicrophoneData microphoneBuffer[MAXIMUM_MICROPHONE_QUEUE_SIZE];

            for(i = 0; i < MAXIMUM_JOYSTICK_QUEUE_SIZE; i++)
            {
                xQueueReceive(gJoystickQueueHandle, &joystickBuffer[i], portMAX_DELAY);
            }

            for(i = 0; i < MAXIMUM_ACCELEROMETER_QUEUE_SIZE; i++)
            {
                xQueueReceive(gAccelerometerQueueHandle, &accelerometerBuffer[i], portMAX_DELAY);
            }

            for(i = 0; i < MAXIMUM_MICROPHONE_QUEUE_SIZE; i++)
            {
                xQueueReceive(gMicrophoneQueueHandle, &microphoneBuffer[i], portMAX_DELAY);
            }

            // Average values.
            uint32_t averageJoystickX = 0;
            uint32_t averageJoystickY = 0;
            uint32_t averageAccelerometerX = 0;
            uint32_t averageAccelerometerY = 0;
            uint32_t averageAccelerometerZ = 0;
            uint32_t averageMicrophone = 0;


            for(i = 0; i < MAXIMUM_JOYSTICK_QUEUE_SIZE; i++)
            {
                averageJoystickX = averageJoystickX + joystickBuffer[i].X;
                averageJoystickY = averageJoystickY + joystickBuffer[i].Y;
            }

            for(i = 0; i < MAXIMUM_ACCELEROMETER_QUEUE_SIZE; i++)
            {
                averageAccelerometerX = averageAccelerometerX + accelerometerBuffer[i].X;
                averageAccelerometerY = averageAccelerometerX + accelerometerBuffer[i].Y;
                averageAccelerometerZ = averageAccelerometerX + accelerometerBuffer[i].Z;
            }

            for(i = 0; i < MAXIMUM_MICROPHONE_QUEUE_SIZE; i++)
            {
                averageMicrophone = averageMicrophone + microphoneBuffer[i].Value;
            }

            averageJoystickX = averageJoystickX / MAXIMUM_JOYSTICK_QUEUE_SIZE;
            averageJoystickY = averageJoystickY / MAXIMUM_JOYSTICK_QUEUE_SIZE;
            averageAccelerometerX = averageAccelerometerX / MAXIMUM_ACCELEROMETER_QUEUE_SIZE;
            averageAccelerometerY = averageAccelerometerY / MAXIMUM_ACCELEROMETER_QUEUE_SIZE;
            averageAccelerometerZ = averageAccelerometerZ / MAXIMUM_ACCELEROMETER_QUEUE_SIZE;
            averageMicrophone = averageMicrophone / MAXIMUM_MICROPHONE_QUEUE_SIZE;

            UARTprintf("\033[1;1H\rJoystick: %d, %d", averageJoystickX, averageJoystickY);
            UARTprintf("\033[2;1H\rAccelerometer: %d, %d, %d", averageAccelerometerX, averageAccelerometerY, averageAccelerometerZ);
            UARTprintf("\033[3;1H\rMicrophone: %d", averageMicrophone);

            vTaskResume(gJoystickTaskHandle);
            vTaskResume(gAccelerometerTaskHandle);
            vTaskResume(gMicrophoneTaskHandle);
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

    // Create message queues for sensor data
    gJoystickQueueHandle = xQueueCreate(MAXIMUM_JOYSTICK_QUEUE_SIZE, sizeof(JoystickData));
    gMicrophoneQueueHandle = xQueueCreate(MAXIMUM_MICROPHONE_QUEUE_SIZE, sizeof(MicrophoneData));
    gAccelerometerQueueHandle = xQueueCreate(MAXIMUM_ACCELEROMETER_QUEUE_SIZE, sizeof(AccelerometerData));

    // Create tasks for reading joystick, accelerometer, and microphone data
    xTaskCreate(JoystickTask, "Joystick", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gJoystickTaskHandle);
    xTaskCreate(AccelerometerTask, "Accelerometer", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gAccelerometerTaskHandle);
    xTaskCreate(MicrophoneTask, "Microphone", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gMicrophoneTaskHandle);
    xTaskCreate(GateKeeperTask, "Gate-Keeper", configMINIMAL_STACK_SIZE, (void*) 0, tskIDLE_PRIORITY, &gGateKeeperTaskHandle);

    // Start the scheduler.
    vTaskStartScheduler();

    return 0;
}

