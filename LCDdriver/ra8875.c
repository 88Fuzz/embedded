#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ra8875.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"





LCDdisp ra8875;

void initRA8875pins(uint32_t base_cs, uint32_t base_rst,
		uint32_t cs_pin, uint32_t rst_pin, uint32_t base_ssi,
		uint32_t SSICLK_pin, uint32_t SSIRX_pin, uint32_t SSITX_pin,
		uint16_t width, uint16_t height)
{
	ra8875.base_cs=base_cs;
	ra8875.base_rst=base_rst;
	ra8875.cs=cs_pin;
	ra8875.rst=rst_pin;
	ra8875.base_ssi=base_ssi;
	ra8875.ssi_clk=SSICLK_pin;
	ra8875.ssi_rx=SSIRX_pin;
	ra8875.ssi_tx=SSITX_pin;
  ra8875.width=width;
  ra8875.height=height;
}

bool ra_begin()
{
  //TODO set CS and RST pins as GPIO based on actual ra8875 values!!!!
	uint8_t j;
	uint32_t tmp;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(ra8875.base_cs,ra8875.cs|ra8875.rst);

	//set cs high
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
	GPIOPinWrite(ra8875.base_rst, ra8875.rst, 0xFF);

	//WAIT 1ms
	SysCtlDelay(ONE_MILISEC);//assumes 80MHz clk

	//set cs low
//	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);

	//set rst low
	GPIOPinWrite(ra8875.base_rst, ra8875.rst, 0);

	//WAIT 1ms
	SysCtlDelay(ONE_MILISEC);//assumes 80MHz clk

	//set rst high
	GPIOPinWrite(ra8875.base_rst, ra8875.rst, 0xFF);

	for(j=0;j<10;j++)
		SysCtlDelay(ONE_MILISEC);

	//TODO MOVE THIS TO initRA8875pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	//GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	//GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_2);

	tmp=SysCtlClockGet();
	tmp/=640;
	//SPI.setClockDivider(SPI_CLOCK_DIV128);    SPI.setDataMode(SPI_MODE0);
	SSIConfigSetExpClk(SSI0_BASE, 640, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1, 8);
	tmp=SSIClockSourceGet(SSI0_BASE);

	SSIEnable(SSI0_BASE);

	ra_initialize();

	tmp=SysCtlClockGet()/20;
	//SPI.setClockDivider(SPI_CLOCK_DIV4);
	//SSIClockSourceSet(SSI0_BASE, 20);
	SSIDisable(SSI0_BASE);
	SSIConfigSetExpClk(SSI0_BASE, 20, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1, 8);
	SSIEnable(SSI0_BASE);

	tmp=SSIClockSourceGet(SSI0_BASE);

  return true;
}

void ra_PLLinit()
{
	writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
	SysCtlDelay(ONE_MILISEC);//assumes 80MHz clk
  	writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
  	SysCtlDelay(ONE_MILISEC);//assumes 80MHz clk
}

