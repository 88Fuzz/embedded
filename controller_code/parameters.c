#include <stdint.h>
#include "parameters.h"
#include "comm.h"

#define	 MICRO_ATTACK 			0x03
#define MICRO_HOLD				0x04
#define MICRO_RELEASE			0x05
#define MICRO_FILTER_COURSE		0x06
#define MICRO_FILTER_FINE		0x07
#define MICRO_FILTER_Q			0x08
#define MICRO_FILTER_TYPE		0x09
#define MICRO_WAVETYPE			0x0A
#define MICRO_VOLUME			0x0B

filter_t g_filterType;
wave_t g_waveType;

//Attack
uint8_t g_attack;
char g_attackLabel[7]="Attack";

//Hold
uint8_t g_hold;
char g_holdLabel[5]="Hold";

//Release
uint8_t g_release;
char g_releaseLabel[8]="Release";

//Filter Fine
uint8_t g_filterFine;
char g_filterFineLabel[8]="Ffine";

//Filter Course
uint8_t g_filterCourse;
char g_filterCourseLabel[8]="Fcourse";

//Q
uint8_t g_filterQ;
char g_filterQLabel[2]="Q";

//volume
uint8_t g_volumeLevel;
char g_volumeLabel[4]="Vol";
