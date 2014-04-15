/*
 * cmd.h
 * 
 * Copyright 2014 Michael Reutman <mreut@m>
 * 
 *  Created on: April 16, 2014
 *      Author: mreut
 * 
 */
 
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


#ifndef CMD_H_
#define CMD_H_

#define CMD_MASK 0xFFFFF000
#define DATA_MASK 0x00000FFF

#define CMD_NOTE_ON 0x0000
#define CMD_NOTE_OFF 0x1000
#define CMD_NOTE_ALL_OFF 0x2000
#define CMD_SET_ATTACK 0x3000
#define CMD_SET_HOLD 0x4000
#define CMD_SET_RELEASE 0x5000
#define CMD_SET_CUTOFF_COURSE 0x6000
#define CMD_SET_CUTOFF_FINE 0x7000
#define CMD_SET_Q 0x8000
#define CMD_SET_FILTER_TYPE 0x9000
#define CMD_SET_WAVEFORM_TYPE 0xA000
#define CMD_SET_VOLUME 0xB000

#endif
