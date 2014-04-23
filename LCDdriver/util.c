#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "ra8875.h"
#include "util.h"
#include "lcd.h"
#include "noteGen.h"
#include "comm.h"
#include "parameters.h"

LCDscreen ra8875;
uint16_t g_backgroundColor=ORANGE_16BIT;
uint8_t g_keyChange;
uint8_t g_sld1Updated;
uint8_t g_sld2Updated;
uint8_t g_gridUpdated;
uint8_t g_waveUpdated;

/*
 * empties the SSI FIFO of SSIbase
 */
void flushSSIFIFO(uint32_t SSIbase)
{
	uint32_t ass;
  while((SSIDataGetNonBlocking(SSIbase,&ass))!=0);
/*	do
	{
		tmp=SSIDataGetNonBlocking(SSIbase,&ass);
	}while(tmp!=0);*/
}

/*
 * transmits data on the SSI base and waits for transmission
 * to complete before returning
 */
void mySSIDataPut(uint32_t base, uint32_t data)
{
	SSIDataPut(base, data);
	while(SSIBusy(base));
}

/*
 * copies contents of src into dest.
 * No error checking is done. Assumes dest
 * has enough space allocated.
 */
void MYstrcpy(char *dest, char *src)
{
	while(*src!='\0')
		*dest++=*src++;
	*dest='\0';
}

void updateWave()
{
	if(g_waveType==SINE)
	{
		g_txtWaveSine.selected=1;
		g_txtWaveSquare.selected=0;
		g_txtWaveTriangle.selected=0;
		text_drawSelected(&g_txtWaveSine);
		text_drawSelected(&g_txtWaveSquare);
		text_drawSelected(&g_txtWaveTriangle);
	}
	else if(g_waveType==SQUARE)
	{
		g_txtWaveSine.selected=0;
		g_txtWaveSquare.selected=1;
		g_txtWaveTriangle.selected=0;
		text_drawSelected(&g_txtWaveSine);
		text_drawSelected(&g_txtWaveSquare);
		text_drawSelected(&g_txtWaveTriangle);
	}
	else
	{
		g_txtWaveSine.selected=0;
		g_txtWaveSquare.selected=0;
		g_txtWaveTriangle.selected=1;
		text_drawSelected(&g_txtWaveSine);
		text_drawSelected(&g_txtWaveSquare);
		text_drawSelected(&g_txtWaveTriangle);
	}
}

/*
 * Appends ": i" at the end of str. assumes str has enough space
 */
void strAppendInt(char *str, uint8_t i)
{
  uint8_t tmp;
  while(*str!='\0')
    str++;

  *str++=':';
  *str++=' ';
  tmp=i/100;
  if(tmp)
	  *str++='1';
  else
	  *str++='0';

  tmp=(i/10)%10;
  switch(tmp)
  {
    case 0:
    	*str++='0';
    break;
    case 1:
    	*str++='1';
    break;
    case 2:
    	*str++='2';
    break;
    case 3:
    	*str++='3';
    break;
    case 4:
    	*str++='4';
    break;
    case 5:
    	*str++='5';
    break;
    case 6:
    	*str++='6';
    break;
    case 7:
    	*str++='7';
    break;
    case 8:
    	*str++='8';
    break;
    case 9:
    	*str++='9';
    break;
  }


  tmp=i%10;
  switch(tmp)
  {
    case 0:
    	*str++='0';
    break;
    case 1:
    	*str++='1';
    break;
    case 2:
    	*str++='2';
    break;
    case 3:
    	*str++='3';
    break;
    case 4:
    	*str++='4';
    break;
    case 5:
    	*str++='5';
    break;
    case 6:
    	*str++='6';
    break;
    case 7:
    	*str++='7';
    break;
    case 8:
    	*str++='8';
    break;
    case 9:
    	*str++='9';
    break;
  }

  *str++='\0';
}



void keyTypeAppend()
{
	if(g_keyType==MAJOR)
		MYstrcpy(g_txtKeyType.label,"Type: Major");
	else
		MYstrcpy(g_txtKeyType.label,"Type: Minor");
}

void chordTextAppend()
{
	if(g_chord==FIRST)
		MYstrcpy(g_txtChord.label,"Chord: I");
	else if(g_chord==SECOND)
		MYstrcpy(g_txtChord.label,"Chord: II");
	else if(g_chord==THIRD)
		MYstrcpy(g_txtChord.label,"Chord: III");
	else if(g_chord==FOURTH)
		MYstrcpy(g_txtChord.label,"Chord: IV");
	else if(g_chord==FIFTH)
		MYstrcpy(g_txtChord.label,"Chord: V");
	else if(g_chord==SIXTH)
		MYstrcpy(g_txtChord.label,"Chord: VI");
	else if(g_chord==SEVENTH)
		MYstrcpy(g_txtChord.label,"Chord: VII");
}

void keyTextAppend()
{
	if(g_key==0)
		MYstrcpy(g_txtKey.label,"Key: C");
	else if(g_key==1)
		MYstrcpy(g_txtKey.label,"Key: C#");
	else if(g_key==2)
		MYstrcpy(g_txtKey.label,"Key: D");
	else if(g_key==3)
		MYstrcpy(g_txtKey.label,"Key: D#");
	else if(g_key==4)
		MYstrcpy(g_txtKey.label,"Key: E");
	else if(g_key==5)
		MYstrcpy(g_txtKey.label,"Key: F");
	else if(g_key==6)
		MYstrcpy(g_txtKey.label,"Key: F#");
	else if(g_key==7)
		MYstrcpy(g_txtKey.label,"Key: G");
	else if(g_key==8)
		MYstrcpy(g_txtKey.label,"Key: G#");
	else if(g_key==9)
		MYstrcpy(g_txtKey.label,"Key: A");
	else if(g_key==10)
		MYstrcpy(g_txtKey.label,"Key: A#");
	else if(g_key==11)
		MYstrcpy(g_txtKey.label,"Key: B");
}

void waveTextAppend()
{
	if(g_waveType==SAW)
		MYstrcpy(g_txtWaveform.label,"Wave: Saw");
	else if(g_waveType==SINE)
		MYstrcpy(g_txtWaveform.label,"Wave: Sine");
	else if(g_waveType==SQUARE)
		MYstrcpy(g_txtWaveform.label,"Wave: Sqr");
}

void filterTextAppend()
{
	if(g_filterType==LOW)
		MYstrcpy(g_txtFilter.label,"Filter: LOW");
	else if(g_filterType==HIGH)
		MYstrcpy(g_txtFilter.label,"Filter: HIGH");
	else if(g_filterType==BAND)
		MYstrcpy(g_txtFilter.label,"Filter: BAND");
}
