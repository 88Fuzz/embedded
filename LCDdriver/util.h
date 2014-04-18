#ifndef MYFUNCS_H_
#define MYFUNCS_H_
#include "lcd.h"

#define ONE_MILISEC 26670
#define TEN_MILISEC 266700
#define ONEHUNDRED_MILISEC 2667000
#define FIVEHUNDRED_MILISEC 13333340

extern text g_txtKey;
extern text g_txtKeyType;
extern text g_txtChord;
extern uint8_t g_keyChange;
extern LCDscreen ra8875;
extern uint16_t g_backgroundColor;

void Timer0IntHandler();

void flushSSIFIFO(uint32_t);
void mySSIDataPut(uint32_t , uint32_t);
void MYstrcpy(char *, char*);
void strAppendInt(char *, uint8_t);
void scanButtons();

void keyTypeAppend();
void chordTextAppend();
void keyTextAppend();

#endif /* MYFUNCS_H_ */
