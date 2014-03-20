#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "ra8875.h"
#include "myFuncs.h"

LCDscreen ra8875;
uint16_t g_backgroundColor=ORANGE_16BIT;

/*
 * empties the SSI FIFO of SSIbase
 */
void flushSSIFIFO(uint32_t SSIbase)
{
	uint32_t tmp, ass;
	do
	{
		tmp=SSIDataGetNonBlocking(SSIbase,&ass);
	}while(tmp!=0);
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
 * initializes a new slider at point (x,y) with label lbl
 *
 * returns initialized slider
 */
slider slider_get(uint16_t x, uint16_t y, char *str)
{
	slider sldr;
	sldr.bgnd.color=GREEN_16BIT;
	sldr.bgnd.x=x;
	sldr.bgnd.y=y;
	sldr.bgnd.width=100;
	sldr.bgnd.height=250;
	sldr.label.color=WHITE_16BIT;
	sldr.label.x=x;
	sldr.label.y=y-15;
	MYstrcpy(sldr.label.label,str);
	sldr.level=0;
	sldr.track.color=RED_16BIT;
	sldr.slide.color=BLACK_16BIT;
	sldr.track.width=10;
	sldr.track.height=200;
	sldr.track.x=sldr.bgnd.x+45;
	sldr.track.y=sldr.bgnd.y+25;
	sldr.slide.width=80;
	sldr.slide.height=50;
	sldr.slide.x=sldr.bgnd.x+10;
	slider_updateSlidePos(&sldr);
	return sldr;
}

/*
 * updates the slide rectangle in slider type based on the
 * level field
 */
void slider_updateSlidePos(slider *sldr)
{
	sldr->slide.y=(sldr->track.height*sldr->level/100)+sldr->track.y-sldr->slide.height/2;
}

/*
 * updates the level varible based on the touch input ty
 */
void slider_updateLevel(slider *sldr, uint16_t ty)
{
	//touch is higher than the track(assume 100%)
	if(ty<sldr->track.y)
		sldr->level=0;
	else if(ty>sldr->track.y+sldr->track.height)
		sldr->level=100;
	else
		sldr->level=(ty-sldr->track.y)*100/200;
}

/*
 * updates both the slider level and the slide rectangle
 */
void slider_updateSlideLevel(slider *sldr, uint16_t ty)
{
	slider_updateLevel(sldr,ty);
	slider_updateSlidePos(sldr);
}

/*
 * checks if touch input (x,y) is inside sldr
 *
 * returns:
 * 		true 	if (x,y) is in sldr.bgnd
 * 		false	if (x,y) is outside sldr.bgnd
 */
bool slider_isTouched(slider *sldr, uint16_t tx, uint16_t ty)
{
	if(tx>sldr->bgnd.x && tx<sldr->bgnd.x+sldr->bgnd.width)
		if(ty>sldr->bgnd.y && ty<sldr->bgnd.y+sldr->bgnd.height)
			return true;
	return false;
}

/*
 * draws the slider sldr on the touch screen
 */
void slider_draw(slider *sldr)
{
	MYstrcpy(sldr->printLabel,sldr->label.label);
	strAppendInt(sldr->printLabel,sldr->level);

	graphicsMode();
	fillRect(sldr->bgnd.x,sldr->bgnd.y,sldr->bgnd.width,sldr->bgnd.height,sldr->bgnd.color);
	fillRect(sldr->track.x,sldr->track.y,sldr->track.width,sldr->track.height,sldr->track.color);
	fillRect(sldr->slide.x,sldr->slide.y,sldr->slide.width,sldr->slide.height,sldr->slide.color);
	fillRect(sldr->label.x,sldr->label.y,sldr->bgnd.width,15,g_backgroundColor);

	textMode();
	textEnlarge(0);
	textSetCursor(sldr->label.x,sldr->label.y);
	textTransparent(sldr->label.color);
	textWrite(sldr->printLabel,15);
	graphicsMode();
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
  i=100-i;
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
