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

int main(void)
{
    // Configure the device pins
    PinoutSet(false, false);

    // Initialize the button driver.
    ButtonsInit();

    // Enable the GPIO peripheral for port N (GPION).
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    int previousIndex = 1;
    int nextIndex = 1;

    volatile uint32_t delay;
    volatile uint32_t delayTime = 200000;

    while (1)
    {
        // Forward LED sequence
        for(nextIndex = 1; nextIndex <= 8; nextIndex *= 2)
        {
            // Turn off the previously lit LED and turn on the next LED
            LEDWrite(previousIndex, 0);
            LEDWrite(nextIndex, nextIndex);

            // Delay to control LED blinking speed
            for(delay = 0; delay < delayTime; delay++){}


            previousIndex = nextIndex;
        }

        // Backward LED sequence
        for(nextIndex = 8; nextIndex > 0; nextIndex /= 2)
        {
            // Turn off the previously lit LED and turn on the next LED
            LEDWrite(previousIndex, 0);
            LEDWrite(nextIndex, nextIndex);

            // Delay to control LED blinking speed
            for(delay = 0; delay < delayTime; delay++){}


            previousIndex = nextIndex;
        }
    }
 }
