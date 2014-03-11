/*
 * dsp.h
 *
 *  Created on: Feb 17, 2014
 *      Author: mreut
 */

#ifndef DSP_H_
#define DSP_H_

#include <stdint.h>

//*****************************************************************************
//
// defines
//
//*****************************************************************************
#define SIZE_LOOKUP_TABLE 	1024
#define SIZE_NOTE_ARRAY 	8
#define VALUE_PI 			3.14159265
#define VALUE_SAMPLE_RATE 	44100

#define NOTE_OFF 			0
#define NOTE_ON				1

typedef struct
{
	uint16_t ui16State;		// on/off state of note
	uint16_t ui16Sample;	// output sample DAC
	float fFrequency;		// note frequency
	float fIncrement;
	float fPosition;		// wave table position
	float fSample;			// output sample float
}Note;


//*****************************************************************************
//
// function prototypes
//
//*****************************************************************************
void NoteInitialize(Note* CurrentNote, float fFrequency);
void NoteIncrement(Note* CurrentNote);
void NoteInterpolate(Note* CurrentNote);
void NoteOn(Note* CurrentNote);
void NoteOff(Note* CurrentNote);

void SineInitialize();
void SquareInitialize();
inline float SquareLookup(uint16_t index);

#endif /* DSP_H_ */
