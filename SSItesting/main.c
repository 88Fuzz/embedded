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
//#include "ra8875.h"

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

