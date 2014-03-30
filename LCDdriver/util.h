#ifndef MYFUNCS_H_
#define MYFUNCS_H_
#include "lcd.h"

#define ONE_MILISEC 26670
#define TEN_MILISEC 266700
#define ONEHUNDRED_MILISEC 2667000
#define FIVEHUNDRED_MILISEC 13333340

extern text txtScale;
extern text txtScaleType;
extern LCDscreen ra8875;
extern uint16_t g_backgroundColor;


void flushSSIFIFO(uint32_t);
void mySSIDataPut(uint32_t , uint32_t);
void MYstrcpy(char *, char*);
void strAppendInt(char *, uint8_t);

#endif /* MYFUNCS_H_ */
