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

LCDscreen ra8875;
uint16_t g_backgroundColor=ORANGE_16BIT;
uint8_t g_updateKeyInfo;
text g_txtKey;
text g_txtKeyType;
text g_txtChord;

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


void scanButtons()
{
	uint32_t tmp;
	uint16_t scanData, shiftData;
	uint8_t j,k,keyChange=0,acciOff,baseAcciOff,baseOctOff,octOff;
	flushSSIFIFO(SSI1_BASE);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	SysCtlDelay(250);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
	SysCtlDelay(250);

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
							g_chord=FIRST;
						else if(k==1)
							g_chord=SECOND;
						else if(k==2)
							g_chord=THIRD;
						else if(k==3)
							g_chord=FOURTH;
						else if(k==4)
							g_chord=FIFTH;
						else if(k==5)
							g_chord=SIXTH;
						else if(k==6)
							g_chord=SEVENTH;
						//set the flag that says we need to re-generate our chord/key
						keyChange=1;
					}
				}
				else
				{
					if(shiftData)
					{
						if(k==8)
							g_key=Cs;
						else if(k==9)
							g_key=Ds;
						else if(k==10)
							g_keyType=MINOR;
						else if(k==11)
							g_keyType=MAJOR;
						else if(k==12)
							g_key=As;
						else if(k==13)
							g_key=Fs;
						else if(k==14)
							g_key=Gs;
						keyChange=1;
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
				if(k==8 && keyChange)
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
							g_key=C;
						else if(k==1)
							g_key=D;
						else if(k==2)
							g_key=E;
						else if(k==3)
							g_key=F;
						else if(k==4)
							g_key=G;
						else if(k==5)
							g_key=A;
						else if(k==6)
							g_key=B;
						keyChange=1;
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
		MYstrcpy(g_txtKey.label,"Key: Cs");
	else if(g_key==2)
			MYstrcpy(g_txtKey.label,"Key: D");
	else if(g_key==3)
			MYstrcpy(g_txtKey.label,"Key: Ds");
	else if(g_key==4)
			MYstrcpy(g_txtKey.label,"Key: E");
	else if(g_key==5)
			MYstrcpy(g_txtKey.label,"Key: F");
	else if(g_key==6)
			MYstrcpy(g_txtKey.label,"Key: Fs");
	else if(g_key==7)
			MYstrcpy(g_txtKey.label,"Key: G");
	else if(g_key==8)
			MYstrcpy(g_txtKey.label,"Key: Gs");
	else if(g_key==9)
			MYstrcpy(g_txtKey.label,"Key: A");
	else if(g_key==10)
			MYstrcpy(g_txtKey.label,"Key: As");
	else if(g_key==11)
			MYstrcpy(g_txtKey.label,"Key: B");
}
