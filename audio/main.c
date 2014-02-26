//*****************************************************************************
//
// audio generation
//
//*****************************************************************************

// c includes
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
// part include
#include "inc/tm4c123gh6pm.h"
// hardware includes
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
// driver library includes
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
// custom includes
#include "dac.h"
#include "dsp.h"

//*****************************************************************************
//
// Interrupt Vectors
//
//*****************************************************************************

uint8_t ui16SSITransmitBuffer[DAC_BYTE_SIZE] = {0xFF, 0x00, 0xFF};
//uint8_t ui16SSIRecieveBuffer[DAC_BYTE_SIZE];
void Timer0IntHandler(void)
{
	// clear the interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

	// send sample out
	SSIDataPutNonBlocking(SSI0_BASE, ui16SSITransmitBuffer[0]);
	SSIDataPutNonBlocking(SSI0_BASE, ui16SSITransmitBuffer[1]);
	SSIDataPutNonBlocking(SSI0_BASE, ui16SSITransmitBuffer[2]);

	while(SSIBusy(SSI0_BASE)) {}
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
	/*
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}
	*/
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
}


//*****************************************************************************
//
// Peripheral Initializations
//
//*****************************************************************************
void InitializeFPU()
{
	FPULazyStackingEnable();
	FPUEnable();
}

void InitializeGPIO()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	// configured to act as CS for SPI communications
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);
}


void InitializeSPI()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_2);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
					   SSI_MODE_MASTER, SysCtlClockGet()/3, 8);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 1);
	SSIEnable(SSI0_BASE);
}


void InitializeTimer()
{
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = SysCtlClockGet() / 44100;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}

//*****************************************************************************
//
// Peripheral Initializations
//
//*****************************************************************************
int
main(void)
{
	// configure operating frequency to be at 80 MHz, system maximum
	// system clock calculation: 400 MHz(PLL) / 2(system) / 2.5(divisor) = 80 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// configure SPI peripheral
	InitializeSPI();

	// configure GPIO pins
	InitializeGPIO();

	// start the timer module
	InitializeTimer();

	while(1)
	{
		// raise your dongers
	}

}
