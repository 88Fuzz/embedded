#define PART_TM4C123GH6PM
//http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/p/304497/1061344.aspx

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "util.h"
#include "lcd.h"
#include "ra8875.h"
#include "noteGen.h"

//#define SSI0_PCBCOM
//#define MIDITEST
//#define LEDBLINK
#define SSI1_PCB_BUTTONS
//#define PCB_LCD

volatile uint32_t butt;

//deleteTHIS deleteTmp[7];

int main()
{
	uint32_t tx, ty, tmp;
	uint16_t xScale, yScale;
	slider sl1, sl2;//, sl3, sl4;
	xyGrid xy;

	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	butt=0;
	g_updateKeyInfo=0;

#ifdef SSI0_PCBCOM
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, SysCtlClockGet()/3, 16);
	SSIEnable(SSI0_BASE);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	//tmp=SysCtlClockGet();
	//ui32Period=SysCtlClockGet()/160;

	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/48000-1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

  while(1);

#elif defined MIDITEST
 /*   SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 31250,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));*/


  	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  	GPIOPinConfigure(GPIO_PB0_U1RX); //B0 receptor
  	GPIOPinConfigure(GPIO_PB1_U1TX); //B1 transmitter
  	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 31250,
  			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE) );


    while (1)
    {
 /*   	UARTCharPut(UART1_BASE,~(0x01));
    	UARTCharPut(UART1_BASE,~(0x02));
    	UARTCharPut(UART1_BASE,~(0x04));
    	UARTCharPut(UART1_BASE,0x08);
    	UARTCharPut(UART1_BASE,0x0A);
    	UARTCharPut(UART1_BASE,0x10);
    	UARTCharPut(UART1_BASE,0x20);
    	UARTCharPut(UART1_BASE,0x40);
    	UARTCharPut(UART1_BASE,0x80);
    	UARTCharPut(UART1_BASE,0xA0);
//    	UARTCharPut(UART1_BASE,0xAA);
  //  	SysCtlDelay(ONEHUNDRED_MILISEC);
*/    	UARTCharPutNonBlocking(UART1_BASE, 0x90);
		UARTCharPutNonBlocking(UART1_BASE, 0x69);
		UARTCharPutNonBlocking(UART1_BASE, 0xFF);

    	SysCtlDelay(FIVEHUNDRED_MILISEC);
    	SysCtlDelay(FIVEHUNDRED_MILISEC);

    	UARTCharPutNonBlocking(UART1_BASE, 0x80);
    	UARTCharPutNonBlocking(UART1_BASE, 0x69);
    	UARTCharPutNonBlocking(UART1_BASE, 0xFF);

    	SysCtlDelay(FIVEHUNDRED_MILISEC);
    	SysCtlDelay(FIVEHUNDRED_MILISEC);
    }



