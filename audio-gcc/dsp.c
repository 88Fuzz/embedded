// c includes
//#include <math.h>
#include <stdint.h>
// custom includes
#include "wavetable.h"
#include "dsp.h"


//*****************************************************************************
//
// global constants and variables
//
//*****************************************************************************
static Note NoteArray[SIZE_NOTE_ARRAY];
static uint8_t ui8NoteCount;
const static float pfNoteAmplitudeScale[SIZE_NOTE_ARRAY + 1] = {0.0, 0.5, 0.333, 0.25, 0.2, 0.166, 0.142, 0.125, 0.0625};

static FilterParameters FilterParams;
static const float fSampleRate = VALUE_SAMPLE_RATE;
static const float fSampleRateDiv = 1.0 / ((float) VALUE_SAMPLE_RATE);
static const float fSizeLookupTable = 1024.0;
static const float fPi = 3.14159265359;
static const float fTwoPi = 2*3.14159265359;
const float* pfWaveTable = pfSawtoothTable;


//*****************************************************************************
//
// Note function definitions
//
//*****************************************************************************
void NoteIncrement(Note* CurrentNote)
{
	CurrentNote->fPosition += CurrentNote->fIncrement;

	// check if the current position is greater than the lookup table size
	if (CurrentNote->fPosition > fSizeLookupTable)
	{
		CurrentNote->fPosition = CurrentNote->fPosition - CurrentNote->fPosition;
	}
}

void NoteInitialize(Note* CurrentNote)
{
	CurrentNote->fFrequency = 0;
	// value to increment each time increment function is called
	CurrentNote->fIncrement = 1024.0 * (fFrequency * fSampleRateDiv);
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
	//fMax = ceilf(CurrentNote->fPosition);
	//fMin = floorf(CurrentNote->fPosition);
	fMax = (float) ((int) ((float)(CurrentNote->fPosition + 1.0)));
	fMax = fMax > fSizeLookupTable ? 0 : fMax;
	fMin = (float) ((int) CurrentNote->fPosition);

	// figure out how much to weight each sample for interpolation
	fDiffMin = fMax - CurrentNote->fPosition;
	fDiffMax = CurrentNote->fPosition - fMin;

	// generate intermediate sample value
	CurrentNote->fSample = pfWaveTable[(int) fMax] * fDiffMax;
	CurrentNote->fSample += pfWaveTable[(int) fMin] * fDiffMin;
}

void NoteOn(Note* CurrentNote, float fFrequency)
{
	CurrentNote->ui8State = NOTE_ON;
	CurrentNote->fFrequency = fFrequency;
	CurrentNote->fIncrement = 1024.0 * (fFrequency * fSampleRateDiv);
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
	CurrentNote->fIncrement = 1024.0 * (fFrequency * fSampleRateDiv);
}


//*****************************************************************************
//
// NoteArray function definitions
//
//*****************************************************************************

void InitializeNoteArray()
{
    for (n = 0; n < SIZE_NOTE_ARRAY; n++)
    {
	NoteInitialize(&NoteArray[n]);
    }
}


float NoteArrayProcess()
{
    float fOutSample;
    
    for (n = 0; n < SIZE_NOTE_ARRAY; n++)
    {
	if (NoteArray[n].ui8State == NOTE_OFF) continue;
	NotePlay(&NoteArray[n]);
	ui8NoteCount++;
	fOutSample += NoteArray[n].fSample;
    }
    
    return fOutSample;
}


//*****************************************************************************
//
// Filter function definitions
//
//*****************************************************************************

// reference: http://www.musicdsp.org/showone.php?id=142
void FilterInitialize()
{
    // 2 * pi * fc / fs
    FilterParams.fCutoff = fTwoPi * 20000 * fSampleRateDiv;
    FilterParams.fDamping = 1.0;
    FilterParams.fLow = 0.0;
    FilterParams.fHigh = 0.0;
    FilterParams.fBand = 0.0;
    FilterParams.fNotch = 0.0;
    FilterParams.fDelay[0] = 0;
    FilterParams.fDelay[1] = 0;
}

void FilterSetCutoff(float fCutoff)
{
	FilterParams.fCutoff = fTwoPi * fCutoff * fSampleRateDiv;
}

void FilterParamsDamping(float fDamping)
{
	FilterParams.fDamping = fDamping;
}

void FilterProcess(float fInput)
{
    // algorithm
    // loop
    // L = D2 + F1 * D1
    // H = I - L - Q1*D1
    // B = F1 * H + D1
    // N = H + L
    // store delays
    // D1 = B
    // D2 = L
    // outputs
    // L,H,B,N
    
    // low pass
    FilterParams.fLow = FilterParams.fDelay[1];
    FilterParams.fLow += FilterParams.fCutoff * FilterParams.fDelay[0];
    // high pass
    FilterParams.fHigh = fInput;
    FilterParams.fHigh -= FilterParams.fLow;
    FilterParams.fHigh -= FilterParams.fDamping * FilterParams.fDelay[0];
    // band pass
    FilterParams.fBand = FilterParams.fCutoff * FilterParams.fHigh;
    FilterParams.fBand += FilterParams.fDelay[0];
    // notch pass
    // FilterParams.fNotch = FilterParams.fHigh + FilterParams.fLow;
    
    // store delays
    FilterParams.fDelay[0] = FilterParams.fBand;
    FilterParams.fDelay[1] = FilterParams.fLow;
}

float FilterOutputLow()
{
	return FilterParams.fLow;
}

float FilterOutputHigh()
{
	return FilterParams.fHigh;
}

float FilterOutputBand()
{
	return FilterParams.fBand;
}
