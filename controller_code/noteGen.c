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
#include "parameters.h"


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






void scanButtons()
{
	uint32_t tmp;
	uint16_t scanData, shiftData;
	uint8_t j,k,acciOff,baseAcciOff,
			baseOctOff,octOff,majorPressed;
	flushSSIFIFO(SSI1_BASE);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	SysCtlDelay(250);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
	SysCtlDelay(250);

	//debounce YO
	g_pChangeLCD=g_changeLCD;

	//load 7 sets of 16 buttons
	for(j=0;j<7;j++)
	{
		SSIDataPut(SSI1_BASE,0);//load 16 bits of data
		SysCtlDelay(250);
	}

	//should run 7 times. We will have problems if it doesn't
	for(j=0;j<7;j++)
	{
		//turn on led if not scanning all the buttons
		if((SSIDataGetNonBlocking(SSI1_BASE,&tmp))==0)
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0xFF);
		else
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);

		scanData=(uint32_t)tmp;

		if(j==0)
		{
			//loop through all 16 bits of scanned data
			for(k=0;k<16;k++)
			{
				shiftData=15-k;
				shiftData=0x01<<shiftData;
				shiftData=(scanData & shiftData);
				//if k is in the top 8 bits, it is a chord select button
				if(k<8)
				{
					if(shiftData)
					{
						if(k==0)
						{
							if(g_chord!=FIRST)
								g_keyChange=1;
							g_chord=FIRST;
						}
						else if(k==1)
						{
							if(g_chord!=SECOND)
								g_keyChange=1;
							g_chord=SECOND;
						}
						else if(k==2)
						{
							if(g_chord!=THIRD)
								g_keyChange=1;
							g_chord=THIRD;
						}
						else if(k==3)
						{
							if(g_chord!=FOURTH)
								g_keyChange=1;
							g_chord=FOURTH;
						}
						else if(k==4)
						{
							if(g_chord!=FIFTH)
								g_keyChange=1;
							g_chord=FIFTH;
						}
						else if(k==5)
						{
							if(g_chord!=SIXTH)
								g_keyChange=1;
							g_chord=SIXTH;
						}
						else if(k==6)
						{
							if(g_chord!=SEVENTH)
								g_keyChange=1;
							g_chord=SEVENTH;
						}
					}
				}
				else
				{
					if(shiftData)
					{
						if(k==8)
						{
							if(g_key!=Cs)
								g_keyChange=1;
							g_key=Cs;
						}
						else if(k==9)
						{
							if(g_key!=Ds)
								g_keyChange=1;
							g_key=Ds;
						}
						else if(k==10)
						{
							majorPressed=1;
							g_changeLCD=0;

							if(g_keyType!=MAJOR)
								g_keyChange=1;
							g_keyType=MAJOR;
						}
						else if(k==11)
						{
							if(majorPressed)
								g_changeLCD=1;
							else
							{
								if(g_keyType!=MINOR)
									g_keyChange=1;
								g_keyType=MINOR;
							}
						}
						else if(k==12)
						{
							if(g_key!=As)
								g_keyChange=1;
							g_key=As;
						}
						else if(k==13)
						{
							if(g_key!=Fs)
								g_keyChange=1;
							g_key=Fs;
						}
						else if(k==14)
						{
							if(g_key!=Gs)
								g_keyChange=1;
							g_key=Gs;
						}
					}
				}
			}
		}
		else if(j==1)
		{
			baseAcciOff=28;
			for(k=0;k<16;k++)
			{
				shiftData=15-k;
				shiftData=0x01<<shiftData;
				shiftData=(scanData & shiftData);

				//if we are now scanning note buttons, change scale/octaves if needed
				if(k==8 && g_keyChange)
				{
					sendAllNotesOff();
					genScale();
					chordSelect();
				}

				if(k<8)
				{
					if(shiftData)
					{
						if(k==0)
						{
							if(g_key!=C)
								g_keyChange=1;
							g_key=C;
						}
						else if(k==1)
						{
							if(g_key!=D)
								g_keyChange=1;
							g_key=D;
						}
						else if(k==2)
						{
							if(g_key!=E)
								g_keyChange=1;
							g_key=E;
						}
						else if(k==3)
						{
							if(g_key!=F)
								g_keyChange=1;
							g_key=F;
						}
						else if(k==4)
						{
							if(g_key!=G)
								g_keyChange=1;
							g_key=G;
						}
						else if(k==5)
						{
							if(g_key!=A)
								g_keyChange=1;
							g_key=A;
						}
						else if(k==6)
						{
							if(g_key!=B)
								g_keyChange=1;
							g_key=B;
						}
						g_keyType=MAJOR;
						g_chord=FIRST;
					}
				}
				else
				{
					acciOff=baseAcciOff+(k-8);
					if(shiftData)
					{
						if(g_octavesAcci[acciOff].state==OFF &&
								g_octavesAcci[acciOff].midi>0)
						{
							g_octavesAcci[acciOff].state=ON;
							SENDNOTEON_ALL(g_octavesAcci[acciOff].midi,0x7F);
						}
					}
					else
					{
						if(g_octavesAcci[acciOff].state==ON &&
								g_octavesAcci[acciOff].midi>0)
						{
							g_octavesAcci[acciOff].state=OFF;
							SENDNOTEOFF_ALL(g_octavesAcci[acciOff].midi,0x7F);
						}
					}
				}
			}
		}
		else
		{
			//I could figure out math to do this, but I don't feel like it
			if(j==2)
			{
				baseOctOff=32;
				baseAcciOff=21;
			}
			else if(j==3)
			{
				baseOctOff=24;
				baseAcciOff=14;
			}
			else if(j==4)
			{
				baseOctOff=16;
				baseAcciOff=7;
			}
			else if(j==5)
			{
				baseOctOff=8;
				baseAcciOff=0;
			}
			else
			{
				baseOctOff=0;
				baseAcciOff=0;
			}

			for(k=0;k<16;k++)
			{
				shiftData=15-k;
				shiftData=0x01<<shiftData;
				shiftData=(scanData & shiftData);

				if(k<8)
				{
					octOff=baseOctOff+k;
					if(shiftData)
					{
						if(g_octaves[octOff].state==OFF)
						{
							g_octaves[octOff].state=ON;
							SENDNOTEON_ALL(g_octaves[octOff].midi,0x7F);
						}
					}
					else
					{
						if(g_octaves[octOff].state==ON)
						{
							g_octaves[octOff].state=OFF;
							SENDNOTEOFF_ALL(g_octaves[octOff].midi,0x7F);
						}
					}
				}
				else if(j!=6 && k!=15)
				{
					acciOff=baseAcciOff+(k-8);
					if(shiftData)
					{
						if(g_octavesAcci[acciOff].state==OFF &&
								g_octavesAcci[acciOff].midi>0)
						{
							g_octavesAcci[acciOff].state=ON;
							SENDNOTEON_ALL(g_octavesAcci[acciOff].midi,0x7F);
						}
					}
					else
					{
						if(g_octavesAcci[acciOff].state==ON &&
								g_octavesAcci[acciOff].midi>0)
						{
							g_octavesAcci[acciOff].state=OFF;
							SENDNOTEOFF_ALL(g_octavesAcci[acciOff].midi,0x7F);
						}
					}
				}
			}
		}
	}
}
