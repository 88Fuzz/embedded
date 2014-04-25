#ifndef NOTEGEN_H_
#define NOTEGEN_H_

#include <stdint.h>
#include "lcd.h"
#include "driverlib/uart.h"

//key gen defines
#define NUMOCTAVES 5
#define SCALESIZE 7
#define SCALEACCISIZE 7
#define SCALEMAX NUMOCTAVES*8
#define SCALEACCIMAX NUMOCTAVES*7
#define LOWESTNOTE 12

typedef enum
{
  Cb=11,C=0,Cs=1,Db=1,D=2,
  Ds=3,Eb=3,E=4,Es=5,
  F=5,Fs=6,Gb=6,G=7,
  Gs=8,Ab=8,A=9,As=10,
  Bb=10,B=11,Bs=0
} key_t;

typedef enum
{
  OFF=0,
  ON=1
} buttonState_t;

typedef enum
{
  MAJOR=0,
  MINOR=1
} keyType_t;

typedef enum
{
  FIRST=0,
  SECOND=1,
  THIRD=2,
  FOURTH=3,
  FIFTH=4,
  SIXTH=5,
  SEVENTH=6,
  EIGHTH=7
} chord_t;

typedef struct
{
  int8_t midi;
  uint8_t octave;
  buttonState_t state;
} note;

extern note g_scale[SCALESIZE];
extern note g_scaleAcci[SCALEACCISIZE];

extern note g_octaves[SCALEMAX];
extern note g_octavesAcci[SCALEACCIMAX];

extern keyType_t g_keyType;
extern key_t g_key;
extern chord_t g_chord;

void initButtons();
void genScale();
void chordSelect();
void sendAllNotesOff();
void scanButtons();

#endif /* NOTEGEN_H_ */
