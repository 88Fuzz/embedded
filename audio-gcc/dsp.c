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
static float pfSquareTable[1024];
static float pfSawtoothTable[1024];
//float pfSineTable[SIZE_LOOKUP_TABLE];

float* pfWaveTable = pfSawtoothTable;


//*****************************************************************************
//
// wave table function definitions
//
//*****************************************************************************
/*
void SineInitialize()
{
	float fRadians;
	uint16_t n = 0;
	fRadians = (2 * VALUE_PI) / SIZE_LOOKUP_TABLE;
	while (n < 1024)
	{
		pfSineTable[n] = 0.5 * sinf(fRadians * n);
		n++;
	}
}
*/

void SquareInitialize()
{
	int n = 0;
	for (n = 0; n < SIZE_LOOKUP_TABLE; n++)
	{
		pfSquareTable[n] = sinf((2.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSquareTable[n] += 1.0/3.00 * sinf((06.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSquareTable[n] += 1.0/5.00 * sinf((10.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSquareTable[n] += 1.0/7.00 * sinf((14.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSquareTable[n] += 1.0/9.00 * sinf((18.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSquareTable[n] += 1.0/11.0 * sinf((22.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSquareTable[n] *= 1.65/VALUE_PI;
	}
}

void SawtoothInitialize()
{
    int n = 0;
	for (n = 0; n < SIZE_LOOKUP_TABLE; n++)
	{
		pfSawtoothTable[n] = 0.0;
        pfSawtoothTable[n] -= VALUE_DIV_PI * sinf((2.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.500*VALUE_DIV_PI * sinf((04.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.333*VALUE_DIV_PI * sinf((06.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.250*VALUE_DIV_PI * sinf((08.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.200*VALUE_DIV_PI * sinf((10.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.166*VALUE_DIV_PI * sinf((12.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.142*VALUE_DIV_PI * sinf((14.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.125*VALUE_DIV_PI * sinf((16.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
        pfSawtoothTable[n] -= 0.111*VALUE_DIV_PI * sinf((16.0*VALUE_PI*n) / SIZE_LOOKUP_TABLE);
		pfSawtoothTable[n] *= 0.9;
	}
}


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
	CurrentNote->fIncrement = SIZE_LOOKUP_TABLE * (fFrequency / VALUE_SAMPLE_RATE);
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
	CurrentNote->fIncrement = SIZE_LOOKUP_TABLE * (fFrequency / VALUE_SAMPLE_RATE);
}