void ra_initialize()
{
  /* Timing values */
  uint8_t pixclk;
  uint8_t hsync_start;
  uint8_t hsync_pw;
  uint8_t hsync_finetune;
  uint8_t hsync_nondisp;
  uint8_t vsync_pw;
  uint16_t vsync_nondisp;
  uint16_t vsync_start;

  ra_PLLinit();
  writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

  /* Set the correct values for the display being used */
  pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
  hsync_nondisp   = 10;
  hsync_start     = 8;
  hsync_pw        = 48;
  hsync_finetune  = 0;
  vsync_nondisp   = 3;
  vsync_start     = 8;
  vsync_pw        = 10;

  writeReg(RA8875_PCSR, pixclk);
  SysCtlDelay(ONE_MILISEC);//assumes 80MHz clk

  /* Horizontal settings registers */
  writeReg(RA8875_HDWR, (ra8875.width / 8) - 1);                          // H width: (HDWR + 1) * 8 = 480
  writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
  writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2)/8);    // H non-display: HNDR * 8 + HNDFTR + 2 = 10
  writeReg(RA8875_HSTR, hsync_start/8 - 1);                         // Hsync start: (HSTR + 1)*8
  writeReg(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw/8 - 1));        // HSync pulse width = (HPWR+1) * 8

  /* Vertical settings registers */
  writeReg(RA8875_VDHR0, (uint16_t)(ra8875.height - 1) & 0xFF);
  writeReg(RA8875_VDHR1, (uint16_t)(ra8875.height - 1) >> 8);
  writeReg(RA8875_VNDR0, vsync_nondisp-1);                          // V non-display period = VNDR + 1
  writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
  writeReg(RA8875_VSTR0, vsync_start-1);                            // Vsync start position = VSTR + 1
  writeReg(RA8875_VSTR1, vsync_start >> 8);
  writeReg(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);            // Vsync pulse width = VPWR + 1

  /* Set active window X */
  writeReg(RA8875_HSAW0, 0);                                        // horizontal start point
  writeReg(RA8875_HSAW1, 0);
  writeReg(RA8875_HEAW0, (uint16_t)(ra8875.width - 1) & 0xFF);            // horizontal end point
  writeReg(RA8875_HEAW1, (uint16_t)(ra8875.width - 1) >> 8);

  /* Set active window Y */
  writeReg(RA8875_VSAW0, 0);                                        // vertical start point
  writeReg(RA8875_VSAW1, 0);
  writeReg(RA8875_VEAW0, (uint16_t)(ra8875.height - 1) & 0xFF);           // horizontal end point
  writeReg(RA8875_VEAW1, (uint16_t)(ra8875.height - 1) >> 8);

  //TODO: Setup touch panel?

  /* Clear the entire window */
  writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);

  //WAIT 500ms;
  delay500();
}

void delay500()
{
	SysCtlDelay(FIVEHUNDRED_MILISEC);
}

/**************************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)
*/
/**************************************************************************/
void textMode()
{
  /* Set text mode */
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
  writeData(temp);

  /* Select the internal (ROM) font */
  writeCommand(0x21);
  temp = readData();
  temp &= ~((1<<7) | (1<<5)); // Clear bits 7 and 5
  writeData(temp);
}

/**************************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)

      @args x[in] The x position of the cursor (in pixels, 0..1023)
      @args y[in] The y position of the cursor (in pixels, 0..511)
*/
/**************************************************************************/
void textSetCursor(uint16_t x, uint16_t y)
{
  /* Set cursor location */
  writeCommand(0x2A);
  writeData(x & 0xFF);
  writeCommand(0x2B);
  writeData(x >> 8);
  writeCommand(0x2C);
  writeData(y & 0xFF);
  writeCommand(0x2D);
  writeData(y >> 8);
}

