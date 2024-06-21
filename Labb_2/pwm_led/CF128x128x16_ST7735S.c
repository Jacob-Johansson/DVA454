/*
 * CF128x128x16_ST7735S.c
 *
 *  Created on: 6 Sep 2021
 *      Author: User
 */


#include <stdint.h>
#include <stdbool.h>

//#include <drivers/SysTickDelay.h>
#include "CF128x128x16_ST7735S.h"

#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"

void SysTickDelayMs(uint32_t ui32Ms) {
    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet()/3
    // 0.001 second = 1 ms = SysCtlClockGet() / 3 / 1000
    SysCtlDelay(ui32Ms * SysCtlClockGet() / 3 / 1000);
  }

#define LCD_HORIZONTAL_MAX  128
#define LCD_VERTICAL_MAX    128

// ST7735S LCD controller Command Set
#define CM_NOP             0x00  // No-op
#define CM_SWRESET         0x01  // Software reset
#define CM_RDDID           0x04  // Read display ID
#define CM_RDDST           0x09  // Read display status
#define CM_RDDPM           0x0A  // Read display power mode
#define CM_RDDMADCTL       0x0B  // Read display MADCTL
#define CM_RDDCOLMOD       0x0C  // Read display pixel format
#define CM_RDDIM           0x0D  // Read display image mode
#define CM_RDDSM           0x0E  // Read display signal mode
#define CM_RDDSDR          0x0F  // Read display self-diagnostic result
#define CM_SLPIN           0x10  // Enter low power sleep, boost converter off
#define CM_SLPOUT          0x11  // Exit sleep & booster on, also performs some self-diagnostic
#define CM_PTLON           0x12  // Partial mode on
#define CM_NORON           0x13  // Partial mode off (normal mode on)
#define CM_INVOFF          0x20  // Display inversion off (Normal)
#define CM_INVON           0x21  // Display inversion on
#define CM_GAMSET          0x26  // Gamma curve select
#define CM_DISPOFF         0x28  // Display off
#define CM_DISPON          0x29  // Display on
#define CM_CASET           0x2A  // Column address set
#define CM_RASET           0x2B  // Row address set
#define CM_RAMWR           0x2C  // Memory write
#define CM_RGBSET          0x2D  // LUT for 4k, 65k, 262k colour display
#define CM_RAMRD           0x2E  // Memory read
#define CM_PTLAR           0x30  // Partial start/end address set
#define CM_SCRLAR          0x33  // Scroll area set
#define CM_TEOFF           0x34  // Tearing effect line off
#define CM_TEON            0x35  // Tearing effect mode set & on
#define CM_MADCTL          0x36  // Memory data access control
#define CM_VSCSAD          0x37  // Scroll start address of ram
#define CM_IDMOFF          0x38  // Idle mode off
#define CM_IDMON           0x39  // Idle mode on
#define CM_COLMOD          0x3A  // Interface pixel format
#define CM_FRMCTR1         0xB1  // Framerate control 1 - Normal mode, full colours
#define CM_FRMCTR2         0xB2  // Framerate control 2 - Idle mode, 8 colours
#define CM_FRMCTR3         0xB3  // Framerate control 3 - Partial mode, full colours
#define CM_INVCTR          0xB4  // Display inversion control
#define CM_PWCTR1          0xC0  // Power control setting 1 - VRH, set GVDD (Gamma reference voltage)
#define CM_PWCTR2          0xC1  // Power control setting 2 - VGH and VGL supply power level
#define CM_PWCTR3          0xC2  // Power control setting 3 - Op-amp current in normal mode, full colours
#define CM_PWCTR4          0xC3  // Power control setting 4 - Op-amp current in idle mode, 8 colours
#define CM_PWCTR5          0xC4  // Power control setting 5 - Op-amp current in partial mode, full colours
#define CM_VMCTR1          0xC5  // VCOM Voltage Setting
#define CM_VMOFCTR         0xC7  // VCOM Offset Control
#define CM_WRID2           0xD1  // Write ID2 Value
#define CM_WRID3           0xD2  // Write ID3 Value
#define CM_NVFCTR1         0xD9  // NVM Control Status
#define CM_NVFCTR2         0xDE  // NVM Read Command
#define CM_NVFCTR3         0xDF  // NVM Write Command
#define CM_GMCTRP1         0xE0  // Gamma ('+' polarity) Correction Characteristics Setting
#define CM_GMCTRN1         0xE1  // Gamma ('-' polarity) Correction Characteristics Setting
#define CM_GCV             0xFC  // Gate Pump Clock Frequency Variable

