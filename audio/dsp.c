// c includes
#include <math.h>
#include <stdint.h>
// driver library includes
#include "driverlib/fpu.h"
// custom includes
#include "dsp.h"


//*****************************************************************************
//
// static includes
//
//*****************************************************************************
//static float pfSquareTable[1024];
//static float pfSawTable[1024];
float pfSineTable[SIZE_LOOKUP_TABLE];

float* pfWaveTable = pfSineTable;


//*****************************************************************************
//
// wave table function definitions
//
//*****************************************************************************
void SineInitialize()
{
	float fRadians;
	uint16_t n = 0;
	fRadians = (2 * VALUE_PI) / 1024;
	while (n < 1024)
	{
		pfSineTable[n] = sinf(fRadians * n);
		n++;
	}
}

/*
void SquareInitialize()
{
	int n = 0;
	for (n = 0; n < LOOKUP_TABLE_SIZE; n++)
	{
		pfSquareTable[n] = sinf((2.0*VALUE_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/3.00 * sinf((06.0*VALUE_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/5.00 * sinf((10.0*VALUE_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/7.00 * sinf((14.0*VALUE_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/9.00 * sinf((18.0*VALUE_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/11.0 * sinf((22.0*VALUE_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] *= 3.6/VALUE_PI;
	}
}

float SquareLookup(uint16_t n)
{
	n = n >= LOOKUP_TABLE_SIZE ? 0 : n;
	return pfSquareTable[n];
}
*/


//*****************************************************************************
//
// Note function definitions
//
//*****************************************************************************
void NoteInitialize(Note* CurrentNote, float fFrequency)
{
	CurrentNote->fFrequency = fFrequency;
	CurrentNote->fIncrement = SIZE_LOOKUP_TABLE * (fFrequency / VALUE_SAMPLE_RATE);
	CurrentNote->fPosition = 0;
	CurrentNote->fSample = 0;
	CurrentNote->ui16Sample = 0;
	CurrentNote->ui16State = NOTE_OFF;
}

void NoteIncrement(Note* CurrentNote)
{
	CurrentNote->fPosition += CurrentNote->fIncrement;

	// check if the current position is greater than the lookup table size
	if (CurrentNote->fPosition > SIZE_LOOKUP_TABLE)
	{
		CurrentNote->fPosition = CurrentNote->fPosition - CurrentNote->fPosition;
	}
}

void NoteInterpolate(Note* CurrentNote)
{
	float fMax, fMin, fDiffMax, fDiffMin;
	// determine sample index above and below current position
	fMax = ceilf(CurrentNote->fPosition);
	fMin = floorf(CurrentNote->fPosition);

	// figure out how much to weight each sample for interpolation
	fDiffMax = fMax - CurrentNote->fSample;
	fDiffMin = CurrentNote->fSample - fMin;

	// generate intermediate sample value
	CurrentNote->fSample = pfWaveTable[(int) fMax] * fDiffMax;
	CurrentNote->fSample += pfWaveTable[(int) fMin] * fDiffMin;
}

void NoteOn(Note* CurrentNote)
{
	CurrentNote->ui16State = NOTE_ON;
}

void NoteOff(Note* CurrentNote)
{
	CurrentNote->ui16State = NOTE_OFF;
}