#elif defined LEDBLINK
						/*CODE FOR 1Hz blinking LED*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

//	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

	//tmp=SysCtlClockGet();
	//ui32Period=SysCtlClockGet()/160;

	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()-1);
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()/2-1);

	IntEnable(INT_TIMER0A);
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
	TimerEnable(TIMER1_BASE, TIMER_A);

	while(1);


#elif defined SSI1_PCB_BUTTONS

				/*CODE TO GET SSI1 WORKING*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1/*SYSCTL_PERIPH_SSI0*/);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF/*SYSCTL_PERIPH_GPIOA*/);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	//configure SPI communication for button scaning

	//GPIOPinConfigure(GPIO_PF1_SSI1TX);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
	GPIOPinConfigure(GPIO_PF3_SSI1FSS);
	GPIOPinConfigure(GPIO_PF0_SSI1RX);
	GPIOPinTypeSSI(GPIO_PORTF_BASE,
			GPIO_PIN_3|GPIO_PIN_2/*|GPIO_PIN_1*/|GPIO_PIN_0);

	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(),
			SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000000, 16);
	SSIEnable(SSI1_BASE);


	//configure SPI communication between micros
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, SysCtlClockGet()/10, 16);
	SSIEnable(SSI0_BASE);

	//Configure error LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);


	//enable UART for MIDI send/receive
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA1_U0TX); //B1 transmitter
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 31250, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE) );

	//initialize the buttons yo!
	initButtons();
	genScale();
	chordSelect();


	//initialize LCD SSI

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	//chip select
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_1);

	//reset pin
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4);

	//Touch int-ish
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_5);

	GPIOPinConfigure(GPIO_PD3_SSI3TX);
	GPIOPinConfigure(GPIO_PD0_SSI3CLK);
	//	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PD2_SSI3RX);
	GPIOPinTypeSSI(GPIO_PORTD_BASE,GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_2);
	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
	SSI_MODE_MASTER, 125000, 8);
	SSIEnable(SSI3_BASE);

	ra8875init(
	/*base_cs*/GPIO_PORTD_BASE,
	/*base_rst*/GPIO_PORTB_BASE,
	/*SSICS_pin*/GPIO_PIN_1,
	/*cs_pin*/GPIO_PIN_1,
	/*rst_pin*/GPIO_PIN_4,
	/*base_ssi*/SSI3_BASE,
	/*SSICLK_pin*/GPIO_PIN_0,
	/*SSIRX_pin*/GPIO_PIN_2,
	/*SSITX_pin*/GPIO_PIN_3,
	/*base_int*/GPIO_PORTE_BASE,
	/*touch_int*/GPIO_PIN_5,
	/*width*/480,
	/*height*/272);

	if(!begin())
	{
		while(1)
		{
			//SHIT BE BROKE
			SysCtlDelay(ONE_MILISEC);
		}
	}

	SSIDisable(SSI3_BASE);
	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
	SSI_MODE_MASTER, 4000000, 8);
	SSIEnable(SSI3_BASE);


	displayOn(true);
	GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	PWM1config(true, RA8875_PWM_CLK_DIV1024);
	SysCtlDelay(TEN_MILISEC);
	PWM1out(255);

	//Draw logo while everything else loads. YO
	fillScreen(BLACK_16BIT);
	//u
	fillRect5(0,26,2,13,WHITE_16BIT);
	fillRect5(0,37,0xf,2,WHITE_16BIT);
	fillRect5(0xd,26,2,21,WHITE_16BIT);
	//S
	fillRect5(0x10,13,0xf,2,WHITE_16BIT);
	fillRect5(0x10,13,2,13,WHITE_16BIT);
	fillRect5(0x10,24,0xf,2,WHITE_16BIT);
	fillRect5(0x1d,24,2,15,WHITE_16BIT);
	fillRect5(0x10,37,0xf,2,WHITE_16BIT);
	//y
	fillRect5(0x20,26,2,13,WHITE_16BIT);
	fillRect5(0x20,37,0xf,2,WHITE_16BIT);
	fillRect5(0x2d,26,2,26,WHITE_16BIT);
	fillRect5(0x20,50,0xf,2,WHITE_16BIT);
	//n
	fillRect5(0x30,24,2,15,WHITE_16BIT);
	fillRect5(0x30,26,0xf,2,WHITE_16BIT);
	fillRect5(0x3d,26,2,13,WHITE_16BIT);
	//t
	fillRect5(0x40,24,0xf,2,WHITE_16BIT);
	fillRect5(0x46,13,2,26,WHITE_16BIT);
	fillRect5(0x46,37,9,2,WHITE_16BIT);
	//h
	fillRect5(0x50,13,2,26,WHITE_16BIT);
	fillRect5(0x50,24,0xf,2,WHITE_16BIT);
	fillRect5(0x5d,24,2,15,WHITE_16BIT);



	//Configure timer0 for button scanning
	//Configure timer1 for sending parameter updates for audio synthesis
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

//	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

//	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/50000-1);
//	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()/24000-1);

//	IntEnable(INT_TIMER0A);
//	IntEnable(INT_TIMER1A);
//	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
//	TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
//	IntMasterEnable();

