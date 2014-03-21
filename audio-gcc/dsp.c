// c includes
#include <math.h>
#include <stdint.h>
// driver library includes
#include "driverlib/fpu.h"
// custom includes
#include "wavetable.h"
#include "dsp.h"


//*****************************************************************************
//
// wave table function definitions
//
//*****************************************************************************
float* pfWaveTable = pfSawtoothTable;


//*****************************************************************************
//
// Note function definitions
//
//*****************************************************************************
void NoteIncrement(Note* CurrentNote)
{
	CurrentNote->fPosition += CurrentNote->fIncrement;

	// check if the current position is greater than the lookup table size
	if (CurrentNote->fPosition > SIZE_LOOKUP_TABLE)
	{
		CurrentNote->fPosition = CurrentNote->fPosition - CurrentNote->fPosition;
	}
}

void NoteInitialize(Note* CurrentNote, float fFrequency)
{
	CurrentNote->fFrequency = fFrequency;
	// value to increment each time increment function is called
	CurrentNote->fIncrement = SIZE_LOOKUP_TABLE * (fFrequency * VALUE_DIV_SAMPLE_RATE);
	CurrentNote->fPosition = 0;
	CurrentNote->fSample = 0;
	CurrentNote->ui16Sample = 0;
	CurrentNote->ui8State = NOTE_OFF;
}

void NoteInterpolate(Note* CurrentNote)
/*
 * gets the next sample by linearly interpolating the wave table
 */
{
	float fMax, fMin, fDiffMax, fDiffMin;
	// determine sample index above and below current position
	fMax = ceilf(CurrentNote->fPosition);
	fMin = floorf(CurrentNote->fPosition);

	// figure out how much to weight each sample for interpolation
	fDiffMin = fMax - CurrentNote->fPosition;
	fDiffMax = CurrentNote->fPosition - fMin;

	// generate intermediate sample value
	CurrentNote->fSample = pfWaveTable[(int) fMax] * fDiffMax;
	CurrentNote->fSample += pfWaveTable[(int) fMin] * fDiffMin;
}

void NoteOn(Note* CurrentNote)
{
	CurrentNote->ui8State = NOTE_ON;
}

void NoteOff(Note* CurrentNote)
{
	CurrentNote->ui8State = NOTE_OFF;
}

void NotePlay(Note* CurrentNote)
/*
 * prepares next sample for note
 */
{
	NoteIncrement(CurrentNote);
	NoteInterpolate(CurrentNote);
}

void NoteSet(Note* CurrentNote, float fFrequency)
/*
 * sets the note's frequency/pitch
 */
{
	CurrentNote->fFrequency = fFrequency;
	CurrentNote->fIncrement = SIZE_LOOKUP_TABLE * (fFrequency * VALUE_DIV_SAMPLE_RATE);
}


//*****************************************************************************
//
// Filter function definitions
//
//*****************************************************************************
static FilterParameters FilterParams;

void FilterInitialize()
{
    FilterParams.fOmega = 0.0;
}
