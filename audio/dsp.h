/*
 * dsp.h
 *
 *  Created on: Feb 17, 2014
 *      Author: mreut
 */

#ifndef DSP_H_
#define DSP_H_

#include <stdint.h>

#define LOOKUP_TABLE_SIZE 1024
#define SAMPLE_RATE 44100

typedef struct
{
	uint32_t ui32Period;
	uint16_t ui16Sample;
	float fSample;
}Note;

void SquareInitialize();
inline float SquareLookup(uint16_t index);

#endif /* DSP_H_ */
