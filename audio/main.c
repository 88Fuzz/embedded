//*****************************************************************************
//
// Testing the peripherals and stuff.
//
//*****************************************************************************

// c includes
#include <stdint.h>
#include <stdbool.h>
// part include
#include "inc/tm4c123gh6pm.h"
// hardware includes
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
// driver library includes
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/udma.h"
// custom includes
#include "dac.h"
#include "dsp.h"

#pragma DATA_ALIGN(ui8ControlTable, 1024)
uint8_t ui8ControlTable[1024];


//*****************************************************************************
//
// Timer0 Interrupt
//
//*****************************************************************************
uint8_t ui16SSITransmitBuffer[DAC_WORD_SIZE];
uint8_t ui16SSIRecieveBuffer[DAC_WORD_SIZE];
void Timer0IntHandler(void)
{
	// clear the interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if (ui16Count++ >= 20)
	{
		ui16Out = ui16Out == 0x7FFF ? 0x7000 : 0x7FFF;
		ui16Count = 0;
	}
	// send sample out
	SSIDataPutNonBlocking(SSI0_BASE, ui16Out);

	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}

	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
}


//*****************************************************************************
//
// Blink the on-board LED, interrupt style.
//
//*****************************************************************************
int
main(void)
{
	uint32_t ui32Period;

	// configure operating frequency
	SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// enable needed peripherals
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// configure general purpose input and output
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);


	// configure ssi module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
				   GPIO_PIN_2);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
					   SSI_MODE_MASTER, 5000000, 16);
	SSIDMAEnable(SSI0_BASE, SSI_DMA_TX);
	SSIEnable(SSI0_BASE);

	//  configure udma
	uDMAChannelAttributeDisable(UDMA_CHANNEL_SSI0TX, UDMA_ATTR_ALTSELECT |UDMA_ATTR_HIGH_PRIORITY| UDMA_ATTR_REQMASK );
	uDMAChannelAttributeEnable(UDMA_CHANNEL_SSI0TX, UDMA_ATTR_BURST);
	uDMAChannelControlSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT,
            			  UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
	uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT,
						   UDMA_MODE_AUTO,
						   (void*) ui16SSITransmitBuffer,
						   (void*) ui16SSIRecieveBuffer,
						   sizeof(ui16SSITransmitBuffer));
	uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);


	// configure timer module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = SysCtlClockGet() / 44100;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	// start the timer module
	TimerEnable(TIMER0_BASE, TIMER_A);

	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);

	while(1)
	{
		// raise your dongers
	}

}