//MADCTL arguments/bit masks
#define CM_MADCTL_MY       0x80  // Mirror Y-axis         } These 3 bits of MADCTL control
#define CM_MADCTL_MX       0x40  // Mirror X-axis         } the write/read direction,
#define CM_MADCTL_MV       0x20  // Page/column selection } used to rotate/flip the screen.
#define CM_MADCTL_ML       0x10  // Vertical order - LCD vertical refresh direction
#define CM_MADCTL_BGR      0x08  // RGB-BGR read order from LCD controller to LCD panel. Set according to the LCD panel specs.
#define CM_MADCTL_MH       0x04  // Horizontal order - LCD horizontal refresh direction


#define LCD_SSI_PERIPH          SYSCTL_PERIPH_SSI2
#define LCD_SSI_BASE            SSI2_BASE

#define LCD_SSI_GPIO_PERIPH     SYSCTL_PERIPH_GPIOD
#define LCD_CS_PERIPH           SYSCTL_PERIPH_GPION
#define LCD_DC_PERIPH           SYSCTL_PERIPH_GPIOL
#define LCD_RST_PERIPH          SYSCTL_PERIPH_GPIOH

#define LCD_SSI_GPIO_BASE       GPIO_PORTD_BASE
#define LCD_CS_BASE             GPIO_PORTN_BASE
#define LCD_DC_BASE             GPIO_PORTL_BASE
#define LCD_RST_BASE            GPIO_PORTH_BASE

#define LCD_SSI_CLK_CFG         GPIO_PD3_SSI2CLK
#define LCD_SSI_FSS_CFG         GPIO_PD2_SSI2FSS
#define LCD_SSI_TX_CFG          GPIO_PD1_SSI2XDAT0
#define LCD_SSI_RX_CFG          GPIO_PD0_SSI2XDAT1

#define LCD_SSI_CLK_PIN         GPIO_PIN_3
#define LCD_SSI_FSS_PIN         GPIO_PIN_2
#define LCD_SSI_TX_PIN          GPIO_PIN_1
#define LCD_SSI_RX_PIN          GPIO_PIN_0
#define LCD_CS_PIN              GPIO_PIN_2
#define LCD_DC_PIN              GPIO_PIN_3
#define LCD_RST_PIN             GPIO_PIN_3

#define CS_SET_LOW       HWREG(LCD_CS_BASE + (LCD_CS_PIN << 2)) = 0              //0x40064010
#define CS_SET_HIGH      HWREG(LCD_CS_BASE + (LCD_CS_PIN << 2)) = LCD_CS_PIN     //0x40064010
#define DC_SET_LOW       HWREG(LCD_DC_BASE + (LCD_DC_PIN << 2)) = 0              //0x40062020
#define DC_SET_HIGH      HWREG(LCD_DC_BASE + (LCD_DC_PIN << 2)) = LCD_DC_PIN     //0x40062020


#define SSIPUT(x)   while(!(HWREG(LCD_SSI_BASE + SSI_O_SR) & SSI_SR_TNF))   \
                    { }                                                     \
                    HWREG(LCD_SSI_BASE + SSI_O_DR) = x

#define SSIGET(x)   while(!(HWREG(ui32Base + SSI_O_SR) & SSI_SR_RNE))       \
                    { }                                                     \
                    x = HWREG(LCD_SSI_BASE + SSI_O_DR)

#define SSIBUSY     ((HWREG(LCD_SSI_BASE + SSI_O_SR) & SSI_SR_BSY) ? true : false)

#define COLOR24TO16(x) ((((x) & 0x00F80000) >> 8) | (((x) & 0x0000FC00) >> 5) | (((x) & 0x000000F8) >> 3))

static LCD_Orientation LcdOrientation = LCD_ORIENTATION_UP;

