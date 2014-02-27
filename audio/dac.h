/*
 * dac.h
 *
 *  Created on: Feb 22, 2014
 *      Author: mreut
 */

#ifndef DAC_H_
#define DAC_H_

#include "settings.h"

#ifdef MAX5216

#define DAC_MASK_NO_OP	0x00		// see datasheet
#define DAC_MASK_WRITE	0x40
#define DAC_MASK_POWER_DOWN 0x80

#define DAC_BIT_MASK 0xFFFFFF		// 24 bits
#define DAC_BYTE_SIZE 3				// 3 bytes => 24 bits



#endif

#endif /* DAC_H_ */
