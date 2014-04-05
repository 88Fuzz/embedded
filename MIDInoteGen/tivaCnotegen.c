#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>

/*	key table:
 *	Cb:	11
 *	C:	0
 *	Cs:	1
 *	Db:	1
 *	D:	2
 *	Ds:	3
 *	Eb:	3
 *	E:	4
 *	Es:	5
 *	F:	5
 *	Fs:	6
 *	Gb:	6
 *	G:	7
 *	Gs:	8
 *	Ab:	8
 *	A:	9
 *	As:	10
 *	Bb:	10
 *	B:	11
 *	Bs:	0
 */
#define NUMOCTAVES 5
#define SCALESIZE 7
#define SCALEACCISIZE 7
#define SCALEMAX NUMOCTAVES*8
#define SCALEACCIMAX NUMOCTAVES*7
#define LOWESTNOTE 36
#define NOTEON 0x90
#define NOTEOFF 0x80

#define NOTEONMSG(note,vel) outchar(NOTEON); \
  outchar(note); \
  outchar(vel);
        
#define NOTEOFFMSG(note,vel) outchar(NOTEOFF); \
  outchar(note); \
  outchar(vel);

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

note scale[SCALESIZE];
note scaleAcci[SCALEACCISIZE];

note octaves[SCALEMAX];
note octavesAcci[SCALEACCIMAX];

keyType_t keyType;
key_t key;
chord_t chord;

void genScale();
void chordSelect();
void printScale();

int main()
{
  #ifdef D_KEY
  key=C;
  #else
  key=A;
  #endif

  key=B;//HIGHEST SCALE POSSIBLE
  
  #ifdef D_MAJOR
  keyType=MAJOR;
  #else
  keyType=MINOR;
  #endif

  keyType=MAJOR;//doesn't matter here

  #ifdef D_FIRST
  chord=FIRST;
  #else
  chord=FIFTH;
  #endif

  chord=SEVENTH;

  genScale();
  chordSelect();
  printScale();
  return 0;
}

void printScale()
{
  int i,j;
  for(j=0;j<5;j++)
  {
    printf("Octave %d:\n",j+1);
    for(i=0;i<8;i++)
    {
      printf("%d: %d\n",i,octaves[8*j+i].midi);
      if(i!=7)
        printf("\t%da: %d\n",i,octavesAcci[8*j+i].midi);
    }
  }
/*  for(i=0;i<SCALEMAX;i++)
  {
    printf("%d: %d\n",i,octaves[i].midi);
  }
  printf("Small buttons:\n");
  for(i=0;i<SCALEACCIMAX;i++)
  {
    printf("%d: %d\n",i,octaves[i].midi);
  }*/
}

void genScale()
{
  uint8_t tmp=key;
	if(keyType==MAJOR)//major key
	{
		scale[0].midi=LOWESTNOTE+tmp;
    tmp+=1;

    scaleAcci[0].midi=LOWESTNOTE+tmp;
		tmp+=1;

  	scale[1].midi=LOWESTNOTE+tmp;
    tmp+=1;

    scaleAcci[1].midi=LOWESTNOTE+tmp;
		tmp+=1;

  	scale[2].midi=LOWESTNOTE+tmp;

    scaleAcci[2].midi=-1;
	  tmp+=1;

  	scale[3].midi=LOWESTNOTE+tmp;
    tmp+=1;

    scaleAcci[3].midi=LOWESTNOTE+tmp;
  	tmp+=1;

	  scale[4].midi=LOWESTNOTE+tmp;
    tmp+=1;
    
    scaleAcci[4].midi=LOWESTNOTE+tmp;
  	tmp+=1;

  	scale[5].midi=LOWESTNOTE+tmp;
    tmp+=1;

    scaleAcci[5].midi=LOWESTNOTE+tmp;
  	tmp+=1;

	  scale[6].midi=LOWESTNOTE+tmp;
	  tmp+=1;

    scaleAcci[6].midi=-1;
//	  scale[7].midi=LOWESTNOTE+tmp;
	}
	else //minor key
	{
		scale[0].midi=LOWESTNOTE+tmp;
    tmp++;
    scaleAcci[0].midi=LOWESTNOTE+tmp;
    tmp++;

		scale[1].midi=LOWESTNOTE+tmp;
    scaleAcci[1].midi=-1;
    tmp++;

		scale[2].midi=LOWESTNOTE+tmp;
    tmp++;
    scaleAcci[2].midi=LOWESTNOTE+tmp;
    tmp++;

		scale[3].midi=LOWESTNOTE+tmp;
    tmp++;
    scaleAcci[3].midi=LOWESTNOTE+tmp;
    tmp++;

		scale[4].midi=LOWESTNOTE+tmp;
    scaleAcci[4].midi=-1;
		tmp++;

		scale[5].midi=LOWESTNOTE+tmp;
    tmp++;
    scaleAcci[5].midi=LOWESTNOTE+tmp;
    tmp++;

		scale[6].midi=LOWESTNOTE+tmp;
    tmp++;
    scaleAcci[6].midi=LOWESTNOTE+tmp;
    tmp++;
//		scale[7].midi=LOWESTNOTE+tmp;
	}
}

void chordSelect()
{
		uint8_t j=0;
	  uint8_t octOff=0; //octave offset
	  uint8_t chordOff=chord; //chord offset chord-1
	  uint8_t balanceOff=0; //balance offset
    uint8_t idx, cnt=0, button8Off=0;//button8Off is the offset for the 8th button in each row
    uint8_t tmpOctOff,tmpBalanceOff;
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
        /*tmpOctOff=octOff;
        tmpBalanceOff=balanceOff;
        if(idx==7)
        {
          tmpOctOff++;
          tmpBalanceOff+=8;
        }
        idx=j+1+chordOff-tmpBalanceOff;*/
        octaves[j].octave=octOff;
        octaves[j].midi=scale[idx].midi+(12*octaves[j].octave);
        octaves[j].state=OFF;
        button8Off++;
        cnt=-1;
      }
      else
      {
	      octaves[j].octave=octOff;
	      octaves[j].midi=scale[idx].midi+(12*octaves[j].octave);
	      octaves[j].state=OFF;

        if(scaleAcci[idx].midi<0)
        {
          octavesAcci[j].octave=octOff;
          octavesAcci[j].midi=-1;
          octavesAcci[j].state=OFF;
        }
        else
        {
          octavesAcci[j].octave=octOff;
          octavesAcci[j].midi=scaleAcci[idx].midi+(12*octaves[j].octave);
          octavesAcci[j].state=OFF;
        }
      }
      cnt++;
	  }
}