//#pragma FUNC_ALWAYS_INLINE(Transfer)
static inline void Transfer(uint_fast8_t data)
{
    // Write data to the output FIFO. This is a blocking operation,
	// meaning there's no need to check if the SSI peripheral busy beforehand.
    SSIPUT(data);

    // Wait until the data is transferred
    while(SSIBUSY);
}

//#pragma FUNC_ALWAYS_INLINE(Transfer16)
static inline void Transfer16(uint_fast8_t high, uint_fast8_t low)
{
	// Write data to the output FIFO. This is a blocking operation,
	// meaning there's no need to check if the SSI peripheral busy beforehand.
    SSIPUT(high);
    SSIPUT(low);

    // Wait until the data is transferred
    while(SSIBUSY);
}

static inline void WriteRegister(uint_fast8_t command, uint_fast8_t parameter)
{
    // Data Command low indicates incoming command, Chip Select low initiates the receiver.
    DC_SET_LOW;
    CS_SET_LOW;

    Transfer(command);

    // Data Command high indicates incoming data.
    DC_SET_HIGH;

	Transfer(parameter);

	// Chip Select high ends the transfer
	CS_SET_HIGH;
}

static inline void WriteRegister2(uint_fast8_t command, uint_fast8_t parameter1, uint_fast8_t parameter2)
{
	// Data Command low indicates incoming command, Chip Select low initiates the receiver.
	DC_SET_LOW;
	CS_SET_LOW;

	Transfer(command);

	// Data Command high indicates incoming data.
	DC_SET_HIGH;

	Transfer(parameter1);
	Transfer(parameter2);

    // Chip Select high ends the transfer
    CS_SET_HIGH;
}

static inline void WriteCommand(uint_fast8_t command)
{
    // Data Command low indicates incoming command, Chip Select low initiates the receiver.
    DC_SET_LOW;
    CS_SET_LOW;

    Transfer(command);

    // Chip Select high ends the transfer, then set Data Command pin high again.
    CS_SET_HIGH;
    DC_SET_HIGH;
}

//#pragma FUNC_ALWAYS_INLINE(WriteData)
static inline void WriteData(uint_fast8_t data)
{
    // Set Chip select pin low to start transfer
    CS_SET_LOW;

    Transfer(data);

    // Set Chip select pin high, ending transfer
    CS_SET_HIGH;
}

//#pragma FUNC_ALWAYS_INLINE(WriteData16)
static inline void WriteData16(uint_fast16_t data)
{
    // Set Chip select pin low to start transfer
    CS_SET_LOW;

    Transfer16(data >> 8, data & 0x000000FF);
    //Transfer((uint_fast8_t)(data >> 8));
    //Transfer((uint_fast8_t)(data & 0x000000FF));

    // Set Chip select pin high, ending transfer
    CS_SET_HIGH;
}

// Sets the start and boundaries of the column and row pointers in the display controller.
// Any data written after a RAMWR command (and before any other command) will be stored
// in GRAM at the current column-row pointer, which in turn will increment on each write
// according to the display orientation, i.e the configured MADCTL flags.
// Basically, tell the display controller the boundary of where we want to draw pixels.
// x0      Top-left corner of the boundary. Given as param 1 & 2 to CASET, is written to SC[15:0].
// y0      Top-left corner of the boundary. Given as param 1 & 2 to RASET, is written to SP[15:0].
// x1      Bottom-Right corner of the boundary. Given as param 3 & 4 to CASET, is written to EC[15:0].
// y1      Bottom-Right corner of the boundary. Given as param 3 & 4 to RASET, is written to EP[15:0].
static void SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // There's no need to check the coordinates are within the screen area, the lcd controller
    // will simply play along and discard any data written to a column or row outside of its
    // configured limits. However the datasheet seems pretty adamant that x1 >= x0 and y1 >= y0,
    // so why not, let's add a check. Also all x and y must be >= 0, hence unsigned.

    if(x0 > x1)
    {
        uint16_t swap = x0;
        x0 = x1;
        x1 = swap;
    }
    if(y0 > y1)
    {
        uint16_t swap = y0;
        y0 = y1;
        y1 = swap;
    }

    // The CFAF128128B-0145T LCD display is 128x128 pixels, however its ST7735S controller
    // only supports one square panel size at 132x132 pixels. The GRAM table is, of course,
    // not aligned to one of the corners, which means the outer-most rows and columns do not
    // actually correspond to any pixels. This switch adjusts the draw window to compensate.
    switch (LcdOrientation) {
            case 0:
                x0 += 2;
                y0 += 3;
                x1 += 2;
                y1 += 3;
                break;
            case 1:
                x0 += 3;
                y0 += 2;
                x1 += 3;
                y1 += 2;
                break;
            case 2:
                x0 += 2;
                y0 += 1;
                x1 += 2;
                y1 += 1;
                break;
            case 3:
                x0 += 1;
                y0 += 2;
                x1 += 1;
                y1 += 2;
                break;
            default:
                break;
        }

    // Column address set
    WriteCommand(CM_CASET);
    WriteData16(x0);
    WriteData16(x1);

    // Row address set
    WriteCommand(CM_RASET);
    WriteData16(y0);
    WriteData16(y1);

    // Memory write enable
    WriteCommand(CM_RAMWR);
}