/**************************************************************************/
/*!
      Sets the fore and background color when rendering text

      @args foreColor[in] The RGB565 color to use when rendering the text
      @args bgColor[in]   The RGB565 colot to use for the background
*/
/**************************************************************************/
void textColor(uint16_t foreColor, uint16_t bgColor)
{
  /* Set Fore Color */
  writeCommand(0x63);
  writeData((foreColor & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((foreColor & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((foreColor & 0x001f));

  /* Set Background Color */
  writeCommand(0x60);
  writeData((bgColor & 0xf800) >> 11);
  writeCommand(0x61);
  writeData((bgColor & 0x07e0) >> 5);
  writeCommand(0x62);
  writeData((bgColor & 0x001f));

  /* Clear transparency flag */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp &= ~(1<<6); // Clear bit 6
  writeData(temp);
}

/**************************************************************************/
/*!
      Sets the text enlarge settings, using one of the following values:

      0 = 1x zoom
      1 = 2x zoom
      2 = 3x zoom
      3 = 4x zoom

      @args scale[in]   The zoom factor (0..3 for 1-4x zoom)
*/
/**************************************************************************/
void textEnlarge(uint8_t scale)
{
  if (scale > 3) scale = 3;

  /* Set font size flags */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp &= ~(0xF); // Clears bits 0..3
  temp |= scale << 2;
  temp |= scale;
  writeData(temp);

  ra8875.textScale = scale;
}

/**************************************************************************/
/*!
      Renders some text on the screen when in text mode

      @args buffer[in]    The buffer containing the characters to render
      @args len[in]       The size of the buffer in bytes
*/
/**************************************************************************/
void textWrite(const char* buffer, uint16_t len)
{
	uint16_t i;
	if (len == 0)
		len = strlen(buffer);
	writeCommand(RA8875_MRWC);
	for (i=0;i<len;i++)
	{
		writeData(buffer[i]);
    //TODO may need to add delay here


/*#if defined(__AVR__)
    if (_textScale > 1) delay(1);
#elif defined(__arm__)
    // This delay is needed with textEnlarge(1) because
    // Teensy 3.X is much faster than Arduino Uno
    if (_textScale > 0) delay(1);
#endif*/
	}
}

/************************* Graphics ***********************************/

/**************************************************************************/
/*!
      Sets the display in graphics mode (as opposed to text mode)
*/
/**************************************************************************/
void graphicsMode(void) {
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
  writeData(temp);
}

/**************************************************************************/
/*!
      Waits for screen to finish by polling the status!
*/
/**************************************************************************/
bool waitPoll(uint8_t regname, uint8_t waitflag) {
  /* Wait for the command to finish */
  while (1)
  {
    uint8_t temp = readReg(regname);
    if (!(temp & waitflag))
      return true;
  }
  return false; // MEMEFIX: yeah i know, unreached! - add timeout?
}

/**************************************************************************/
/*!
      Sets the current X/Y position on the display before drawing

      @args x[in] The 0-based x location
      @args y[in] The 0-base y location
*/
/**************************************************************************/
void setXY(uint16_t x, uint16_t y) {
  writeReg(RA8875_CURH0, x);
  writeReg(RA8875_CURH1, x >> 8);
  writeReg(RA8875_CURV0, y);
  writeReg(RA8875_CURV1, y >> 8);
}

/**************************************************************************/
/*!
      HW accelerated function to push a chunk of raw pixel data

      @args num[in] The number of pixels to push
      @args p[in]   The pixel color to use
*/
/**************************************************************************/
void pushPixels(uint32_t num, uint16_t p) {
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);

	//SPI.transfer(RA8875_DATAWRITE);
	SSIDataPut(ra8875.base_ssi, RA8875_DATAWRITE);
	while (num--)
	{
		//SPI.transfer(p >> 8);
		//SPI.transfer(p);
		SSIDataPut(ra8875.base_ssi, p>>8);
		SSIDataPut(ra8875.base_ssi, p);
	}

	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void fillRect_void() {
  writeCommand(RA8875_DCR);
  writeData(RA8875_DCR_LINESQUTRI_STOP | RA8875_DCR_DRAWSQUARE);
  writeData(RA8875_DCR_LINESQUTRI_START | RA8875_DCR_FILL | RA8875_DCR_DRAWSQUARE);
}

/**************************************************************************/
/*!
      Draws a single pixel at the specified location

      @args x[in]     The 0-based x location
      @args y[in]     The 0-base y location
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawPixel(int16_t x, int16_t y, uint16_t color)
{
	writeReg(RA8875_CURH0, x);
	writeReg(RA8875_CURH1, x >> 8);
	writeReg(RA8875_CURV0, y);
	writeReg(RA8875_CURV1, y >> 8);
	writeCommand(RA8875_MRWC);
	//digitalWrite(_cs, LOW);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);
	//SPI.transfer(RA8875_DATAWRITE);
	SSIDataPut(ra8875.base_ssi, RA8875_DATAWRITE);
	//SPI.transfer(color >> 8);
	SSIDataPut(ra8875.base_ssi, color>>8);
	//SPI.transfer(color);
	SSIDataPut(ra8875.base_ssi, color);
	//digitalWrite(_cs, HIGH);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
}

/**************************************************************************/
/*!
      Draws a HW accelerated line on the display

      @args x0[in]    The 0-based starting x location
      @args y0[in]    The 0-base starting y location
      @args x1[in]    The 0-based ending x location
      @args y1[in]    The 0-base ending y location
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  /* Set X */
  writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y0);
  writeCommand(0x94);
  writeData(y0 >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData((x1) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(y1);
  writeCommand(0x98);
  writeData((y1) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  writeData(0x80);

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  drawLine(x, y, x, y+h, color);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  drawLine(x, y, x+w, y, color);
}

/**************************************************************************/
/*!
      Draws a HW accelerated rectangle on the display

      @args x[in]     The 0-based x location of the top-right corner
      @args y[in]     The 0-based y location of the top-right corner
      @args w[in]     The rectangle width
      @args h[in]     The rectangle height
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  rectHelper(x, y, x+w, y+h, color, 0);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled rectangle on the display

      @args x[in]     The 0-based x location of the top-right corner
      @args y[in]     The 0-based y location of the top-right corner
      @args w[in]     The rectangle width
      @args h[in]     The rectangle height
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  rectHelper(x, y, x+w, y+h, color, 1);
}

/**************************************************************************/
/*!
      Fills the screen with the spefied RGB565 color

      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillScreen(uint16_t color)
{
  rectHelper(0, 0, ra8875.width-1, ra8875.height-1, color, 1);
}

/**************************************************************************/
/*!
      Draws a HW accelerated circle on the display

      @args x[in]     The 0-based x location of the center of the circle
      @args y[in]     The 0-based y location of the center of the circle
      @args w[in]     The circle's radius
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  circleHelper(x0, y0, r, color, 0);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled circle on the display

      @args x[in]     The 0-based x location of the center of the circle
      @args y[in]     The 0-based y location of the center of the circle
      @args w[in]     The circle's radius
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  circleHelper(x0, y0, r, color, 1);
}

/**************************************************************************/
/*!
      Draws a HW accelerated triangle on the display

      @args x0[in]    The 0-based x location of point 0 on the triangle
      @args y0[in]    The 0-based y location of point 0 on the triangle
      @args x1[in]    The 0-based x location of point 1 on the triangle
      @args y1[in]    The 0-based y location of point 1 on the triangle
      @args x2[in]    The 0-based x location of point 2 on the triangle
      @args y2[in]    The 0-based y location of point 2 on the triangle
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  triangleHelper(x0, y0, x1, y1, x2, y2, color, 0);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled triangle on the display

      @args x0[in]    The 0-based x location of point 0 on the triangle
      @args y0[in]    The 0-based y location of point 0 on the triangle
      @args x1[in]    The 0-based x location of point 1 on the triangle
      @args y1[in]    The 0-based y location of point 1 on the triangle
      @args x2[in]    The 0-based x location of point 2 on the triangle
      @args y2[in]    The 0-based y location of point 2 on the triangle
      @args color[in] The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  triangleHelper(x0, y0, x1, y1, x2, y2, color, 1);
}

/**************************************************************************/
/*!
      Draws a HW accelerated ellipse on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args color[in]     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
  ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, 0);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled ellipse on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args color[in]     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
  ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, 0);
}

/**************************************************************************/
/*!
      Draws a HW accelerated curve on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args curvePart[in] The corner to draw, where in clock-wise motion:
                            0 = 180-270�
                            1 = 270-0�
                            2 = 0-90�
                            3 = 90-180�
      @args color[in]     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
  curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, 0);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled curve on the display

      @args xCenter[in]   The 0-based x location of the ellipse's center
      @args yCenter[in]   The 0-based y location of the ellipse's center
      @args longAxis[in]  The size in pixels of the ellipse's long axis
      @args shortAxis[in] The size in pixels of the ellipse's short axis
      @args curvePart[in] The corner to draw, where in clock-wise motion:
                            0 = 180-270�
                            1 = 270-0�
                            2 = 0-90�
                            3 = 90-180�
      @args color[in]     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
  curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, 1);
}

/**************************************************************************/
/*!
      Helper function for higher level circle drawing code
*/
/**************************************************************************/
void circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled)
{
  /* Set X */
  writeCommand(0x99);
  writeData(x0);
  writeCommand(0x9a);
  writeData(x0 >> 8);

  /* Set Y */
  writeCommand(0x9b);
  writeData(y0);
  writeCommand(0x9c);
  writeData(y0 >> 8);

  /* Set Radius */
  writeCommand(0x9d);
  writeData(r);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled)
  {
    writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
  }
  else
  {
    writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level rectangle drawing code
*/
/**************************************************************************/
void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled)
{
  /* Set X */
  writeCommand(0x91);
  writeData(x);
  writeCommand(0x92);
  writeData(x >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y);
  writeCommand(0x94);
  writeData(y >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(w);
  writeCommand(0x96);
  writeData((w) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(h);
  writeCommand(0x98);
  writeData((h) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled)
  {
    writeData(0xB0);
  }
  else
  {
    writeData(0x90);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level triangle drawing code
*/
/**************************************************************************/
void triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
  /* Set Point 0 */
  writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);
  writeCommand(0x93);
  writeData(y0);
  writeCommand(0x94);
  writeData(y0 >> 8);

  /* Set Point 1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData(x1 >> 8);
  writeCommand(0x97);
  writeData(y1);
  writeCommand(0x98);
  writeData(y1 >> 8);

  /* Set Point 2 */
  writeCommand(0xA9);
  writeData(x2);
  writeCommand(0xAA);
  writeData(x2 >> 8);
  writeCommand(0xAB);
  writeData(y2);
  writeCommand(0xAC);
  writeData(y2 >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled)
  {
    writeData(0xA1);
  }
  else
  {
    writeData(0x81);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level ellipse drawing code
*/
/**************************************************************************/
void ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled)
{
  /* Set Center Point */
  writeCommand(0xA5);
  writeData(xCenter);
  writeCommand(0xA6);
  writeData(xCenter >> 8);
  writeCommand(0xA7);
  writeData(yCenter);
  writeCommand(0xA8);
  writeData(yCenter >> 8);

  /* Set Long and Short Axis */
  writeCommand(0xA1);
  writeData(longAxis);
  writeCommand(0xA2);
  writeData(longAxis >> 8);
  writeCommand(0xA3);
  writeData(shortAxis);
  writeCommand(0xA4);
  writeData(shortAxis >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(0xA0);
  if (filled)
  {
    writeData(0xC0);
  }
  else
  {
    writeData(0x80);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level curve drawing code
*/
/**************************************************************************/
void curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled)
{
  /* Set Center Point */
  writeCommand(0xA5);
  writeData(xCenter);
  writeCommand(0xA6);
  writeData(xCenter >> 8);
  writeCommand(0xA7);
  writeData(yCenter);
  writeCommand(0xA8);
  writeData(yCenter >> 8);

  /* Set Long and Short Axis */
  writeCommand(0xA1);
  writeData(longAxis);
  writeCommand(0xA2);
  writeData(longAxis >> 8);
  writeCommand(0xA3);
  writeData(shortAxis);
  writeCommand(0xA4);
  writeData(shortAxis >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(0xA0);
  if (filled)
  {
    writeData(0xD0 | (curvePart & 0x03));
  }
  else
  {
    writeData(0x90 | (curvePart & 0x03));
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/************************* Mid Level ***********************************/

/**************************************************************************/
/*!

*/
/**************************************************************************/
void GPIOX(bool on) {
  if (on)
    writeReg(RA8875_GPIOX, 1);
  else
    writeReg(RA8875_GPIOX, 0);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void PWM1out(uint8_t p) {
  writeReg(RA8875_P1DCR, p);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void PWM2out(uint8_t p) {
  writeReg(RA8875_P2DCR, p);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void PWM1config(bool on, uint8_t clock) {
  if (on) {
    writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
  }
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void PWM2config(bool on, uint8_t clock) {
  if (on) {
    writeReg(RA8875_P2CR, RA8875_P2CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P2CR, RA8875_P2CR_DISABLE | (clock & 0xF));
  }
}

/**************************************************************************/
/*!
      Enables or disables the on-chip touch screen controller
*/
/**************************************************************************/
void touchEnable(bool on)
{
  if (on)
  {
    /* Enable Touch Panel (Reg 0x70) */
    writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE        |
                           RA8875_TPCR0_WAIT_4096CLK  |
                           RA8875_TPCR0_WAKEDISABLE   |
                           RA8875_TPCR0_ADCCLK_DIV4); // 10mhz max!
    /* Set Auto Mode      (Reg 0x71) */
    writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO    |
                           // RA8875_TPCR1_VREFEXT |
                           RA8875_TPCR1_DEBOUNCE);
    /* Enable TP INT */
    writeReg(RA8875_INTC1, readReg(RA8875_INTC1) | RA8875_INTC1_TP);
  }
  else
  {
    /* Disable TP INT */
    writeReg(RA8875_INTC1, readReg(RA8875_INTC1) & ~RA8875_INTC1_TP);
    /* Disable Touch Panel (Reg 0x70) */
    writeReg(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
  }
}

/**************************************************************************/
/*!
      Checks if a touch event has occured

      @returns  True is a touch event has occured (reading it via
                touchRead() will clear the interrupt in memory)
*/
/**************************************************************************/
bool touched(void)
{
  if (readReg(RA8875_INTC2) & RA8875_INTC2_TP) return 1;
  return false;
}

/**************************************************************************/
/*!
      Reads the last touch event

      @args x[out]  Pointer to the uint16_t field to assign the raw X value
      @args y[out]  Pointer to the uint16_t field to assign the raw Y value

      @note Calling this function will clear the touch panel interrupt on
            the RA8875, resetting the flag used by the 'touched' function
*/
/**************************************************************************/
bool touchRead(uint16_t *x, uint16_t *y)
{
  uint16_t tx, ty;
  uint8_t temp;

  tx = readReg(RA8875_TPXH);
  ty = readReg(RA8875_TPYH);
  temp = readReg(RA8875_TPXYL);
  tx <<= 2;
  ty <<= 2;
  tx |= temp & 0x03;        // get the bottom x bits
  ty |= (temp >> 2) & 0x03; // get the bottom y bits

  *x = tx;
  *y = ty;

  /* Clear TP INT Status */
  writeReg(RA8875_INTC2, RA8875_INTC2_TP);

  return true;
}

/**************************************************************************/
/*!
      Turns the display on or off
*/
/**************************************************************************/
void displayOn(bool on)
{
 if (on)
   writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
 else
   writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

/**************************************************************************/
/*!
    Puts the display in sleep mode, or disables sleep mode if enabled
*/
/**************************************************************************/
void sleep(bool sleep)
{
 if (sleep)
   writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
 else
   writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF);
}

/************************* Low Level ***********************************/

/**************************************************************************/
/*!

*/
/**************************************************************************/
void  writeReg(uint8_t reg, uint8_t val)
{
  writeCommand(reg);
  writeData(val);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
uint8_t  readReg(uint8_t reg)
{
  writeCommand(reg);
  return readData();
}

/**************************************************************************/
/*!
GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
SSIDataPut(ra8875.base_ssi, RA8875_DATAWRITE);
*/
/**************************************************************************/
void  writeData(uint8_t d)
{
	//digitalWrite(_cs, LOW);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);
	//SPI.transfer(RA8875_DATAWRITE);
	SSIDataPut(ra8875.base_ssi, RA8875_DATAWRITE);
	//SPI.transfer(d);
	SSIDataPut(ra8875.base_ssi, d);
	//digitalWrite(_cs, HIGH);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
uint8_t  readData(void)
{
	uint8_t x;
	//digitalWrite(_cs, LOW);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);
	//SPI.transfer(RA8875_DATAREAD);
	SSIDataPut(ra8875.base_ssi, RA8875_DATAREAD);
	//x = SPI.transfer(0x0);
	SSIDataGet(ra8875.base_ssi, (uint32_t *)&x);
	//digitalWrite(_cs, HIGH);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
	return x;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void  writeCommand(uint8_t d)
{
	//digitalWrite(_cs, LOW);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);
	//SPI.transfer(RA8875_CMDWRITE);
	SSIDataPut(ra8875.base_ssi, RA8875_CMDWRITE);
	//SPI.transfer(d);
	SSIDataPut(ra8875.base_ssi, d);
	//digitalWrite(_cs, HIGH);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
uint8_t  readStatus(void)
{
	uint8_t x;
	//digitalWrite(_cs, LOW);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0);
	//SPI.transfer(RA8875_CMDREAD);
	SSIDataPut(ra8875.base_ssi, RA8875_CMDREAD);
	//x = SPI.transfer(0x0);
	SSIDataGet(ra8875.base_ssi, (uint32_t *)&x);
	//digitalWrite(_cs, HIGH);
	GPIOPinWrite(ra8875.base_cs, ra8875.cs, 0xFF);
	return x;
}
