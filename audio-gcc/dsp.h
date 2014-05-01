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
#define SIZE_NOTE_ARRAY 	    8
#define VALUE_SAMPLE_RATE 	48000
#define NOTE_OFF 		0x00
#define NOTE_ON			0x01

#define FILTER_LOW_PASS         0x01
#define FILTER_HIGH_PASS        0x02;
#define FILTER_BAND_PASS        0x03;
#define FILTER_NOTCH_PASS       0x04;


typedef struct
{
    uint8_t ui8State;		// on/off state of note
    uint16_t ui16Sample;	// output sample DAC
    volatile float fCutoffBase;	// note frequency
    float fIncrement;
    float fPosition;		// wave table position
    float fSample;		// output sample float
}Note;


typedef struct
{
    volatile float fCutoffBase;
    volatile float fCutoff;
    volatile float fDamping;
    float fLow;
    float fHigh;
    float fBand;
    float fNotch;
    float fDelay[2];
    volatile float* pfOutput;
    volatile uint8_t ui8Type;
}FilterParameters;


//*****************************************************************************
//
// function prototypes
//
//*****************************************************************************
void WaveTableSelect(uint16_t ui16Type);

void NoteIncrement(Note* CurrentNote);
void NoteInitialize(Note* CurrentNote);
void NoteInterpolate(Note* CurrentNote);
void NoteOn(Note* CurrentNote, float fCutoffBase);
void NoteOff(Note* CurrentNote);
void NotePlay(Note* CurrentNote);
void NoteSet(Note* CurrentNote, float fCutoffBase);

void SineInitialize();
void SquareInitialize();
void SawtoothInitialize();

void InitializeFilter();
void FilterSetCutoffCourse(uint32_t ui32Data);
void FilterSetCutoffFine(uint32_t ui32Data);
void FilterSetDamping(uint32_t ui32Data);
void FilterSetOutput(uint32_t ui32Data);
float FilterProcess(float fInput);

void InitializeNoteArray();
float NoteArrayProcess();
void NoteArrayNoteOn(uint32_t ui32Data);
void NoteArrayNoteOff(uint32_t ui32Data);
void NoteArrayAllOff();

void InitialzeVolume();
void SetVolume(uint32_t ui32Data);
float GetVolume();


#endif /* DSP_H_ */