// Draws a single pixel
// pvDisplayData    A pointer to driver specific data (usually unused)
// i32X             The X coordinate of the pixel
// i32Y             The X coordinate of the pixel
// ui32Value        The pixel colour
static void CF128x128x16_ST7735SPixelDraw(void *pvDisplayData, int32_t i32X, int32_t i32Y, uint32_t ui32Value)
{
    // The display is square, so this check doesn't have to consider orientation.
    if( (i32X >= LCD_HORIZONTAL_MAX) || (i32Y >= LCD_VERTICAL_MAX) )
    {
        return;
    }
    SetWindow(i32X, i32Y, i32X+1, i32Y+1);
    WriteData16(ui32Value);
}

// Draws a horizontal sequence of pixels on the screen
// pvDisplayData    A pointer to driver specific data (usually unused)
// i32X             The X coordinate of the pixel
// i32Y             The X coordinate of the pixel
// i32X0            The sub-pixel offset, valid for 1 or 4 bit per pixel formats
// i32Count         The number of pixels to draw
// i32BPP           The number of bits per pixel, must be 1, 4 or 8 (why is this an i32? why TI?)
// pui8Data         Pointer to the pixel data. For 1 and 4 BPP formats, the MSB represents the left-most pixel
// pui8Palette      Pointer to the palette used to draw the pixels
static void CF128x128x16_ST7735SPixelDrawMultiple(void *pvDisplayData, int32_t i32X, int32_t i32Y, int32_t i32X0,
                                              int32_t i32Count, int32_t i32BPP,
                                              const uint8_t *pui8Data, const uint8_t *pui8Palette)
{
    uint32_t data;

    // The third argument is fine even if (i32X + i32Count) > LCD_HORIZONTAL_MAX.
    // The ST7735S controller will happily set its row and column pointers outside of the display area,
    // where it will simply ignore any data written while the column/row pointers are OOB. This should let
    // oversized draws or long text overflow outside the bounds of the display area.
    SetWindow(i32X, i32Y, i32X + i32Count, LCD_VERTICAL_MAX);

    switch(i32BPP)
    {
    case 1:
        while(i32Count) // While we have pixels to draw
        {
            data = *pui8Data++; // Get the next byte of image data
            for(; (i32X0 < 8) && i32Count; i32X0++, i32Count--) // Loop through those bytes
            {
                WriteData16( ((uint32_t *)pui8Palette)[ (data >> (7 - i32X0)) & 1] ); // oof
            }
            i32X0 = 0;  // Start over at the next byte
        }
        break;

    case 4:
        switch(i32X0 & 1)
        {
        case 0:
            while(i32Count)
            {
                data = (*pui8Data >> 4) * 3; // Get the upper nibble of the next byte
                data = (*(uint32_t *)(pui8Palette + data) & 0x00FFFFFF); // Get the corresponding 24-bit colour entry
                WriteData16(COLOR24TO16(data)); // Translate to 16-bit 5-6-5 color and transmit
                i32Count--;

                if(i32Count)
                {
        case 1:         //Yep, this is some fuckery. Google "Duff's device"
                data = (*pui8Data++ & 15)* 3; //Get the lower nibble of the next byte, then same deal as above
                data = (*(uint32_t *)(pui8Palette + data) & 0x00FFFFFF);
                WriteData16(COLOR24TO16(data));
                i32Count--;
                }
            }
        }
        break;

    case 8:
        while(i32Count--)
        {
            data = *pui8Data++ * 3;  // Get the next pixel byte
            data = *(uint32_t *)(pui8Palette + data) & 0x00FFFFFF;   // Get the 24-bit colour for said byte
            WriteData16(COLOR24TO16(data)); // 24bit to 5-6-5bit colour, and away it goes.
        }
        break;

    case 16:    // Native format for this screen. Doesn't conform to grlib, but useful for images.
        while(i32Count--)
        {
            WriteData16( *((uint16_t *)pui8Data) ); // Cast to pointer to uint16_t, dereference for value
            pui8Data += 2; // Move the pointer two bytes forward.
        }
        break;
    }
}

