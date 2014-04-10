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

LCDscreen ra8875;
uint16_t g_backgroundColor=ORANGE_16BIT;
text txtScale;
text txtScaleType;

/*
 * empties the SSI FIFO of SSIbase
 */
void flushSSIFIFO(uint32_t SSIbase)
{
	uint32_t tmp, ass;
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
