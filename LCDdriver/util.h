#ifndef MYFUNCS_H_
#define MYFUNCS_H_
#include "lcd.h"

#define ONE_MILISEC 26670
#define TEN_MILISEC 266700
#define ONEHUNDRED_MILISEC 2667000
#define FIVEHUNDRED_MILISEC 13333340

extern uint8_t g_keyChange;
extern LCDscreen ra8875;
extern uint16_t g_backgroundColor;
extern uint8_t g_sld1Updated;
extern uint8_t g_sld2Updated;
extern uint8_t g_gridUpdated;
extern uint8_t g_waveUpdated;
extern uint8_t g_filterUpdated;
extern uint8_t g_sld1UpdatedMenu;
extern uint8_t g_sld2UpdatedMenu;


void Timer0IntHandler();

void updateWave();

void flushSSIFIFO(uint32_t);
void mySSIDataPut(uint32_t , uint32_t);
void MYstrcpy(char *, char*);
void strAppendInt(char *, uint8_t);

void keyTypeAppend();
void chordTextAppend();
void keyTextAppend();
void waveTextAppend();
void filterTextAppend();
void updateFilter();
void updateSld1();
void updateSld2();

#endif /* MYFUNCS_H_ */