// Draws a horizontal line.
// The argument coordinates are assumed to be inside the display area.
// pvDisplayData    A pointer to driver specific data (usually unused)
// i32X1            The X coordinate of the start of the line
// i32X2            The X coordinate of the end of the line
// i32Y             The Y coordinate of the line
// ui32Value        The line colour
static void CF128x128x16_ST7735SLineDrawH(void *pvDisplayData, int32_t i32X1, int32_t i32X2, int32_t i32Y, uint32_t ui32Value)
{
    SetWindow(i32X1, i32Y, i32X2, i32Y);

    int i;

    for(i = i32X1; i <= i32X2; i++)
    {
        WriteData16((uint16_t)ui32Value);
    }
}

// Draws a vertical line.
// The argument coordinates are assumed to be inside the display area.
// pvDisplayData    A pointer to driver specific data (usually unused)
// i32X             The X coordinate of the line
// i32Y1            The Y coordinate of the start of the line
// i32Y2            The Y coordinate of the end of the line
// ui32Value        The line colour
static void CF128x128x16_ST7735SLineDrawV(void *pvDisplayData, int32_t i32X, int32_t i32Y1, int32_t i32Y2, uint32_t ui32Value)
{
    SetWindow(i32X, i32Y1, i32X, i32Y2);

    int i;

    for(i = i32Y1; i <= i32Y2; i++)
    {
        WriteData16((uint16_t)ui32Value);
    }
}

static void CF128x128x16_ST7735SRectFill(void *pvDisplayData, const tRectangle *psRect, uint32_t ui32Value)
{
    uint16_t x0 = (uint16_t)(psRect->i16XMin);
    uint16_t x1 = (uint16_t)(psRect->i16XMax);
    uint16_t y0 = (uint16_t)(psRect->i16YMin);
    uint16_t y1 = (uint16_t)(psRect->i16YMax);

    if(x0 > x1)
    {
        // TODO: swap
    }
    if(y0 > y1)
    {
        // TODO: swap
    }

    // Tell the display the boundaries of where we want to draw
    SetWindow(x0, y0, x1, y1);

    // Despite being a 32-bit variable we only deal in at most 24 bit colour.
    // The LCD is configured for only 16 bits per pixel however, so we only need to send as much.
    // This assumes the colour value has already been converted, otherwise we'll be sending 2/3 colour bytes.
    uint_fast8_t highByte = (uint_fast8_t)(ui32Value >> 8);
    uint_fast8_t lowByte  = (uint_fast8_t)(ui32Value & 0xFF);

    // Make sure data/command pin is high and set chip-select low to enable sending data
    GPIOPinWrite(LCD_DC_BASE, LCD_DC_PIN, LCD_DC_PIN);
    GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, 0);

    // The size calculation is bottom-right inclusive (hence the +1's) as per the grlib documentation.
    uint32_t i = (uint32_t)(y1 - y0 + 1) * (x1 - x0 + 1);
    for(; i > 0; i--)
    {
        Transfer16(highByte, lowByte);
    }

    GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);
}

