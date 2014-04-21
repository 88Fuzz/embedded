#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"
#include "util.h"
#include "ra8875.h"
#include "comm.h"


slider g_sld1, g_sld2;
xyGrid g_xy;
text g_txtKey;
text g_txtKeyType;
text g_txtChord;
text g_txtWaveform;
text g_txtFilter;

/*
 * given x,y,x label, and y label will return new xyGrid
 */
xyGrid xyGrid_get(uint16_t x, uint16_t y, char* xStr, uint8_t* xinitLevel, uint8_t xlevelID,
				char* yStr, uint8_t* yinitLevel, uint8_t ylevelID)
{
	xyGrid grid;
	grid.xlevel=xinitLevel;
	grid.ylevel=yinitLevel;
	grid.xlevelID=xlevelID;
	grid.ylevelID=ylevelID;
	grid.bgnd=rect_get(x,y,230,228,BLACK_16BIT);
	grid.leftEdge=rect_get(x-9,y,8,237,g_backgroundColor);
	grid.rightEdge=rect_get(x+grid.bgnd.width+1,y,9,237,g_backgroundColor);
	grid.bottomEdge=rect_get(x,y+grid.bgnd.height+1,grid.bgnd.width,12,g_backgroundColor);
	grid.dot=circle_get(x,y+grid.bgnd.height,5,WHITE_16BIT);
	grid.xlabel=text_get(x, y-16, xStr, grid.bgnd.width/2,
			15, WHITE_16BIT, g_backgroundColor);
	grid.ylabel=text_get(x+grid.bgnd.width/2, y-16, yStr, grid.bgnd.width/2,
			15, WHITE_16BIT, g_backgroundColor);

	xyGrid_updateDotPos(&grid);
	return grid;
}
/*
 * check if xyGrid is touched
 * return true if touch is in grid
 * return false if not touched
 */
bool xyGrid_isTouched(xyGrid *grid, uint16_t tx, uint16_t ty)
{
	if(tx>grid->bgnd.x && tx<(grid->bgnd.x+grid->bgnd.width))//x touch is on grid
	{
		if(ty>grid->bgnd.y && ty<(grid->bgnd.y+grid->bgnd.height))
		{
			return true;
		}
	}
	return false;
}

/*
 * updates the dot in xyGrid based on the level fields
 */
void xyGrid_updateDotPos(xyGrid *grid)
{
	grid->dot.x=(grid->bgnd.width**(grid->xlevel)/100)+grid->bgnd.x;//-grid->dot.radius;
	//subtract from 100 because level 0 should be bottom of xy grid
	//the bottom of the xygrid is where y is greater
	grid->dot.y=(grid->bgnd.height*(100-*(grid->ylevel))/100)+grid->bgnd.y;//-grid->dot.radius;
}

/*
 * updates the the x and y levels based on the touch tx and ty
 */
void xyGrid_updateLevels(xyGrid *grid, uint16_t tx, uint16_t ty)
{
	*grid->xlevel=(tx-grid->bgnd.x)*100/grid->bgnd.width;
	//subtract from 100 because level 0 should be bottom of xy grid
	//the bottom of the xygrid is where y is greater
	*grid->ylevel=100-((ty-grid->bgnd.y)*100/grid->bgnd.height);
}

/*
 * updates both the xy levels and dot
 */
void xyGrid_updateDotLevels(xyGrid *grid, uint16_t tx, uint16_t ty)
{
	xyGrid_updateLevels(grid, tx,ty);
	xyGrid_updateDotPos(grid);
}

void xyGrid_draw(xyGrid *grid)
{
	MYstrcpy(grid->xPrintLabel,grid->xlabel.label);
	MYstrcpy(grid->yPrintLabel,grid->ylabel.label);
	strAppendInt(grid->xPrintLabel,*grid->xlevel);
	strAppendInt(grid->yPrintLabel,*grid->ylevel);

	rect_draw(&grid->bgnd);
	circle_draw(&grid->dot);
	rect_draw(&grid->leftEdge);
	rect_draw(&grid->rightEdge);
	rect_draw(&grid->bottomEdge);
	text_drawVal(&grid->xlabel, grid->xPrintLabel);
	text_drawVal(&grid->ylabel, grid->yPrintLabel);
}

