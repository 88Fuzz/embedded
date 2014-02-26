// c includes
#include <math.h>
#include <stdint.h>
// driver library includes
#include "driverlib/fpu.h"
// custom includes
#include "dsp.h"
#include "settings.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

//*****************************************************************************
//
// static includes
//
//*****************************************************************************

static float pfSquareTable[1024];
//static float pfSawTable[1024];


//*****************************************************************************
//
// wave table implementation
//
//*****************************************************************************

void SquareInitialize()
{
	int n = 0;
	for (n = 0; n < LOOKUP_TABLE_SIZE; n++)
	{
		pfSquareTable[n] = sinf((2.0*M_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/3.00 * sinf((06.0*M_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/5.00 * sinf((10.0*M_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/7.00 * sinf((14.0*M_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/9.00 * sinf((18.0*M_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] += 1.0/11.0 * sinf((22.0*M_PI*n) / LOOKUP_TABLE_SIZE);
		pfSquareTable[n] *= 3.6/M_PI;
	}
}

float SquareLookup(uint16_t n)
{
	n = n >= LOOKUP_TABLE_SIZE ? 0 : n;
	return pfSquareTable[n];
}