//	TimerEnable(TIMER0_BASE, TIMER_A);
//	TimerEnable(TIMER1_BASE, TIMER_A);

	/*CODE FOR TOUCH SCREEN*/

	//wait 5 seconds to look at our pretty splash screen
	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	fillScreen(g_backgroundColor);

	sl1=slider_get(10,35,"Test 1");
	sl2=slider_get(120,35,"Test 2");
	xy=xyGrid_get(230,35, "x val", "y val");
	g_txtKey=text_get(25,0,"Key: C", 50,15,WHITE_16BIT,g_backgroundColor);
	g_txtKeyType=text_get(125,0,"Type: Major", 50,15,WHITE_16BIT,g_backgroundColor);
	g_txtChord=text_get(225,0,"Chord: I", 50, 15, WHITE_16BIT, g_backgroundColor);

	touchEnable(true);
	xScale=1024*10000/ra8875.width;
	yScale=1024*10000/ra8875.height;

	text_draw(&g_txtKey);
	text_draw(&g_txtKeyType);
	text_draw(&g_txtChord);
	slider_draw(&sl1);
	slider_draw(&sl2);
	xyGrid_draw(&xy);

	while(1)
	{
		// Wait around for touch events
		if (!GPIOPinRead(ra8875.base_rst, ra8875.touch_int))//digitalRead(RA8875_INT))
		{
			if (touched())
			{
				touchRead(&tx, &ty);
				if(tx<X_TOUCH_MID)//roughly in the middle
				{
					tmp=tx*10000;
					tmp*=25;
					tmp/=X_TOUCH_MID;
					tmp/=10000;
					tx+=tmp-25;
				}
				else
				{
					tmp=tx*10000;
					tmp*=12;
					tmp/=X_TOUCH_MAX;
					tmp/=10000;
					tx+=tmp;
				}

				if(ty<Y_TOUCH_MID)//maybe do some kind of offset where the closer the point is to the edge, the greater the offset?
				{
					tmp=ty*10000;
					tmp*=100;
					tmp/=Y_TOUCH_MID;
					tmp/=10000;
					ty+=tmp-100;
				}
				else
				{
					tmp=ty*10000;
					tmp*=50;					//TODO: THIS LINE COULD BE ADJUSTED A BIT MORE
					tmp/=Y_TOUCH_MAX;
					tmp/=10000;
					ty+=tmp;
				}
				tx*=10000;
				ty*=10000;
				tx/=xScale;
				ty/=yScale;

				if(slider_isTouched(&sl1,tx,ty))
				{
					slider_updateSlideLevel(&sl1, ty);
					slider_draw(&sl1);
				}
				else if(slider_isTouched(&sl2,tx,ty))
				{
					slider_updateSlideLevel(&sl2, ty);
					slider_draw(&sl2);
				}
				else if(xyGrid_isTouched(&xy,tx,ty))
				{
					xyGrid_updateDotLevels(&xy,tx,ty);
					xyGrid_draw(&xy);
				}
			}
		}
		if(g_updateKeyInfo)
		{
			g_updateKeyInfo=0;
			keyTypeAppend();
			chordTextAppend();
			keyTextAppend();
			text_draw(&g_txtKey);
			text_draw(&g_txtKeyType);
			text_draw(&g_txtChord);
		}
		scanButtons();
	}

#else
					/*CODE FOR TOUCH SCREEN*/

	sl1=slider_get(10,35,"Test 1");
	sl2=slider_get(120,35,"Test 2");
	xy=xyGrid_get(230,35, "x val", "y val");
	txtScale=text_get(50,0,"Scale: C#", 50,15,WHITE_16BIT,g_backgroundColor);
	txtScaleType=text_get(150,0,"Type: minor", 50,15,WHITE_16BIT,g_backgroundColor);

	//sl3=slider_get(230,20,"Test 3");
	//sl4=slider_get(340,20,"Test 4");

