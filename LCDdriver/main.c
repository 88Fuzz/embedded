#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "ra8875.h"

uint16_t tx, ty;

int main() 
{
  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */

	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

  initRA8875pins(GPIO_PORTC_BASE, GPIO_PORTC_BASE,
      GPIO_PIN_7, GPIO_PIN_6, SSI0_BASE, GPIO_PA2_SSI0CLK,
      GPIO_PA4_SSI0RX, GPIO_PA5_SSI0TX, 480, 272);

  ra_begin();

  displayOn(true);
  GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  //tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  //tft.PWM1out(255);


  // With hardware accelleration this is instant
  fillScreen(RA8875_WHITE);

  // Play with PWM
  /*for (uint8_t i=255; i!=0; i-=5 ) 
  {
    tft.PWM1out(i); 
    delay(10);
  }  
  for (uint8_t i=0; i!=255; i+=5 ) 
  {
    tft.PWM1out(i); 
    delay(10);
  }
  tft.PWM1out(255);*/
  
  

  while(1)
  {
	  fillScreen(RA8875_RED);
	    delay500();
	    fillScreen(RA8875_YELLOW);
	    delay500();
	    fillScreen(RA8875_GREEN);
	    delay500();
	    fillScreen(RA8875_CYAN);
	    delay500();
	    fillScreen(RA8875_MAGENTA);
	    delay500();
	    fillScreen(RA8875_BLACK);
	    delay500();

	    // Try some GFX acceleration!
/*	    drawCircle(100, 100, 50, RA8875_BLACK);
	    fillCircle(100, 100, 49, RA8875_GREEN);

	    fillRect(11, 11, 398, 198, RA8875_BLUE);
	    drawRect(10, 10, 400, 200, RA8875_GREEN);
	    //fillRoundRect(200, 10, 200, 100, 10, RA8875_RED);
	    drawPixel(10,10,RA8875_BLACK);
	    drawPixel(11,11,RA8875_BLACK);
	    drawLine(10, 10, 200, 100, RA8875_RED);
	    drawTriangle(200, 15, 250, 100, 150, 125, RA8875_BLACK);
	    fillTriangle(200, 16, 249, 99, 151, 124, RA8875_YELLOW);
	    drawEllipse(300, 100, 100, 40, RA8875_BLACK);
	    fillEllipse(300, 100, 98, 38, RA8875_GREEN);
	    // Argument 5 (curvePart) is a 2-bit value to control each corner (select 0, 1, 2, or 3)
	    drawCurve(50, 100, 80, 40, 2, RA8875_BLACK);
	    fillCurve(50, 100, 78, 38, 2, RA8875_WHITE);
*/
	    //pinMode(RA8875_INT, INPUT);
	    //digitalWrite(RA8875_INT, HIGH);

	    //tft.touchEnable(true);

	    //Serial.print("Status: "); Serial.println(tft.readStatus(), HEX);
	    //Serial.println("Waiting for touch events ...");
  }

  return 0;
}

/*void loop() 
{
  float xScale = 1024.0F/tft.width();
  float yScale = 1024.0F/tft.height();

  // Wait around for touch events 
  if (! digitalRead(RA8875_INT)) 
  {
    if (tft.touched()) 
    {
      Serial.print("Touch: "); 
      tft.touchRead(&tx, &ty);
      Serial.print(tx); Serial.print(", "); Serial.println(ty);
      // Draw a circle 
      tft.fillCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_WHITE);
    } 
  }
}*/


/*
#define NUM_SSI_DATA 8
const uint8_t pui8DataTx[NUM_SSI_DATA] =
{0x88, 0xF8, 0xF8, 0x88, 0x01, 0x1F, 0x1F, 0x01};

// Bit-wise reverses a number.
uint8_t
Reverse(uint8_t ui8Number)
{
	uint8_t ui8Index;
	uint8_t ui8ReversedNumber = 0;
	for(ui8Index=0; ui8Index<8; ui8Index++)
	{
		ui8ReversedNumber = ui8ReversedNumber << 1;
		ui8ReversedNumber |= ((1 << ui8Index) & ui8Number) >> ui8Index;
	}
	return ui8ReversedNumber;
}

int main(void)
{
	uint8_t ui32Index;
	uint8_t ui32Data;

	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	//GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	//GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);
	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_2);

	//SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 10000, 8);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 10000, 8);

	SSIEnable(SSI0_BASE);

	while(1)
	{
		for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
		{
			//ui32Data = (Reverse(pui8DataTx[ui32Index]) << 8) + (1 << ui32Index);
			ui32Data=0xAA;
			SSIDataPut(SSI0_BASE, ui32Data);
			while(SSIBusy(SSI0_BASE))
			{
			}
		}

	}

}*/


//EIGHT PUSH BUTTON SCANNING CODE
/*#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

typedef struct
{
	uint8_t state;
	uint8_t pin;
	uint32_t base;
} button;

button butts[8];

uint8_t count=0;
int main()
{
	uint32_t ui32Period;

	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_4);//|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_7|GPIO_PIN_6);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_4);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	//tmp=SysCtlClockGet();
	ui32Period=SysCtlClockGet()/160;

	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	butts[0].base=GPIO_PORTF_BASE;
	butts[0].pin=GPIO_PIN_4;

	butts[1].base=GPIO_PORTD_BASE;
	butts[1].pin=GPIO_PIN_7;

	butts[2].base=GPIO_PORTD_BASE;
	butts[2].pin=GPIO_PIN_6;

	butts[3].base=GPIO_PORTC_BASE;
	butts[3].pin=GPIO_PIN_7;

	butts[4].base=GPIO_PORTC_BASE;
	butts[4].pin=GPIO_PIN_6;

	butts[5].base=GPIO_PORTC_BASE;
	butts[5].pin=GPIO_PIN_5;

	butts[6].base=GPIO_PORTC_BASE;
	butts[6].pin=GPIO_PIN_4;

	butts[7].base=GPIO_PORTB_BASE;
	butts[7].pin=GPIO_PIN_3;

	while(1)
	{}
}

void Timer0IntHandler()
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(count==0)
	{
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3, 0);//latch the data
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);//set to shift mode
	}
	else
	{
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);//shift data
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
	}

	if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_4))
	//if(GPIOPinRead(butts[count].base, butts[count].pin))
		GPIOPinWrite(butts[count].base, butts[count].pin,0xFF);
		//GPIOPinWrite(butts[count].base, butts[count].pin,butts[count].pin);
	else
		GPIOPinWrite(butts[count].base, butts[count].pin,0);

	count++;
	if(count==9)
		count=0;
}*/

//PROVIDED TIMER CODE
/*int main()
{
	uint32_t ui32Period, tmp;

	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	tmp=SysCtlClockGet();
	ui32Period=tmp/40;

	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{}
}

void Timer0IntHandler()
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	else
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
}*/
