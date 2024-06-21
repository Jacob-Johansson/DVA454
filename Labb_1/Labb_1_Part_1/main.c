#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "drivers/buttons.h"
#include "drivers/pinout.h"

// The error routine that is called if the driver library
// encounters an error.
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
 while(1);
}
#endif

// States of the LED.
enum BlinkingStates
{
    None,
    On,
    Off,
};

// Main Function
int main(void)
{
     unsigned char ucDelta, ucState;

     int blinkCounter = 0;
     int blinkCountLimit = 100000;
     enum BlinkingStates state = None;


     // Configure the device pins
     PinoutSet(false, false);

      // Initialize the button driver.
      ButtonsInit();

      // Enable the GPIO pin for the LED (PN0).
      // Set the direction as output, and
      // enable the GPIO pin for digital function.
      GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

      // Loop forever.
      while(1)
      {
          // Poll the buttons.
          ucState = ButtonsPoll(&ucDelta, 0);

          // If right button was pressed, then switch to "on" state.
          if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
          {
              state = On;
          }

          switch(state)
          {

          // Turn LED off.
          case None:
              LEDWrite(CLP_D1, 0);
              break;

          // Set LED to be on with a "blinkCounter" as delay variable.
          case On:
              if(blinkCounter < blinkCountLimit)
              {
                  LEDWrite(CLP_D1, 1);
                  blinkCounter++;
              }
              else
              {
                  state = Off;
                  blinkCounter = 0;
              }

          break;

          // Set LED to be off with a "blinkCounter" as delay variable.
          case Off:
              if(blinkCounter < blinkCountLimit)
              {
                  LEDWrite(CLP_D1, 0);
                  blinkCounter++;
              }

              else
              {
                  state = On;
                  blinkCounter = 0;
              }
              break;

          default:
            break;
          }

          // If left button was pressed, then switch to "default" state which is off.
          if(BUTTON_PRESSED(LEFT_BUTTON, ucState, ucDelta))
          {
              state = None;
          }

      }
 }
