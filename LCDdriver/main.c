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
#include "comm.h"
#include "parameters.h"

int main()
{
	uint32_t tx, ty, tmp;
	uint16_t xScale, yScale;

	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	g_keyChange=0;
	g_attack=50;
	g_hold=50;
	g_release=50;
	g_filterFine=50;
	g_filterCourse=50;
	g_filterQ=50;
	g_volumeLevel=50;
	g_sld1Updated=0;
	g_sld2Updated=0;
	g_gridUpdated=0;
	g_filterType=LOW;
	g_waveType=SAW;
	g_changeLCD=0;
	g_stateLCD=AUDIO;
	g_backgroundColorOptions=BLUE_16BIT;

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



	//Configure timer0 for sending parameter updates for audio synthesis
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/500-1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	/*CODE FOR TOUCH SCREEN*/

	//wait 5 seconds to look at our pretty splash screen
	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

/*	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);

	SysCtlDelay(FIVEHUNDRED_MILISEC);
	SysCtlDelay(FIVEHUNDRED_MILISEC);
*/

	g_sld1=slider_get(10,35,g_volumeLabel,&g_volumeLevel,MICRO_VOLUME);
	g_sld2=slider_get(120,35,g_filterCourseLabel,&g_filterCourse,MICRO_FILTER_COURSE);//"Test 2");
	g_xy=xyGrid_get(230,35, g_filterFineLabel, &g_filterFine, MICRO_FILTER_FINE,
			g_filterQLabel, &g_filterQ, MICRO_FILTER_Q);
	g_txtKey=text_get(10,0,"Key: C", 75,15,WHITE_16BIT,g_backgroundColor);
	g_txtKeyType=text_get(75,0,"Type: Major", 100,15,WHITE_16BIT,g_backgroundColor);
	g_txtChord=text_get(175,0,"Chord: I", 95, 15, WHITE_16BIT, g_backgroundColor);
	g_txtWaveform=text_get(270,0,"Wave: Saw",90,15,WHITE_16BIT,g_backgroundColor);
	g_txtFilter=text_get(360, 0,"Filter: Low",100,15,WHITE_16BIT,g_backgroundColor);



	touchEnable(true);
	xScale=1024*10000/ra8875.width;
	yScale=1024*10000/ra8875.height;

	fillScreen(g_backgroundColor);
	text_draw(&g_txtKey);
	text_draw(&g_txtKeyType);
	text_draw(&g_txtChord);
	text_draw(&g_txtWaveform);
	text_draw(&g_txtFilter);
	slider_draw(&g_sld1);
	slider_draw(&g_sld2);
	xyGrid_draw(&g_xy);

	while(1)
	{

		if(g_changeLCD && !g_pChangeLCD)
		{
			if(g_stateLCD==OPTIONS)
			{
				fillScreen(g_backgroundColor);
				text_draw(&g_txtKey);
				text_draw(&g_txtKeyType);
				text_draw(&g_txtChord);
				text_draw(&g_txtWaveform);
				text_draw(&g_txtFilter);
				slider_draw(&g_sld1);
				slider_draw(&g_sld2);
				xyGrid_draw(&g_xy);
				g_stateLCD=AUDIO;
			}
			else
			{
				fillScreen(g_backgroundColorOptions);
				g_stateLCD=OPTIONS;
			}
		}
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

				if(g_stateLCD==AUDIO)
				{
					if(slider_isTouched(&g_sld1,tx,ty))
					{
						slider_updateSlideLevel(&g_sld1, ty);
						slider_draw(&g_sld1);
						g_sld1Updated=1;
					}
					else if(slider_isTouched(&g_sld2,tx,ty))
					{
						slider_updateSlideLevel(&g_sld2, ty);
						slider_draw(&g_sld2);
						g_sld2Updated=1;
					}
					else if(xyGrid_isTouched(&g_xy,tx,ty))
					{
						xyGrid_updateDotLevels(&g_xy,tx,ty);
						xyGrid_draw(&g_xy);
						g_gridUpdated=1;
					}
				}
			}
		}
		if(g_keyChange && g_stateLCD==AUDIO)
		{
			g_keyChange=0;
			keyTypeAppend();
			chordTextAppend();
			keyTextAppend();
			text_draw(&g_txtKey);
			text_draw(&g_txtKeyType);
			text_draw(&g_txtChord);
		}
		scanButtons();
	}

	return 0;
}

void Timer0IntHandler()
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	if(g_sld1Updated)
	{
		g_sld1Updated=0;
		SENDCOMMAND_MICRO(g_sld1.levelID,*(g_sld1.level)*MAXPARAMVAL/100);
	}

	if(g_sld2Updated)
	{
		g_sld2Updated=0;
		SENDCOMMAND_MICRO(g_sld2.levelID,*(g_sld2.level)*MAXPARAMVAL/100);
	}

	if(g_gridUpdated)
	{
		g_gridUpdated=0;
		SENDCOMMAND_MICRO(g_xy.xlevelID,*(g_xy.xlevel)*MAXPARAMVAL/100);
		SENDCOMMAND_MICRO(g_xy.ylevelID,*(g_xy.ylevel)*MAXPARAMVAL/100);
	}
}
