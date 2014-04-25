#ifndef PARAMETERS_H_
#define PARAMETERS_H_

//MIDI message defines
#define NOTEON 0x90
#define NOTEOFF 0x80

//MICRO parameters
#define MICRO_NOTEON			0x0
#define MICRO_NOTEOFF			0x01
#define MICRO_NOTEALLOFF		0x02
#define	 MICRO_ATTACK 			0x03
#define MICRO_HOLD				0x04
#define MICRO_RELEASE			0x05
#define MICRO_FILTER_COURSE		0x06
#define MICRO_FILTER_FINE		0x07
#define MICRO_FILTER_Q			0x08
#define MICRO_FILTERTYPE		0x09
#define MICRO_WAVETYPE			0x0A
#define MICRO_VOLUME			0x0B

#define MAXPARAMVAL				4095

typedef enum
{
	LOW=0,
	HIGH=1,
	BAND=2
} filter_t;

typedef enum
{
	SINE=0,
	SAW=1,
	SQUARE=2
}wave_t;

extern filter_t g_filterType;
extern wave_t g_waveType;

//Attack
extern uint8_t g_attack;
extern char g_attackLabel[7];

//Hold
extern uint8_t g_hold;
extern char g_holdLabel[5];

//Release
extern uint8_t g_release;
extern char g_releaseLabel[8];

//Filter Fine
extern uint8_t g_filterFine;
extern char g_filterFineLabel[8];

//Filter Course
extern uint8_t g_filterCourse;
extern char g_filterCourseLabel[8];

//Q
extern uint8_t g_filterQ;
extern char g_filterQLabel[2];

//volume
extern uint8_t g_volumeLevel;
extern char g_volumeLabel[4];



#endif /* PARAMETERS_H_ */
