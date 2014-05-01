#ifndef COMM_H_
#define COMM_H_
/****************************************************************************
*****************************************************************************
****************************AUDIO COMMANDS***********************************
*****************************************************************************
*****************************************************************************

Commands are passed between the user input controller and audio controller via
SPI communication. The Tiva C series microcontroller has a maximum SPI message
size of 16 bits. Therefore, the first 4 bits will designate the command and
following 12 bits will hold the data associated with the command.

Bit Position: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
              |-command-| |---------data----------|


------------------------------Command Table----------------------------------

Command Number          Command Type            Data Range

0                       Note On                 0-82 (Midi 24-106)

1                       Note Off                0-82 (Midi 24-106)

2                       All Notes Off           none

3                       Set Attack              0-4095 (0 is no attack time)

4                       Set Hold                0-4095 (4095 is infinte sustain)

5                       Set Release             0-4095

6                       Filter Cutoff Course    0-4095

7                       Filter Cutoff Fine      0-4095

8                       Filter Q Factor         0-4095

9                       Filter Type             0-2 (0: low, 1: high, 2: band)

10                      Waveform Type           0-2 (0: sine, 1. saw, 2: square)

11                      Output Volume           0-4095

12

13

14

15

*****************************************************************************
*****************************************************************************
*****************************************************************************
*****************************************************************************
****************************************************************************/

#define SENDNOTEON_ALL(note,vel) \
	SENDNOTEON_MIDI(note,vel); \
	SENDNOTEON_MICRO(note);

#define SENDNOTEOFF_ALL(note,vel) \
	SENDNOTEOFF_MIDI(note,vel); \
	SENDNOTEOFF_MICRO(note);

#define SENDNOTEON_MIDI(note,vel) \
	UARTCharPutNonBlocking(UART0_BASE, NOTEON); \
	UARTCharPutNonBlocking(UART0_BASE,note); \
	UARTCharPutNonBlocking(UART0_BASE, vel);

#define SENDNOTEOFF_MIDI(note,vel) \
	UARTCharPutNonBlocking(UART0_BASE, NOTEOFF); \
	UARTCharPutNonBlocking(UART0_BASE, note); \
	UARTCharPutNonBlocking(UART0_BASE, vel);

#define SENDNOTEON_MICRO(note) \
		SSIDataPut(SSI0_BASE,(note-LOWESTNOTE));

#define SENDNOTEOFF_MICRO(note) \
		SSIDataPut(SSI0_BASE,((MICRO_NOTEOFF<<12)+(note-LOWESTNOTE)));

#define SENDALLNOTEOFF_MICRO() \
		SSIDataPut(SSI0_BASE,(MICRO_NOTEALLOFF<<12));

#define SENDCOMMAND_MICRO(cmd,val) \
		SSIDataPut(SSI0_BASE,((cmd<<12)+(val)));



#endif /* COMM_H_ */
