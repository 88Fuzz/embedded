#ifndef MYFUNCS_H_
#define MYFUNCS_H_
#include "lcd.h"

#define MICRO_NOTEON 0x0
#define MICRO_NOTEOFF 0x01
#define MICRO_NOTEALLOFF 0x02

#define ONE_MILISEC 26670
#define TEN_MILISEC 266700
#define ONEHUNDRED_MILISEC 2667000
#define FIVEHUNDRED_MILISEC 13333340

extern text txtScale;
extern text txtScaleType;
extern LCDscreen ra8875;
extern uint16_t g_backgroundColor;

void Timer0IntHandler();

void flushSSIFIFO(uint32_t);
void mySSIDataPut(uint32_t , uint32_t);
void MYstrcpy(char *, char*);
void strAppendInt(char *, uint8_t);

typedef struct
{
	uint8_t note;
	uint8_t state;
} deleteTHIS;

#endif /* MYFUNCS_H_ */