// Translates 24-bit RGB colour into a format which the display can handle.
// In this case, the LCD is configured for 16-bit colour, so we need 5-6-5 bit RGB.
// We still want it packed into a 32-bit int so it'll adhere to the driver API.
static uint32_t CF128x128x16_ST7735SColorTranslate(void *pvDisplayData, uint32_t ui32Value)
{
    // Mask out the most significant bits for each colour, shift them to fit inside 16-bits and OR it all together.
    return COLOR24TO16(ui32Value);
}

// Used for displays without a frame buffer, writes changes in RAM to the display.
// Not used by this driver, but is still a part of the API.
static void CF128x128x16_ST7735SFlush(void *pvDisplayData)
{
    // do nothing
}

static void SSIInit(uint32_t sysClock)
{
    // Enable the SSI peripheral
    SysCtlPeripheralEnable(LCD_SSI_PERIPH);

    SSIDisable(LCD_SSI_BASE);

    //Enable the required GPIO peripheral ports
    SysCtlPeripheralEnable(LCD_SSI_GPIO_PERIPH);
    SysCtlPeripheralEnable(LCD_CS_PERIPH);
    SysCtlPeripheralEnable(LCD_DC_PERIPH);
    SysCtlPeripheralEnable(LCD_RST_PERIPH);

    // Configure pins as outputs
    GPIOPinTypeGPIOOutput(LCD_CS_BASE, LCD_CS_PIN);
    GPIOPinTypeGPIOOutput(LCD_DC_BASE, LCD_DC_PIN);
    GPIOPinTypeGPIOOutput(LCD_RST_BASE, LCD_RST_PIN);


    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, LCD_RST_PIN);
    GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);
    GPIOPinWrite(LCD_DC_BASE, LCD_DC_PIN, LCD_DC_PIN);

    // Pin muxing setup for the SSI pins
    GPIOPinConfigure(LCD_SSI_CLK_CFG);
    //GPIOPinConfigure(LCD_SSI_FSS_CFG);
    GPIOPinConfigure(LCD_SSI_TX_CFG);
    //GPIOPinConfigure(LCD_SSI_RX_CFG);

    // Configure the SSI pins as alternate function pins, which lets the SSI peripheral control them
    //GPIOPinTypeSSI(LCD_SSI_GPIO_BASE, (LCD_SSI_CLK_PIN | LCD_SSI_FSS_PIN | LCD_SSI_TX_PIN | LCD_SSI_RX_PIN) );
    GPIOPinTypeSSI(LCD_SSI_GPIO_BASE, (LCD_SSI_CLK_PIN | LCD_SSI_TX_PIN) );

    // Up the drive from the default 2ma to 4ma on TX/DAT0 and SCL for sharper edges.
    // Probably not necessary, but they looked a bit sluggish at 15Mhz on my scope.
    GPIOPadConfigSet(LCD_SSI_GPIO_BASE, (LCD_SSI_CLK_PIN | LCD_SSI_TX_PIN), GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    // Since CS and DC are active low, and should stay high most of the time, configure them with pull-ups.
    // Again, probably not strictly necessary, but eh, it sounds like good practice.
    GPIOPadConfigSet(LCD_CS_BASE, LCD_CS_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(LCD_DC_BASE, LCD_DC_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Set SSI to 15Mhz, master mode and 8 bit data width.
    SSIConfigSetExpClk(LCD_SSI_BASE, sysClock, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 15000000, 8);

    // Enable the SSI module
    SSIEnable(LCD_SSI_BASE);

    // Make sure the RX FIFO is empty. Other implementations do this so presumably it's important.
    uint32_t trash;
    while(SSIDataGetNonBlocking(LCD_SSI_BASE, &trash))
    { }
}

void CF128x128x16_ST7735SInit(uint32_t sysClock)
{

    SSIInit(sysClock);

    // Reset the display via reset pin.
    // Timing requirements are something like 10µs minimum, shorter than 5µ and it rejects the reset.
    // Also, there's a 5ms wait after reset before any commands can be issued, and 120ms before SLPOUT can be sent.
    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, 0);
    SysTickDelayMs(1);
    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, LCD_RST_PIN);
    SysTickDelayMs(120);

    // Software reset. Seems unecessary, but it changes a few registers from their HW reset values.
    WriteCommand(CM_SWRESET);
    SysTickDelayMs(120);

    // There is also a required 120ms delay after a sleep-out command.
    WriteCommand(CM_SLPOUT);
    SysTickDelayMs(120);

    //Gamma curve select
    WriteRegister(CM_GAMSET, 0x04);

    // Set the framerate during normal operation See ST7753S datasheet for details.
    // This command code (0xB1) is switched with power control options on the (supposedly) equivalent HX8353.
    // It should be fine though, SETPOWER (0xB1) on HX8353 is a restricted extended command, meaning
    // unless the EXTC pin is tied low, or a correct PASSWDEN command is given, it will register as a no-op.
	WriteRegister2(CM_FRMCTR1, 0x0A, 0x14);

	WriteRegister2(CM_PWCTR1, 0x0A, 0x00);

    WriteRegister2(CM_PWCTR3, 0x13, 0x00);


    //

    // Interface pixel format, set to 16-bit/pixel
    // Options are 0x03 for 12-bit, 0x05 for 16-bit or 0x06 for 18-bit, but only really 16-bit is practical for SPI use.
    WriteRegister(CM_COLMOD, 0x05);
    SysTickDelayMs(10);

    // Memory data access control - set BGR read order from LCD controller to LCD panel.
    // This is an internal setting for how the LCD panel wants its pixel values served from the controller.
    // The controller still wants pixels in the order of R-G-B from us.
	WriteRegister(CM_MADCTL, CM_MADCTL_BGR);

    // Partial mode off, go to normal mode.
    // Partial mode segments the GRAM into 128x128 pages, so you can write a full screen to GRAM before displaying it.
    // Normal mode updates the display on write, but allows fancy stuff like scrolling pages.
	WriteCommand(CM_NORON);
	SysTickDelayMs(10);

    // Turn on display
    WriteCommand(CM_DISPON);
    SysTickDelayMs(120);

    // Tell the screen which way is up
    CF128x128x16_ST7735SSetOrientation(LCD_ORIENTATION_UP);

    // Overwrite the frame buffer in the LCD. The buffer keeps through resets, but contains garbage after power loss.
    CF128x128x16_ST7735SClear(0);
}

