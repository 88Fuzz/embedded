/*
 * dsp.h
 *
 *  Created on: Feb 17, 2014
 *      Author: mreut
 */

#ifndef DSP_H_
#define DSP_H_

#include <stdint.h>

typedef struct
{
	uint32_t ui32Period;
	uint16_t ui16Sample;
	float fSample;
}Note;

#endif /* DSP_H_ */
