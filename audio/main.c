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
// static global defines
//
//*****************************************************************************

//static uint8_t pui8SPITxBuffer[4] = {0xFF, 0x00, 0xFF, 0x00};

static uint32_t ui32OutSample = 0x00000000;
static uint8_t* pui8SPITx;
static uint32_t ui32State = 0;
//*****************************************************************************
//
// Interrupt Vectors
//
//*****************************************************************************
void Timer0IntHandler(void)
{
	// clear the interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	pui8SPITx = (uint8_t*)&ui32OutSample;
	pui8SPITx[2] |= DAC_MASK_WRITE;
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

	// send sample out
	SSIDataPutNonBlocking(SSI0_BASE, pui8SPITx[2]);
	SSIDataPutNonBlocking(SSI0_BASE, pui8SPITx[1]);
	SSIDataPutNonBlocking(SSI0_BASE, pui8SPITx[0]);

	while(SSIBusy(SSI0_BASE)) {}
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
	ui32State = 0;
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
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2,
					   SSI_MODE_MASTER, SysCtlClockGet()/3, 8);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
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
// helper functions
//

//*****************************************************************************
/*
inline void PrepareSampleSPITx(float fInputSample)
{
	(uint32_t *)pui8SPITxBuffer = (uint32_t) (DAC_MASK_WRITE | ((UINT16_MAX * fInputSample) << 22));
	uint32_t ui32InputSample = (uint32_t) UINT16_MAX * fInputSample;
	*(uint32	_t *)pui8SPITxBuffer = DAC_MASK_WRITE | (ui32InputSample << 22);
}
*/

//*****************************************************************************
//
// main routine
//
//*****************************************************************************
int main(void)
{
	float fOutSample;
	uint32_t n = 0;

	float fRadians;
	float pfSineTable[1024];

	// configure operating frequency to be at 80 MHz, system maximum
	// system clock calculation: 400 MHz(PLL) / 2(system) / 2.5(divisor) = 80 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// configure FPU
	InitializeFPU();

	// configure SPI peripheral
	InitializeSPI();

	// configure GPIO pins
	InitializeGPIO();

	// start the timer module
	InitializeTimer();

	fRadians = (2 * 3.14159265) / 1024;
	while (n < 1024)
	{
		pfSineTable[n] = sinf(fRadians * n);
		n++;
	}
	n = 0;
	while(1)
	{
		if (ui32State != 1)
		{
			// raise your dongers
			fOutSample = pfSineTable[n++];
			n = n >= 1024 ? 0 : n;
			fOutSample /= 2.2;
			fOutSample += 0.5;
			ui32OutSample = (uint32_t) UINT16_MAX * (fOutSample);
			ui32OutSample <<= 6;
			ui32State = 1;
		}
	}

}