#ifdef PCB_LCD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	//chip select
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_1);

	//reset pin
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4);

	//Touch int-ish
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_5);

	GPIOPinConfigure(GPIO_PD3_SSI3TX);
	GPIOPinConfigure(GPIO_PD0_SSI3CLK);
//	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PD2_SSI3RX);
	GPIOPinTypeSSI(GPIO_PORTD_BASE,GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_2);
	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, 125000, 8);
	SSIEnable(SSI3_BASE);

	ra8875init(
//		/*GPIOcfg*/SYSCTL_PERIPH_GPIOA,
		/*base_cs*/GPIO_PORTD_BASE,
		/*base_rst*/GPIO_PORTB_BASE,
//		/*SSIcfg*/SYSCTL_PERIPH_SSI0,
//		/*SSIGPIOcfg*/SYSCTL_PERIPH_GPIOA,
//		/*SSIcfg_clk*/GPIO_PA2_SSI0CLK,
//		/*SSIcfg_rx*/GPIO_PA4_SSI0RX,
//		/*SSIcfg_tx*/GPIO_PA5_SSI0TX,
//		/*SSIcfg_cs*/GPIO_PA3_SSI0FSS,
		/*SSICS_pin*/GPIO_PIN_1,
		/*cs_pin*/GPIO_PIN_1,
		/*rst_pin*/GPIO_PIN_4,
		/*base_ssi*/SSI3_BASE,
		/*SSICLK_pin*/GPIO_PIN_0,
		/*SSIRX_pin*/GPIO_PIN_2,
		/*SSITX_pin*/GPIO_PIN_3,
		/*base_int*/GPIO_PORTE_BASE,
		/*touch_int*/GPIO_PIN_5,
		/*width*/480,
		/*height*/272);

	if(!begin())
	{
		while(1)
		{
			//SHIT BE BROKE
			SysCtlDelay(ONE_MILISEC);
		}
	}

	SSIDisable(SSI3_BASE);
	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, 4000000, 8);
	SSIEnable(SSI3_BASE);

#else
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

	shit will break horribly if you dont fix anything
#endif


	displayOn(true);
	GPIOX(true);      // Enable TFT - display enable tied to GPIOX
