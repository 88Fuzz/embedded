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

#define SAMPLE_PREPARE 	0x80
#define SAMPLE_READY 	0xC0

//*****************************************************************************
//
// static global defines
//
//*****************************************************************************

static uint32_t ui32OutSample = 0x00000000;
static uint8_t* pui8SPITx;
static volatile uint8_t ui8State = 0;


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
	//GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
    GPIO_PORTA_DATA_R &= ~(0x08);
    
	// send sample out
	SSIDataPutNonBlocking(SSI0_BASE, pui8SPITx[2]);
	SSIDataPutNonBlocking(SSI0_BASE, pui8SPITx[1]);
	SSIDataPutNonBlocking(SSI0_BASE, pui8SPITx[0]);
	
	if (ui8State == SAMPLE_PREPARE)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);
	}
	/*
    else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
	}
	*/
	while (SSIBusy(SSI0_BASE)) {}
	//GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
	GPIO_PORTA_DATA_R |= 0x08;
    ui8State = SAMPLE_PREPARE;
}


//*****************************************************************************
//
// Peripheral Initializations
//
//*****************************************************************************
void InitializeFPU()
{
	FPULazyStackingEnable();
	FPUFlushToZeroModeSet(FPU_FLUSH_TO_ZERO_EN);
	FPUEnable();
}

void InitializeGPIO()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	// configured to act as CS for SPI communications
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);
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
	ui32Period = SysCtlClockGet() / VALUE_SAMPLE_RATE;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void InitializeSystem()
{
	// configure operating frequency to be at 80 MHz, system maximum
	// system clock calculation: 400 MHz(PLL) / 2(system) / 2.5(divisor) = 80 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// configure FPU
	InitializeFPU();

	// configure SPI peripheral
	InitializeSPI();

	// configure GPIO pins
	InitializeGPIO();
}

//*****************************************************************************
//
// main routine
//
//*****************************************************************************
Note NoteArray[SIZE_NOTE_ARRAY];
uint8_t ui8NoteCount;
float pfNoteAmplitudeScale[SIZE_NOTE_ARRAY + 1] = {0.0, 1.0, 0.5, 0.333, 0.25, 0.2, 0.166, 0.142, 0.125};
float fOutSample;
int main(void)
{
	uint32_t n;
	InitializeSystem();
    
	for (n = 0; n < SIZE_NOTE_ARRAY; n++)
	{
		NoteInitialize(&NoteArray[n], 440.0 * n);
	}

	NoteOn(&NoteArray[1]);
	//NoteOn(&NoteArray[2]);
	//NoteOn(&NoteArray[3]);
	ui8State = SAMPLE_PREPARE;
	// start the timer module with interrupt

	InitializeTimer();

	while(1)
	{
		// wait here until sample is output
		while (ui8State == SAMPLE_READY) {}
		ui8NoteCount = 0;
        fOutSample = 0;
        for (n = 0; n < SIZE_NOTE_ARRAY; n++)
		{
			if (NoteArray[n].ui8State == NOTE_OFF) continue;
			NotePlay(&NoteArray[n]);
            ui8NoteCount++;
			fOutSample += NoteArray[n].fSample;
		}
		fOutSample *= pfNoteAmplitudeScale[ui8NoteCount];       // scale amplitude based on active notes to avoid clipping
        fOutSample += 0.5;                                      // DC offset to allow sample to rest between 0 and 3V
        ui32OutSample = (uint32_t) (UINT16_MAX * fOutSample);   // convert to fixed point
		ui32OutSample *= 64;                                    // "left shift" by 6, multiply is a single cycle 
		ui8State = SAMPLE_READY;
	}

}
