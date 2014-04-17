#define PART_TM4C123GH6PM
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "midi.h"

//limited to 16 notes. Do you want more? feel free to change the number :)
#define MAXLENGTH 16

uint32_t ulStatus;
unsigned short message;
unsigned short sequence[MAXLENGTH]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned short i,j=0,n=0,flag=0;
uint32_t ulADC0_Value[4];
signed short trans=0;

int main(void) {

  //set clock to 20MHz (should be enough)
//  SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  //configure switches
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_6);

  //enable UART for MIDI send/receive
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinConfigure(GPIO_PB0_U1RX); //B0 receptor
  GPIOPinConfigure(GPIO_PB1_U1TX); //B1 transmitter
  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 31250, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE) );

  //configure ADC0 for potentiometers
/*  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
  ADCHardwareOversampleConfigure(ADC0_BASE, 8);
  ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4|GPIO_PIN_5);
  ADCSequenceStepConfigure(ADC0_BASE,  1 , 0 , ADC_CTL_CH8 );
  ADCSequenceStepConfigure(ADC0_BASE,  1 , 1 , ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
  ADCSequenceEnable(ADC0_BASE, 1);
  ADCIntClear(ADC0_BASE, 1);
*/

  //Send all the possible note Off for reset
  for (i=0;i<128; i++){
    UARTCharPutNonBlocking(UART1_BASE,MIDI_CH0_NOTE_OFF);
    UARTCharPutNonBlocking(UART1_BASE,i);
    UARTCharPutNonBlocking(UART1_BASE,0);
    SysCtlDelay(10000);
  }
  i=0;

  //enable UART Interrupts
  IntMasterEnable();
  IntEnable(INT_UART1);
  UARTIntEnable(UART1_BASE, UART_INT_RX|UART_INT_RT);

  while (1)    { //loop forever

    //read the pontetiometers value
//    ADCProcessorTrigger(ADC0_BASE, 1 );
//    while(!ADCIntStatus(ADC0_BASE, 1 , false)){  }
//    ADCIntClear(ADC0_BASE, 1 );
//    ADCSequenceDataGet(ADC0_BASE, 1 , ulADC0_Value);

    //send note on
    UARTCharPutNonBlocking(UART1_BASE, MIDI_CH0_NOTE_ON );  //on
    UARTCharPutNonBlocking(UART1_BASE,84);//sequence[i]+trans);//note
    UARTCharPutNonBlocking(UART1_BASE,127);//(ulADC0_Value[1]+1)/32-1);//velocity

    //delay between notes
    SysCtlDelay(5000000);

    //send note off
    UARTCharPutNonBlocking(UART1_BASE,MIDI_CH0_NOTE_OFF);//off
    UARTCharPutNonBlocking(UART1_BASE,84);//sequence[i]+trans);//same note
    UARTCharPutNonBlocking(UART1_BASE,0);       //velocity=0

    //next note
    if (n!=0)
      i = (i + 1) % n;

    //check switches
        if(!GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5)){
            trans++;
        }
        if(!GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6)){
          trans--;
        }

  }
}

void UARTIntHandler(void){

  IntMasterDisable();
  IntDisable(INT_UART1);

  //get interrupt status. RX or RT?
  ulStatus = UARTIntStatus(UART1_BASE, true);
  //clear the asserted interrupts
  UARTIntClear(UART1_BASE, ulStatus);

  //loop while there are chars
  while(UARTCharsAvail(UART1_BASE)){
    message=UARTCharGetNonBlocking(UART1_BASE);

    if(flag){
      sequence[j]=message;
      j=(j + 1) % MAXLENGTH;
      n++;
    }

    //if this message is a note On, next is the note code
    flag=isNoteOn(message);

  }
  IntMasterEnable();
  IntEnable(INT_UART1);
}
