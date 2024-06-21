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
#include "driverlib/debug.h"
#include "inc/hw_ints.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"
#include "driverlib/adc.h"
#include "joystick.h"
#include "microphone.h"
#include "accelerometer.h"
#include "grlib/grlib.h"
#include "lcd.h"
#include "stdio.h"


//****************************************************************************
//
// System clock rate in Hz.
//
//****************************************************************************
uint32_t g_ui32SysClock;

//****************************************************************************
//
// Pulse width.
//
//****************************************************************************
int pulse_width = 1000;
int min_pulse_width = 1;
int max_pulse_width = 1000;

//****************************************************************************
//
// Which part of the lab to use to change the LED.
// TRUE: Uses input from the user through the terminal to update the pulse_width
// FALSE: Uses input from the user through the
//
//****************************************************************************
#define USE_UART false

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

int convert_percentage_to_pulse_width(int percentage, int min_pule_width, int max_pulse_width)
{
    float percentage_decimal = ((float)percentage/(float)100);
    return max_pulse_width - percentage_decimal * max_pulse_width;
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

void clear_string(char* string, uint32_t length)
{
    int i;
    for(i = 0; i < length; i++)
        string[i] = '0';
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
bool
UARTIntHandler(volatile char* buffer, volatile int buffer_length, volatile int* bufferIndex)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = MAP_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    MAP_UARTIntClear(UART0_BASE, ui32Status);

    bool isDone = false;

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(MAP_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        unsigned char data = MAP_UARTCharGetNonBlocking(UART0_BASE);
        MAP_UARTCharPutNonBlocking(UART0_BASE, data);

        switch(data)
        {
        case '\r':
            isDone = true;
            if(*bufferIndex < buffer_length)
            {
                buffer[(*bufferIndex)++] = '\0';
            }
            break;
        default:
            if(*bufferIndex < buffer_length)
            {
                buffer[(*bufferIndex)++] = data;
            }
            isDone = false;
            break;
        }
    }

    return isDone;
}

void convert_to_string(uint32_t value, unsigned char* string, uint32_t length)
{
    // Initialize the index of the string buffer.
    int index = 0;
    uint32_t local_value = value;

    // If the joystick value is zero, add a leading zero to the string buffer.
    if (local_value == 0) {
        string[index++] = '0';
    }

    if(value >= 1000)
    {
        // Divide the joystick value by 10 repeatedly until the quotient is zero.
        while (local_value > 0) {
            string[index++] = (local_value % 10) + '0';
            local_value /= 10;
        }
    }
    else if(value >= 100 && value < 1000)
    {
        string[index++] = '0';

        // Divide the joystick value by 10 repeatedly until the quotient is zero.
        while (local_value > 0) {
            string[index++] = (local_value % 10) + '0';
            local_value /= 10;
        }
    }
    else if(value >= 10 && value < 100)
    {
        string[index++] = '0';
        string[index++] = '0';

        // Divide the joystick value by 10 repeatedly until the quotient is zero.
        while (local_value > 0) {
            string[index++] = (local_value % 10) + '0';
            local_value /= 10;
        }
    }
    else
    {
        string[index++] = '0';
        string[index++] = '0';
        string[index++] = '0';

        // Divide the joystick value by 10 repeatedly until the quotient is zero.
        while (local_value > 0) {
            string[index++] = (local_value % 10) + '0';
            local_value /= 10;
        }
    }

    // Terminate the string buffer with a null terminator.
    string[index] = '\0';

    // Reverse the string buffer so that the most significant digit is first.
    int i;
    for (i = 0; i < index / 2; i++) {
      char temp = string[i];
      string[i] = string[index - i - 1];
      string[index - i - 1] = temp;
    }
}

//**********************************************************************
//
// Graphics context used to show text on the QVGA display.
//
//*****************************************************************************
tContext g_sContext;

tDisplay g_sPinoutConfig;

int main(void)
{
    ConfigureUART();

    //
    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240), 120000000);

    float pwm_word;
    uint32_t systemClock;
    systemClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 16000);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Enable the GPIO pins for the LED (PN0).
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    //
    // Enable the peripherals used by this example.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
   // Enable processor interrupts.
   //
   MAP_IntMasterEnable();

   pwm_word = systemClock / 200;
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
   SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
   SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
   SysCtlPeripheralReset(SYSCTL_PERIPH_PWM0);
   SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);


   GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
   GPIOPinConfigure(GPIO_PF2_M0PWM2);

   // Configure the device pins
   PinoutSet(false, false);

   // Enable the GPIO pin for the LED (PN0).
   // Set the direction as output, and
   // enable the GPIO pin for digital function.
   GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

   UARTprintf("%d\n", 1);

   PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
   UARTprintf("2\n");
   PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, pwm_word);
   UARTprintf("3\n");
   PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm_word/pulse_width);
   UARTprintf("4\n");
   PWMGenEnable(PWM0_BASE, PWM_GEN_1);
   UARTprintf("5\n");
   PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);

   UARTprintf("Ready\n");

    //
    // Enable the UART interrupt.
    //
    MAP_IntEnable(INT_UART0);
    MAP_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    volatile int bufferLength = 10;
    volatile int bufferIndex = 0;
    volatile char buffer[10];

    bool hasCleared = false;
    while(1)
    {
        if(USE_UART == true)
        {
            if(UARTIntHandler(buffer, bufferLength, &bufferIndex))
            {
                int percentage = myAtoi(buffer);

                switch(percentage)
                {
                case 0:
                    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 1);
                    LEDWrite(CLP_D1, 0);
                    break;
                case 100:
                    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, max_pulse_width * 100);
                    LEDWrite(CLP_D1, 1);
                    break;
                default:
                    pulse_width = convert_percentage_to_pulse_width(percentage, min_pulse_width, max_pulse_width);
                    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm_word/pulse_width);
                    break;
                }

                // Reset
                bufferIndex = 0;
                uint32_t i;
                for(i = 0; i < bufferLength; i++)
                    buffer[i] = '\0';

                UARTprintf("\nChoose next percentage..\n");
            }
        }
        else
        {
            if(!hasCleared)
            {
                // Initialize the LCD display controller.
                CF128x128x16_ST7735SInit(300000000);

                hasCleared = true;
            }

            uint32_t joystick_x = read_joystick_x();
            volatile uint32_t joystick_y = read_joystick_y();
            volatile uint32_t microphone = read_microphone();
            volatile uint32_t accelerometer_x = read_accelerometer_x();
            volatile uint32_t accelerometer_y = read_accelerometer_y();
            volatile uint32_t accelerometer_z = read_accelerometer_z();


            {
                const uint32_t buffer_length = 5;
                const uint32_t value_x_start = 90;
                unsigned char joystick_x_string[buffer_length];
                unsigned char joystick_y_string[buffer_length];
                unsigned char microphone_string[buffer_length];
                unsigned char accelerometer_x_string[buffer_length];
                unsigned char accelerometer_y_string[buffer_length];
                unsigned char accelerometer_z_string[buffer_length];

                // Clear strings
                clear_string(joystick_x_string, buffer_length);
                clear_string(joystick_y_string, buffer_length);
                clear_string(microphone_string, buffer_length);
                clear_string(accelerometer_x_string, buffer_length);
                clear_string(accelerometer_y_string, buffer_length);
                clear_string(accelerometer_z_string, buffer_length);

                sprintf(joystick_x_string, "%d   ", joystick_x);
                sprintf(joystick_y_string, "%d   ", joystick_y);
                sprintf(microphone_string, "%d   ", microphone);
                sprintf(accelerometer_x_string, "%d   ", accelerometer_x);
                sprintf(accelerometer_y_string, "%d   ", accelerometer_y);
                sprintf(accelerometer_z_string, "%d   ", accelerometer_z);

                // Set the display orientation.
                CF128x128x16_ST7735SSetOrientation(0);

                text_position joystick_x_position;
                text_position joystick_y_position;

                text_position microphone_position;

                text_position accelerometer_x_position;
                text_position accelerometer_y_position;
                text_position accelerometer_z_position;

                text_position joystick_x_text_position;
                text_position joystick_y_text_position;

                text_position microphone_text_position;

                text_position accelerometer_x_text_position;
                text_position accelerometer_y_text_position;
                text_position accelerometer_z_text_position;
                joystick_x_text_position.x = 10;
                joystick_x_text_position.y = 10;
                joystick_x_position.x = value_x_start;
                joystick_x_position.y = 10;

                joystick_y_text_position.x = 10;
                joystick_y_text_position.y = 30;

                joystick_y_text_position.x = 10;
                joystick_y_text_position.y = 30;
                joystick_y_position.x = value_x_start;
                joystick_y_position.y = 30;

                microphone_text_position.x = 10;
                microphone_text_position.y = 50;
                microphone_position.x = value_x_start;
                microphone_position.y = 50;

                accelerometer_x_text_position.x = 10;
                accelerometer_x_text_position.y = 70;
                accelerometer_x_position.x = value_x_start;
                accelerometer_x_position.y = 70;

                accelerometer_y_text_position.x = 10;
                accelerometer_y_text_position.y = 90;
                accelerometer_y_position.x = value_x_start;
                accelerometer_y_position.y = 90;

                accelerometer_z_text_position.x = 10;
                accelerometer_z_text_position.y = 110;
                accelerometer_z_position.x = value_x_start;
                accelerometer_z_position.y = 110;

                draw_string("Stick X:", 15, joystick_x_text_position);
                draw_string(joystick_x_string, strlen(joystick_x_string), joystick_x_position);

                draw_string("Stick Y:", 15, joystick_y_text_position);
                draw_string(joystick_y_string, buffer_length, joystick_y_position);

                draw_string("Mic:", 15, microphone_text_position);
                draw_string(microphone_string, buffer_length, microphone_position);

                draw_string("Acc X:", 15, accelerometer_x_text_position);
                draw_string(accelerometer_x_string, buffer_length, accelerometer_x_position);

                draw_string("Acc y:", 15, accelerometer_y_text_position);
                draw_string(accelerometer_y_string, buffer_length, accelerometer_y_position);

                draw_string("Acc Z:", 15, accelerometer_z_text_position);
                draw_string(accelerometer_z_string, buffer_length, accelerometer_z_position);
            }

            float p = (float)joystick_x / (float)4100;
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (10000 - p * 10000));
        }
    }
}
