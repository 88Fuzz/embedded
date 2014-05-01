//*****************************************************************************
//
// audio generation
//
//*****************************************************************************

// c includes
//#include <math.h>
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
#include "cmd.h"
#include "dac.h"
#include "dsp.h"
 
// test toggling of debug LED
//#define DEBUG_BLINK_LED
// test audio output of single note
//#define DEBUG_SSI_OUTPUT
// test ssi input command 
//#define DEBUG_SSI_INPUT

#define SAMPLE_PREPARE 	0x80
#define SAMPLE_READY 	0xC0


//*****************************************************************************
//
// static global defines
//
//*****************************************************************************
static uint32_t ui32OutSample = 0x00000000;
static uint8_t* pui8SPITx;
static uint32_t ui32SPIRx;
static volatile uint8_t ui8State = 0;


//*****************************************************************************
//
// Interrupt Vectors
//
//*****************************************************************************
/*
 * timer interrupt that occures once every sample period
 */
void Timer0IntHandler(void)
{
	// clear the timer interrupt flag
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

#if defined DEBUG_BLINK_LED
	// LED will toggle on/off every second
	GPIO_PORTE_DATA_R ^= 0x01;

#elif defined DEBUG_SSI_INPUT
	if (SSIDataGetNonBlocking(SSI0_BASE, &ui32SPIRx) != 0)
	{
		GPIO_PORTE_DATA_R = 0x01;	
	}
	else
	{
		GPIO_PORTE_DATA_R = 0x00;
		ui32SPIRx = 0;
	}	

#else	 
	// clear the interrupt
	pui8SPITx = (uint8_t*)&ui32OutSample;
	pui8SPITx[2] |= DAC_MASK_WRITE;

	// assert active low CS 
    GPIO_PORTD_DATA_R &= ~(0x02);
    
	// send sample out
	// each byte is placed in the SSI FIFO, 24 bits total are transmitted
	SSI3_DR_R = pui8SPITx[2];
	SSI3_DR_R = pui8SPITx[1];
	SSI3_DR_R = pui8SPITx[0];
	
	// if this triggers, CPU is taking too long to generate samples
	if (ui8State == SAMPLE_PREPARE) {GPIO_PORTE_DATA_R ^= 0x01;}
	
	while ((SSIDataGetNonBlocking(SSI0_BASE, &ui32SPIRx)) != 0)
	{
		switch (ui32SPIRx & CMD_MASK)
		{
			case CMD_NOTE_ON:
				NoteArrayNoteOn(ui32SPIRx & DATA_MASK);
				break;
			case CMD_NOTE_OFF:
				NoteArrayNoteOff(ui32SPIRx & DATA_MASK);
				break;
			case CMD_NOTE_ALL_OFF:
				NoteArrayAllOff();
				break;
			/* 
			case CMD_SET_ATTACK:
			case CMD_SET_HOLD:
			case CMD_SET_RELEASE:
			*/
			case CMD_SET_CUTOFF_COURSE:
				FilterSetCutoffCourse(ui32SPIRx & DATA_MASK);
				break;
			case CMD_SET_CUTOFF_FINE:
				FilterSetCutoffFine(ui32SPIRx & DATA_MASK);
				break;
			case CMD_SET_Q:
				FilterSetDamping(ui32SPIRx & DATA_MASK);
				break;
			case CMD_SET_FILTER_TYPE:
				FilterSetOutput(ui32SPIRx & DATA_MASK);
				break;
			case CMD_SET_WAVEFORM_TYPE:
				WaveTableSelect(ui32SPIRx & DATA_MASK);
				break;
			case CMD_SET_VOLUME:
				SetVolume(ui32SPIRx & DATA_MASK);
				break;
			//default:
				// do nothing
		}
	}
	
	// wait to de-assert CS until all 24 bits have transmitted
	while (SSI3_SR_R & SSI_SR_BSY) {}
	GPIO_PORTD_DATA_R |= 0x02;
    
    // change state to generate next sample after returning from interrupt
    ui8State = SAMPLE_PREPARE;
#endif 
}


//*****************************************************************************
//
// Peripheral Initializations
//
//*****************************************************************************
void InitializeFPU()
/*
 * initialize the floating point unit for dsp operation
 */
{
	// don't bother push floting point registers onto stack
	FPULazyStackingEnable();
	// denormals treated as zero
	FPUFlushToZeroModeSet(FPU_FLUSH_TO_ZERO_EN);
	FPUEnable();
}

void InitializeGPIO()
/*
 * initialize necessary GPIO pins
 */
{
	// PE0-PE3 used for debug
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
}

void InitializeSPI()
/*
 * initialize SSI input and output
 */
{
	// input SSI for sending data to DAC
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
					   SSI_MODE_SLAVE, SysCtlClockGet()/10, 16);
	SSIEnable(SSI0_BASE);
		
	// output SSI for recieving commands from other microcontroller
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinConfigure(GPIO_PD0_SSI3CLK);
	GPIOPinConfigure(GPIO_PD3_SSI3TX);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_3);
	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2,
					   SSI_MODE_MASTER, SysCtlClockGet()/3, 8);
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
	SSIEnable(SSI3_BASE);
	
	// clear input SSI RX FIFO
	while (SSIDataGetNonBlocking(SSI0_BASE, &ui32SPIRx) != 0) {}
}

void InitializeTimer()
/*
 * initialize timer to interrupt at desired sample period
 */
{
	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	
#ifdef DEBUG_BLINK_LED
	// set interrupt to occur once every second
	ui32Period = SysCtlClockGet();

#else
	// set interrupt to occur at the sample rate
	ui32Period = SysCtlClockGet() / VALUE_SAMPLE_RATE;

#endif

	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void InitializeSystem()
/*
 * set operating frequency and call initialization routines
 */
{
	// configure operating frequency to be at 80 MHz, system maximum
	// system clock calculation: 400 MHz(PLL) / 2(system) / 2.5(divisor) = 80 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	/***********************************************
	 peripheral initialization
	***********************************************/
	InitializeFPU();
	InitializeSPI();
	InitializeGPIO();
	
	/***********************************************
	 software initialization
	***********************************************/
	InitializeNoteArray();
    InitializeFilter();
    InitialzeVolume();
}


//*****************************************************************************
//
// main routine
//
//*****************************************************************************
int main(void)
{
	float fOutSample;
	
	InitializeSystem();
    
	// start the timer module with interrupt
	InitializeTimer();

#ifdef DEBUG_SSI_OUTPUT
	NoteArrayNoteOn(2);
#endif

	ui8State = SAMPLE_PREPARE;

	while(1)
	{
		// wait here until sample is output
		while (ui8State == SAMPLE_READY) {}
        
        /***********************************************
		 calculate next output sample
		***********************************************/
		fOutSample = NoteArrayProcess();
		fOutSample = FilterProcess(fOutSample);
		
		/***********************************************
		 preperation of sample for dac output
		***********************************************/
        fOutSample *= 0.12;	// scale to keep things from getting too loudS
        fOutSample *= GetVolume();
        // DC offset to allow sample to rest between 0 and 3V
        fOutSample += 0.5;                                      
        // convert to fixed point
        ui32OutSample = (uint32_t) (UINT16_MAX * fOutSample);   
		// "left shift" by 6 for DAC, multiply is a single cycle 
		ui32OutSample *= 64;                                    
		ui8State = SAMPLE_READY;
	}
}