void CF128x128x16_ST7735SSetOrientation(LCD_Orientation orientation)
{
    LcdOrientation = orientation;
    WriteCommand(CM_MADCTL);
    switch (orientation)
    {
        case LCD_ORIENTATION_UP:
            WriteData(CM_MADCTL_MX | CM_MADCTL_MY | CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_LEFT:
            WriteData(CM_MADCTL_MY | CM_MADCTL_MV | CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_DOWN:
            WriteData(CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_RIGHT:
            WriteData(CM_MADCTL_MX | CM_MADCTL_MV | CM_MADCTL_BGR);
            break;

    }
}

void CF128x128x16_ST7735SClear(uint32_t colour)
{
    //blank the display
    tRectangle displayArea =
    {
         0,
         0,
         LCD_HORIZONTAL_MAX,
         LCD_VERTICAL_MAX
    };

    // If there's any data above 16 bits, it must be a 24 bit colour, in which case translate it to 16 bit.
    // Any 24 bit colour where the red channel = 0 would still get through, but oh well.
    if(colour & 0xFFFF0000)
    {
        colour = CF128x128x16_ST7735SColorTranslate(0, colour);
    }

    CF128x128x16_ST7735SRectFill(0, &displayArea, colour);
}

const tDisplay g_sCF128x128x16_ST7735S =
{
     sizeof(tDisplay),
     0,
     LCD_HORIZONTAL_MAX,
     LCD_VERTICAL_MAX,
     CF128x128x16_ST7735SPixelDraw,
     CF128x128x16_ST7735SPixelDrawMultiple,
     CF128x128x16_ST7735SLineDrawH,
     CF128x128x16_ST7735SLineDrawV,
     CF128x128x16_ST7735SRectFill,
     CF128x128x16_ST7735SColorTranslate,
     CF128x128x16_ST7735SFlush
};
