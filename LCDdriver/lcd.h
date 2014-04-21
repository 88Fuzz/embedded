#ifndef LCD_H_
#define LCD_H_
#include <stdint.h>
#include <stdbool.h>

#define X_TOUCH_MAX 950
#define Y_TOUCH_MAX 900
#define X_TOUCH_MID X_TOUCH_MAX/2
#define Y_TOUCH_MID Y_TOUCH_MAX/2

typedef struct
{
	uint32_t /*GPIOcfg,*/ base_cs, base_rst,
		cs, rst, base_ssi, ssi_clk, ssi_cs,
		ssi_rx, ssi_tx, /*SSIcfg, SSIGPIOcfg,
		SSIcfg_clk, SSIcfg_rx, SSIcfg_tx, SSIcfg_cs,*/
		base_int, touch_int;
	uint16_t width, height;
	uint8_t textScale;
}LCDscreen;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t color;
} rect;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t radius;
	uint16_t color;
}circle;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t color;
	uint16_t bgColor;
	uint16_t bgWidth;
	uint16_t bgHeight;
	char label[14];//max 12 characters, 8 for sliders
} text;

typedef struct
{
	rect slide;
	rect track;
	rect bgnd;
	rect bottomEdge;
	uint8_t *level;//position of slider as a percent
	uint8_t levelID;
	text label;
	char printLabel[13];//same as label.label, but has ": 0 level" appended at the end
} slider;

typedef struct
{
	rect bgnd;
	rect leftEdge;
	rect rightEdge;
	rect bottomEdge;
	circle dot;
	uint8_t *xlevel;
	uint8_t xlevelID;
	uint8_t *ylevel;
	uint8_t ylevelID;
	text xlabel;
	text ylabel;
	char xPrintLabel[13];
	char yPrintLabel[13];
} xyGrid;

extern slider g_sld1, g_sld2;
extern xyGrid g_xy;
extern text g_txtKey;
extern text g_txtKeyType;
extern text g_txtChord;
extern text g_txtWaveform;
extern text g_txtFilter;


xyGrid xyGrid_get(uint16_t, uint16_t, char*, uint8_t*, uint8_t,
				char*, uint8_t*, uint8_t);
bool xyGrid_isTouched(xyGrid*, uint16_t, uint16_t);
void xyGrid_updateDotPos(xyGrid *);
void xyGrid_updateLevels(xyGrid *, uint16_t, uint16_t);
void xyGrid_updateDotLevels(xyGrid *, uint16_t, uint16_t);
void xyGrid_draw(xyGrid *);

//slider functions
slider slider_get(uint16_t x, uint16_t y, char *str, uint8_t *initLevel, uint8_t levelID);
void slider_updateSlidePos(slider *);
void slider_updateLevel(slider *, uint16_t);
void slider_updateSlideLevel(slider *, uint16_t);
bool slider_isTouched(slider *, uint16_t, uint16_t);
void slider_draw(slider *);

//rectangle functions
rect rect_get(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void rect_draw(rect *);

//circle functions
circle circle_get(uint16_t,uint16_t,uint16_t,uint16_t);
void circle_draw(circle *);

//text functions
text text_get(uint16_t, uint16_t, char *, uint16_t,
		uint16_t, uint16_t, uint16_t);
void text_drawVal(text *,char *);
void text_draw(text *);

#endif
