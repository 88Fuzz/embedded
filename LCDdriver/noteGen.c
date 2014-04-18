#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "noteGen.h"
#include "util.h"
#include "comm.h"


note g_scale[SCALESIZE];
note g_scaleAcci[SCALEACCISIZE];

note g_octaves[SCALEMAX];
note g_octavesAcci[SCALEACCIMAX];

keyType_t g_keyType;
key_t g_key;
chord_t g_chord;

/*
 * Initializes all push buttons to OFF and sets the
 * default key to C Major, with buttons selected to
 * the first
 */
void initButtons()
{
	uint8_t j;
	g_keyType=MAJOR;
	g_key=C;
	g_chord=FIRST;
	for(j=0;j<SCALESIZE;j++)
		g_scale[j].state=OFF;

	for(j=0;j<SCALEACCISIZE;j++)
		g_scaleAcci[j].state=OFF;

	for(j=0;j<SCALEMAX;j++)
		g_octaves[j].state=OFF;

	for(j=0;j<SCALEACCIMAX;j++)
		g_octavesAcci[j].state=OFF;
}

/*
 * Should be called whenever the key is changed.
 * changes base scale that the rest of the buttons
 * are based off of.
 */
void genScale()
{
	uint8_t tmp=g_key;
	if(g_keyType==MAJOR)//major key
	{
		g_scale[0].midi=LOWESTNOTE+tmp;
		tmp+=1;

	    g_scaleAcci[0].midi=LOWESTNOTE+tmp;
		tmp+=1;

	  	g_scale[1].midi=LOWESTNOTE+tmp;
	    tmp+=1;

	    g_scaleAcci[1].midi=LOWESTNOTE+tmp;
		tmp+=1;

	  	g_scale[2].midi=LOWESTNOTE+tmp;

	  	g_scaleAcci[2].midi=-1;
	    tmp+=1;

	  	g_scale[3].midi=LOWESTNOTE+tmp;
	    tmp+=1;

	    g_scaleAcci[3].midi=LOWESTNOTE+tmp;
	  	tmp+=1;

	  	g_scale[4].midi=LOWESTNOTE+tmp;
	    tmp+=1;

	    g_scaleAcci[4].midi=LOWESTNOTE+tmp;
	  	tmp+=1;

	  	g_scale[5].midi=LOWESTNOTE+tmp;
	    tmp+=1;

	    g_scaleAcci[5].midi=LOWESTNOTE+tmp;
	  	tmp+=1;

	  	g_scale[6].midi=LOWESTNOTE+tmp;
	  	tmp+=1;

	    g_scaleAcci[6].midi=-1;
	}
	else //minor key
	{
		g_scale[0].midi=LOWESTNOTE+tmp;
	    tmp++;

	    g_scaleAcci[0].midi=LOWESTNOTE+tmp;
	    tmp++;

		g_scale[1].midi=LOWESTNOTE+tmp;
	    g_scaleAcci[1].midi=-1;
	    tmp++;

		g_scale[2].midi=LOWESTNOTE+tmp;
	    tmp++;
	    g_scaleAcci[2].midi=LOWESTNOTE+tmp;
	    tmp++;

		g_scale[3].midi=LOWESTNOTE+tmp;
	    tmp++;
	    g_scaleAcci[3].midi=LOWESTNOTE+tmp;
	    tmp++;

		g_scale[4].midi=LOWESTNOTE+tmp;
	    g_scaleAcci[4].midi=-1;
		tmp++;

		g_scale[5].midi=LOWESTNOTE+tmp;
	    tmp++;
	    g_scaleAcci[5].midi=LOWESTNOTE+tmp;
	    tmp++;

		g_scale[6].midi=LOWESTNOTE+tmp;
	    tmp++;
	    g_scaleAcci[6].midi=LOWESTNOTE+tmp;
	    tmp++;
	}
}

void chordSelect()
{
	uint8_t j=0;
	uint8_t octOff=0; //octave offset
	uint8_t chordOff=g_chord; //chord offset chord-1
	uint8_t balanceOff=0; //balance offset
	uint8_t idx, cnt=0, button8Off=0;//button8Off is the offset for the 8th button in each row

	for(j=0;j<SCALEMAX;j++)
	{
		idx=j+chordOff-balanceOff-button8Off;
		if(idx>6)//out of range of scale array
		{
			octOff++;
			balanceOff+=7;
			idx=j+chordOff-balanceOff-button8Off;
		}

		if(cnt==7)
		{
			g_octaves[j].octave=octOff;
			g_octaves[j].midi=g_scale[idx].midi+(12*g_octaves[j].octave);
			g_octaves[j].state=OFF;
			button8Off++;
			cnt=-1;
		}
		else
		{
			g_octaves[j].octave=octOff;
			g_octaves[j].midi=g_scale[idx].midi+(12*g_octaves[j].octave);
			g_octaves[j].state=OFF;

	/*		if(g_scaleAcci[idx].midi<0)
			{
				g_octavesAcci[j].octave=octOff;
				g_octavesAcci[j].midi=-1;
				g_octavesAcci[j].state=OFF;
			}
			else
			{
				g_octavesAcci[j].octave=octOff;
				g_octavesAcci[j].midi=g_scaleAcci[idx].midi+(12*g_octaves[j].octave);
				g_octavesAcci[j].state=OFF;
			}*/
		}
		cnt++;
	}

		octOff=0;
		chordOff=g_chord;
		balanceOff=0;

	for(j=0;j<SCALEACCIMAX;j++)
	{

		idx=j+chordOff-balanceOff;
		if(idx>6)
		{
			octOff++;
			balanceOff+=7;
			idx=j+chordOff-balanceOff;
		}

		if(g_scaleAcci[idx].midi<0)
		{
			g_octavesAcci[j].octave=octOff;
			g_octavesAcci[j].midi=-1;
			g_octavesAcci[j].state=OFF;
		}
		else
		{
			g_octavesAcci[j].octave=octOff;
			g_octavesAcci[j].midi=g_scaleAcci[idx].midi+(12*g_octavesAcci[j].octave);
			g_octavesAcci[j].state=OFF;
		}
	}
	g_keyChange=1;
}

void sendAllNotesOff()
{
	uint8_t j;
	SENDALLNOTEOFF_MICRO();
	for(j=0;j<SCALEMAX;j++)
	{
		if(g_octaves[j].state==ON)
		{
			SENDNOTEOFF_MIDI(g_octaves[j].midi,0x00);
			g_octaves[j].state=OFF;
		}
	}

	for(j=0;j<SCALEACCIMAX;j++)
	{
		if(g_octavesAcci[j].state==ON)
		{
			SENDNOTEOFF_MIDI(g_octavesAcci[j].midi,0x00);
			g_octavesAcci[j].state=OFF;
		}
	}
}
