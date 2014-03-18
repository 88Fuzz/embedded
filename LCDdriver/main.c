#define PART_TM4C123GH6PM
//http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/p/304497/1061344.aspx

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "myFuncs.h"
#include "ra8875.h"


int main()
{
	char string[15]="Hello World!";
	uint32_t tx, ty, tmp;
	uint16_t xScale, yScale, i;
	/*void ra8875init(uint32_t GPIOcfg,//SYSCTL_PERIPH_GPIOC
		uint32_t base_cs, uint32_t base_rst, uint32_t SSIcfg,
		uint32_t SSIGPIOcfg, uint32_t SSIcfg_clk, uint32_t SSIcfg_rx,
		uint32_t SSIcfg_tx, uint32_t SSIcfg_cs, uint32_t SSICS_pin,
		uint32_t cs_pin, uint32_t rst_pin, uint32_t base_ssi,
		uint32_t SSICLK_pin, uint32_t SSIRX_pin, uint32_t SSITX_pin,
		uint32_t base_int, uint32_t touch_int,
		uint16_t width, uint16_t height)*/

	ra8875init(
			/*GPIOcfg*/SYSCTL_PERIPH_GPIOA,
			/*base_cs*/GPIO_PORTA_BASE,
			/*base_rst*/GPIO_PORTA_BASE,
			/*SSIcfg*/SYSCTL_PERIPH_SSI0,
			/*SSIGPIOcfg*/SYSCTL_PERIPH_GPIOA,
			/*SSIcfg_clk*/GPIO_PA2_SSI0CLK,
			/*SSIcfg_rx*/GPIO_PA4_SSI0RX,
			/*SSIcfg_tx*/GPIO_PA5_SSI0TX,
			/*SSIcfg_cs*/GPIO_PA3_SSI0FSS,
			/*SSICS_pin*/GPIO_PIN_3,
			/*cs_pin*/GPIO_PIN_3,
			/*rst_pin*/GPIO_PIN_6,
			/*base_ssi*/SSI0_BASE,
			/*SSICLK_pin*/GPIO_PIN_2,
			/*SSIRX_pin*/GPIO_PIN_4,
			/*SSITX_pin*/GPIO_PIN_5,
			/*base_int*/GPIO_PORTA_BASE,
			/*touch_int*/GPIO_PIN_7,
			/*width*/480,
			/*height*/272);

	  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
	if(!begin())
	{
		while(1)
		{
			//SHIT BE BROKE
			SysCtlDelay(ONE_MILISEC);
		}
	}

	displayOn(true);
	GPIOX(true);      // Enable TFT - display enable tied to GPIOX
//	this line breaks things bad
	PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight   //MAYBE THIS BREAKS SHIT
//	PWM1out(255);

	// With hardware accelleration this is instant
	fillScreen(RA8875_WHITE);

	// Play with PWM
//	for (i=255; i!=0; i-=5 )
//	{
//		PWM1out(i);
//		SysCtlDelay(TEN_MILISEC);
//	}

//	for (i=0; i!=255; i+=5 )
//	{
//	    PWM1out(i);
//	    SysCtlDelay(TEN_MILISEC);
//	}
	PWM1out(0);

	//text test
	fillScreen(RA8875_BLACK);

	textMode();
	textSetCursor(10,10);

	textTransparent(RA8875_WHITE);
	textWrite(string,15);
	textColor(RA8875_WHITE, RA8875_RED);
	textWrite(string,15);
	textTransparent(RA8875_CYAN);
	textWrite(string,15);
	textTransparent(RA8875_GREEN);
	textWrite(string,15);
	textColor(RA8875_YELLOW, RA8875_CYAN);
	textWrite(string,15);
	textColor(RA8875_BLACK, RA8875_MAGENTA);
	textWrite(string,15);

	//Change the cursor location and color
	textSetCursor(100, 100);
	textTransparent(RA8875_WHITE);
	//If necessary, enlarge the font
	textEnlarge(1);
	//and render some more text!
	textWrite(string,15);
	textSetCursor(100, 150);
	textEnlarge(2);
	textWrite(string,15);

	graphicsMode();

	fillScreen(RA8875_RED);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(RA8875_YELLOW);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(RA8875_GREEN);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(RA8875_CYAN);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(RA8875_MAGENTA);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(RA8875_MAGENTA);
	SysCtlDelay(TEN_MILISEC);

	  // Try some GFX acceleration!
	drawCircle(100, 100, 50, RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	fillCircle(100, 100, 49, RA8875_GREEN);
	SysCtlDelay(TEN_MILISEC);

	fillRect(11, 11, 398, 198, RA8875_BLUE);
	SysCtlDelay(TEN_MILISEC);
	drawRect(10, 10, 400, 200, RA8875_GREEN);
	SysCtlDelay(TEN_MILISEC);
	drawPixel(10,10,RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	drawPixel(11,11,RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	drawLine(10, 10, 200, 100, RA8875_RED);
	SysCtlDelay(TEN_MILISEC);
	drawTriangle(200, 15, 250, 100, 150, 125, RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	fillTriangle(200, 16, 249, 99, 151, 124, RA8875_YELLOW);
	SysCtlDelay(TEN_MILISEC);
	drawEllipse(300, 100, 100, 40, RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	fillEllipse(300, 100, 98, 38, RA8875_GREEN);
	SysCtlDelay(TEN_MILISEC);
	  // Argument 5 (curvePart) is a 2-bit value to control each corner (select 0, 1, 2, or 3)
	drawCurve(50, 100, 80, 40, 2, RA8875_BLACK);
	SysCtlDelay(TEN_MILISEC);
	fillCurve(50, 100, 78, 38, 2, RA8875_WHITE);
	SysCtlDelay(TEN_MILISEC);

	fillScreen(ORANGE);
	SysCtlDelay(TEN_MILISEC);

	//pinMode(RA8875_INT, INPUT);
	//digitalWrite(RA8875_INT, HIGH);

	touchEnable(true);
	xScale=1024*10000/ra8875.width;
	yScale=1024*10000/ra8875.height;

	while(1)
	{
		// Wait around for touch events
		if (!GPIOPinRead(ra8875.base_rst, ra8875.touch_int))//digitalRead(RA8875_INT))
		{
			if (touched())
			{
				for(i=50;i<100;i++)
				drawPixel(i,50, RA8875_BLACK);

				touchRead(&tx, &ty);
				if(tx<X_TOUCH_MID)//roughly in the middle
				{
					tmp=tx*10000;
					tmp*=50/X_TOUCH_MID/10000;
					tx+=tmp-50;
				}

				else
				{
					tmp=tx*10000;
					tmp*=25/X_TOUCH_MAX/10000;
					tx+=tmp;
				}

				if(ty<Y_TOUCH_MID)//maybe do some kind of offset where the closer the point is to the edge, the greater the offset?
				{
					tmp=ty*10000;
					tmp*=100/Y_TOUCH_MID/10000;
					ty+=tmp-100;
				}
				else
				{
					tmp=ty*10000;
					tmp*=100/Y_TOUCH_MAX/10000;
					ty+=tmp;

				}
		//		tx-=50;
		//		ty-=100;
				tx*=10000;
				ty*=10000;
				tx/=xScale;
				ty/=yScale;
				// Draw a circle
				fillCircle((uint16_t)(tx), (uint16_t)(ty), 4, RA8875_WHITE);
			}
		}
	}

	return 0;
}


/*
//8 push button SSI code

int main()
{
	uint32_t tmp, ret;
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	//GPIOPinConfigure(GPIO_PF2_SSI1CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	//GPIOPinConfigure(GPIO_PF3_SSI1FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	//GPIOPinConfigure(GPIO_PF0_SSI1RX);
//	GPIOPinConfigure(GPIO_PF1_SSI1TX);
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_2);
	//GPIOPinTypeSSI(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_5);
	//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000, 8);
	SSIEnable(SSI0_BASE);

	//SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000, 8);
	//SSIEnable(SSI1_BASE);

	//flush RX fifo
	do
	{
		ret=SSIDataGetNonBlocking(SSI0_BASE,&tmp);
	}while(ret!=0);

	while(1)
	{
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
		SysCtlDelay(500);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0xFF);
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
		SysCtlDelay(600);

		SSIDataPut(SSI0_BASE,0);//load data
		//ret=SSIDataGetNonBlocking(SSI1_BASE, &tmp);
		SSIDataGet(SSI0_BASE,&tmp);
		//if(ret!=0)
//		SSIDisable(SSI0_BASE);
			SysCtlDelay(500);
//		SSIEnable(SSI0_BASE);
		//else
			SysCtlDelay(500);

		//SSIDataPut(SSI0_BASE,0xFF);
		//SSIDataGet(SSI0_BASE,&tmp);
		//SSIDataGet(SSI1_BASE,&tmp);
		SysCtlDelay(500);
	}

	return 0;
}
*/