/*
 * initializes a new slider at point (x,y) with label lbl
 *
 * returns initialized slider
 */
slider slider_get(uint16_t x, uint16_t y, char *str, uint8_t *initLevel, uint8_t levelID)
{
	slider sldr;
	sldr.level=initLevel;
	sldr.levelID=levelID;
	sldr.bgnd=rect_get(x,y,100,228,GREEN_16BIT);
	sldr.label=text_get(x,y-15,str,sldr.bgnd.width,14,WHITE_16BIT,g_backgroundColor);
	sldr.track=rect_get(sldr.bgnd.x+45,sldr.bgnd.y+15,10,200, RED_16BIT);
	sldr.slide=rect_get(sldr.bgnd.x+10,0,80,30,BLACK_16BIT);
	sldr.bottomEdge=rect_get(x,y+sldr.bgnd.height,sldr.bgnd.width,12,g_backgroundColor);
	slider_updateSlidePos(&sldr);
	return sldr;
}

/*
 * updates the slide rectangle in slider type based on the
 * level field
 */
void slider_updateSlidePos(slider *sldr)
{
	sldr->slide.y=(sldr->track.height*(100-*(sldr->level))/100)+sldr->track.y-sldr->slide.height/2;
}

/*
 * updates the level varible based on the touch input ty
 */
void slider_updateLevel(slider *sldr, uint16_t ty)
{
	//touch is higher than the track(assume 100%)
	if(ty<sldr->track.y)
		*(sldr->level)=0;
	else if(ty>sldr->track.y+sldr->track.height)
		*(sldr->level)=100;
	else
		*(sldr->level)=(ty-sldr->track.y)*100/sldr->track.height;

	*(sldr->level)=100-*(sldr->level);
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
	strAppendInt(sldr->printLabel,*(sldr->level));

	rect_draw(&sldr->bgnd);
	rect_draw(&sldr->track);
	rect_draw(&sldr->slide);
	rect_draw(&sldr->bottomEdge);
	text_drawVal(&sldr->label, sldr->printLabel);
}


/*
 * returns a rect given x, y, width, height, and color
 */
rect rect_get(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	rect ret;
	ret.x=x;
	ret.y=y;
	ret.width=width;
	ret.height=height;
	ret.color=color;
	return ret;
}

/*
 * draws a rectangle on the LCD
 */
void rect_draw(rect *rct)
{
	fillRect(rct->x,rct->y,rct->width,rct->height,rct->color);
}

/*
 * returns a label given x, y, text, and color
 */
text text_get(uint16_t x, uint16_t y, char *str, uint16_t bgWidth,
		uint16_t bgHeight, uint16_t fntColor, uint16_t bgColor)
{
	text lbl;
	lbl.x=x;
	lbl.y=y;
	MYstrcpy(lbl.label,str);
	lbl.color=fntColor;
	lbl.bgColor=bgColor;
	lbl.bgWidth=bgWidth;
	lbl.bgHeight=bgHeight;
	return lbl;
}

/*
 * draws the text on screen with a new value appended to the end
 */
void text_drawVal(text *txt, char *str)
{
	fillRect(txt->x,txt->y,txt->bgWidth,txt->bgHeight,txt->bgColor);
	textMode();
	textEnlarge(0);
	textSetCursor(txt->x,txt->y);
	textTransparent(txt->color);
	textWrite(str);
	graphicsMode();
}

/*
 * draws text on screen
 */
void text_draw(text *txt)
{
	fillRect(txt->x,txt->y,txt->bgWidth,txt->bgHeight,txt->bgColor);
	textMode();
	textEnlarge(0);
	textSetCursor(txt->x,txt->y);
	textTransparent(txt->color);
	textWrite(txt->label);
	graphicsMode();
}

/*
 * returns a new circle given x, y, radius, and color
 */
circle circle_get(uint16_t x,uint16_t y,uint16_t radius,uint16_t color)
{
	circle circ;
	circ.x=x;
	circ.y=y;
	circ.radius=radius;
	circ.color=color;
	return circ;
}

/*
 * draws circle on LCD
 */
void circle_draw(circle *circ)
{
	fillCircle(circ->x,circ->y,circ->radius,circ->color);
}