//	this line breaks things bad
	PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight   //MAYBE THIS BREAKS SHIT
	SysCtlDelay(TEN_MILISEC);
	PWM1out(255);

	// With hardware accelleration this is instant
	fillScreen(WHITE_16BIT);

	//LOGO
	fillScreen(BLACK_16BIT);
	//u
	fillRect5(0,26,2,13,WHITE_16BIT);
	fillRect5(0,37,0xf,2,WHITE_16BIT);
	fillRect5(0xd,26,2,21,WHITE_16BIT);
	//S
	fillRect5(0x10,13,0xf,2,WHITE_16BIT);
	fillRect5(0x10,13,2,13,WHITE_16BIT);
	fillRect5(0x10,24,0xf,2,WHITE_16BIT);
	fillRect5(0x1d,24,2,15,WHITE_16BIT);
	fillRect5(0x10,37,0xf,2,WHITE_16BIT);
	//y
	fillRect5(0x20,26,2,13,WHITE_16BIT);
	fillRect5(0x20,37,0xf,2,WHITE_16BIT);
	fillRect5(0x2d,26,2,26,WHITE_16BIT);
	fillRect5(0x20,50,0xf,2,WHITE_16BIT);
	//n
	fillRect5(0x30,24,2,15,WHITE_16BIT);
	fillRect5(0x30,26,0xf,2,WHITE_16BIT);
	fillRect5(0x3d,26,2,13,WHITE_16BIT);
	//t
	fillRect5(0x40,24,0xf,2,WHITE_16BIT);
	fillRect5(0x46,13,2,26,WHITE_16BIT);
	fillRect5(0x46,37,9,2,WHITE_16BIT);
	//h
	fillRect5(0x50,13,2,26,WHITE_16BIT);
	fillRect5(0x50,24,0xf,2,WHITE_16BIT);
	fillRect5(0x5d,24,2,15,WHITE_16BIT);


	//text test
	fillScreen(BLACK_16BIT);

	textMode();
	textSetCursor(10,10);

	textTransparent(WHITE_16BIT);
	textWrite(string);
	textColor(WHITE_16BIT, RED_16BIT);
	textWrite(string);
	textTransparent(CYAN_16BIT);
	textWrite(string);
	textTransparent(GREEN_16BIT);
	textWrite(string);
	textColor(YELLOW_16BIT, CYAN_16BIT);
	textWrite(string);
	textColor(BLACK_16BIT, MAGENTA_16BIT);
	textWrite(string);

	//Change the cursor location and color
	textSetCursor(100, 100);
	textTransparent(WHITE_16BIT);
	//If necessary, enlarge the font
	textEnlarge(1);
	//and render some more text!
	textWrite(string);
	textSetCursor(100, 150);
	textEnlarge(2);
	textWrite(string);
	textEnlarge(0);

	graphicsMode();

	fillScreen(RED_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(YELLOW_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(GREEN_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(CYAN_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(MAGENTA_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillScreen(MAGENTA_16BIT);
	SysCtlDelay(TEN_MILISEC);

	  // Try some GFX acceleration!
	drawCircle(100, 100, 50, BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillCircle(100, 100, 49, GREEN_16BIT);
	SysCtlDelay(TEN_MILISEC);

	fillRect(11, 11, 398, 198, BLUE_16BIT);
	SysCtlDelay(TEN_MILISEC);
	drawRect(10, 10, 400, 200, GREEN_16BIT);
	SysCtlDelay(TEN_MILISEC);
	drawPixel(10,10,BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	drawPixel(11,11,BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	drawLine(10, 10, 200, 100, RED_16BIT);
	SysCtlDelay(TEN_MILISEC);
	drawTriangle(200, 15, 250, 100, 150, 125, BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillTriangle(200, 16, 249, 99, 151, 124, YELLOW_16BIT);
	SysCtlDelay(TEN_MILISEC);
	drawEllipse(300, 100, 100, 40, BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillEllipse(300, 100, 98, 38, GREEN_16BIT);
	SysCtlDelay(TEN_MILISEC);
	  // Argument 5 (curvePart) is a 2-bit value to control each corner (select 0, 1, 2, or 3)
	drawCurve(50, 100, 80, 40, 2, BLACK_16BIT);
	SysCtlDelay(TEN_MILISEC);
	fillCurve(50, 100, 78, 38, 2, WHITE_16BIT);
	SysCtlDelay(TEN_MILISEC);

	fillScreen(g_backgroundColor);
	SysCtlDelay(TEN_MILISEC);

	//pinMode(RA8875_INT, INPUT);
	//digitalWrite(RA8875_INT, HIGH);

	touchEnable(true);
	xScale=1024*10000/ra8875.width;
	yScale=1024*10000/ra8875.height;

	text_draw(&txtScale);
	text_draw(&txtScaleType);
	slider_draw(&sl1);
	slider_draw(&sl2);
	xyGrid_draw(&xy);
	//slider_draw(&sl3);
	//slider_draw(&sl4);
	while(1)
	{
		// Wait around for touch events
		if (!GPIOPinRead(ra8875.base_rst, ra8875.touch_int))//digitalRead(RA8875_INT))
		{
			if (touched())
			{
				touchRead(&tx, &ty);
				if(tx<X_TOUCH_MID)//roughly in the middle
				{
					tmp=tx*10000;
					tmp*=25;
					tmp/=X_TOUCH_MID;
					tmp/=10000;
					tx+=tmp-25;
				}
				else
				{
					tmp=tx*10000;
					tmp*=12;
					tmp/=X_TOUCH_MAX;
					tmp/=10000;
					tx+=tmp;
				}

				if(ty<Y_TOUCH_MID)//maybe do some kind of offset where the closer the point is to the edge, the greater the offset?
				{
					tmp=ty*10000;
					tmp*=100;
					tmp/=Y_TOUCH_MID;
					tmp/=10000;
					ty+=tmp-100;
				}
				else
				{
					tmp=ty*10000;
					tmp*=50;					//TODO: THIS LINE COULD BE ADJUSTED A BIT MORE
					tmp/=Y_TOUCH_MAX;
					tmp/=10000;
					ty+=tmp;
				}
				tx*=10000;
				ty*=10000;
				tx/=xScale;
				ty/=yScale;

				if(slider_isTouched(&sl1,tx,ty))
				{
					slider_updateSlideLevel(&sl1, ty);
					slider_draw(&sl1);
				}
				else if(slider_isTouched(&sl2,tx,ty))
				{
					slider_updateSlideLevel(&sl2, ty);
					slider_draw(&sl2);
				}
				else if(xyGrid_isTouched(&xy,tx,ty))
				{
					xyGrid_updateDotLevels(&xy,tx,ty);
					xyGrid_draw(&xy);
				}
				/*else if(slider_isTouched(&sl3,tx,ty))
				{
					slider_updateSlideLevel(&sl3, ty);
					slider_draw(&sl3);
				}
				else if(slider_isTouched(&sl4,tx,ty))
				{
					slider_updateSlideLevel(&sl4, ty);
					slider_draw(&sl4);
				}*/
/*				else
				{
					//Draw a circle
					fillCircle((uint16_t)(tx), (uint16_t)(ty), 4, WHITE_16BIT);
				}
*/			}
		}
	}
#endif
	return 0;
}

void Timer0IntHandler()
{
//	uint32_t tmp;
//	uint16_t scanData, shiftData;
//	uint8_t j,k,keyChange=0,acciOff,baseAcciOff,baseOctOff,octOff;
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//	butt=1;
//#ifdef SSI0_PCBCOM
//	SSIDataPut(SSI0_BASE, 0xAF0A);
//#elif defined SSI1_PCB_BUTTONS
//
//	flushSSIFIFO(SSI1_BASE);
//	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
//	SysCtlDelay(250);
//	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
//	SysCtlDelay(250);
//
//	//load 7 sets of 16 buttons
//	for(j=0;j<7;j++)
//	{
//		SSIDataPut(SSI1_BASE,0);//load 16 bits of data
//		SysCtlDelay(250);
//	}
//
//	//should run 7 times. We will have problems if it doesn't
//	for(j=0;j<7;j++)
//	{
//		//turn on led if not scanning all the buttons
//		if((SSIDataGetNonBlocking(SSI1_BASE,&tmp))==0)
//			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF);
//		else
//			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
//
//		scanData=(uint32_t)tmp;
//
//		if(j==0)
//		{
//			//loop through all 16 bits of scanned data
//			for(k=0;k<16;k++)
//			{
//				shiftData=15-k;
//				shiftData=0x01<<shiftData;
//				shiftData=(scanData & shiftData);
//				//if k is in the top 8 bits, it is a chord select button
//				if(k<8)
//				{
//					if(shiftData)
//					{
//						if(k==0)
//							g_chord=FIRST;
//						else if(k==1)
//							g_chord=SECOND;
//						else if(k==2)
//							g_chord=THIRD;
//						else if(k==3)
//							g_chord=FOURTH;
//						else if(k==4)
//							g_chord=FIFTH;
//						else if(k==5)
//							g_chord=SIXTH;
//						else if(k==6)
//							g_chord=SEVENTH;
//						//set the flag that says we need to re-generate our chord/key
//						keyChange=1;
//					}
//				}
//				else
//				{
//					if(shiftData)
//					{
//						if(k==8)
//							g_key=Cs;
//						else if(k==9)
//							g_key=Ds;
//						else if(k==10)
//							g_keyType=MINOR;
//						else if(k==11)
//							g_keyType=MAJOR;
//						else if(k==12)
//							g_key=As;
//						else if(k==13)
//							g_key=Fs;
//						else if(k==14)
//							g_key=Gs;
//						keyChange=1;
//					}
//				}
//			}
//		}
//		else if(j==1)
//		{
//			baseAcciOff=28;
//			for(k=0;k<16;k++)
//			{
//				shiftData=15-k;
//				shiftData=0x01<<shiftData;
//				shiftData=(scanData & shiftData);
//
//				//if we are now scanning note buttons, change scale/octaves if needed
//				if(k==8 && keyChange)
//				{
//					sendAllNotesOff();
//					genScale();
//					chordSelect();
//				}
//
//				if(k<8)
//				{
//					if(shiftData)
//					{
//						if(k==0)
//							g_key=C;
//						else if(k==1)
//							g_key=D;
//						else if(k==2)
//							g_key=E;
//						else if(k==3)
//							g_key=F;
//						else if(k==4)
//							g_key=G;
//						else if(k==5)
//							g_key=A;
//						else if(k==6)
//							g_key=B;
//						keyChange=1;
//					}
//				}
//				else
//				{
//					acciOff=baseAcciOff+(k-8);
//					if(shiftData)
//					{
//						if(g_octavesAcci[acciOff].state==OFF &&
//								g_octavesAcci[acciOff].midi>0)
//						{
//							g_octavesAcci[acciOff].state=ON;
//							SENDNOTEON_ALL(g_octavesAcci[acciOff].midi,0x7F);
//						}
//					}
//					else
//					{
//						if(g_octavesAcci[acciOff].state==ON &&
//								g_octavesAcci[acciOff].midi>0)
//						{
//							g_octavesAcci[acciOff].state=OFF;
//							SENDNOTEOFF_ALL(g_octavesAcci[acciOff].midi,0x7F);
//						}
//					}
//				}
//			}
//		}
//		else
//		{
//			//I could figure out math to do this, but I don't feel like it
//			if(j==2)
//			{
//				baseOctOff=32;
//				baseAcciOff=21;
//			}
//			else if(j==3)
//			{
//				baseOctOff=24;
//				baseAcciOff=14;
//			}
//			else if(j==4)
//			{
//				baseOctOff=16;
//				baseAcciOff=7;
//			}
//			else if(j==5)
//			{
//				baseOctOff=8;
//				baseAcciOff=0;
//			}
//			else
//			{
//				baseOctOff=0;
//				baseAcciOff=0;
//			}
//
//			for(k=0;k<16;k++)
//			{
//				shiftData=15-k;
//				shiftData=0x01<<shiftData;
//				shiftData=(scanData & shiftData);
//
//				if(k<8)
//				{
//					octOff=baseOctOff+k;
//					if(shiftData)
//					{
//						if(g_octaves[octOff].state==OFF)
//						{
//							g_octaves[octOff].state=ON;
//							SENDNOTEON_ALL(g_octaves[octOff].midi,0x7F);
//						}
//					}
//					else
//					{
//						if(g_octaves[octOff].state==ON)
//						{
//							g_octaves[octOff].state=OFF;
//							SENDNOTEOFF_ALL(g_octaves[octOff].midi,0x7F);
//						}
//					}
//				}
//				else if(j!=6 && k!=15)
//				{
//					acciOff=baseAcciOff+(k-8);
//					if(shiftData)
//					{
//						if(g_octavesAcci[acciOff].state==OFF &&
//								g_octavesAcci[acciOff].midi>0)
//						{
//							g_octavesAcci[acciOff].state=ON;
//							SENDNOTEON_ALL(g_octavesAcci[acciOff].midi,0x7F);
//						}
//					}
//					else
//					{
//						if(g_octavesAcci[acciOff].state==ON &&
//								g_octavesAcci[acciOff].midi>0)
//						{
//							g_octavesAcci[acciOff].state=OFF;
//							SENDNOTEOFF_ALL(g_octavesAcci[acciOff].midi,0x7F);
//						}
//					}
//				}
//			}
//		}
//	}
//	butt=0;
//#else
//	if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
//		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
//	else
//	    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF);
//#endif
}

void Timer1IntHandler()
{
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
/*	if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);
	else
	    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0xFF);
*/
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
