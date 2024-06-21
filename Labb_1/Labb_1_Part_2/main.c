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

// Main Function
int main(void)
{
    unsigned char ucDelta, ucState;
    bool shouldBeLit = false;

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

        // Check if the LED should be lit or not
       if(shouldBeLit)
       {
            // If the LEFT_BUTTON is pressed, turn off the LED
           if(BUTTON_PRESSED(LEFT_BUTTON, ucState, 1))
           {
               shouldBeLit = false;
           }
           // Otherwise, turn on the LED
           else
                LEDWrite(CLP_D1, 1);
       }
       else
       {
            // If the LEFT_BUTTON is pressed, turn on the LED
          if(BUTTON_PRESSED(LEFT_BUTTON, ucState, 1))
          {
              LEDWrite(CLP_D1, 1);
          }
          // Otherwise, turn off the LED
          else
              LEDWrite(CLP_D1, 0);
       }

        // Toggle the shouldBeLit flag if the RIGHT_BUTTON is pressed
       if(BUTTON_PRESSED(RIGHT_BUTTON, ucState, ucDelta))
       {
           shouldBeLit = !shouldBeLit;
       }
   }
}
