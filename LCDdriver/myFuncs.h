#ifndef MYFUNCS_H_
#define MYFUNCS_H_

#define ONE_MILISEC 26670
#define TEN_MILISEC 266700
#define ONEHUNDRED_MILISEC 2667000
#define FIVEHUNDRED_MILISEC 13333340
#define X_TOUCH_MAX 950
#define Y_TOUCH_MAX 900
#define X_TOUCH_MID X_TOUCH_MAX/2
#define Y_TOUCH_MID Y_TOUCH_MAX/2

typedef struct
{
	uint32_t GPIOcfg, base_cs, base_rst,
		cs, rst, base_ssi, ssi_clk, ssi_cs,
		ssi_rx, ssi_tx, SSIcfg, SSIGPIOcfg,
		SSIcfg_clk, SSIcfg_rx, SSIcfg_tx, SSIcfg_cs,
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
	uint16_t color;
	char label[9];//max 8 characters
} text;

typedef struct
{
	rect slide;
	rect track;
	rect bgnd;
	uint8_t level;//position of slider as a percent
	text label;
	char printLabel[13];//same as label.label, but has ": 0 level" appended at the end
} slider;

extern LCDscreen ra8875;
extern uint16_t g_backgroundColor;

void flushSSIFIFO(uint32_t);
void mySSIDataPut(uint32_t , uint32_t);
slider slider_get(uint16_t , uint16_t , char *);
void slider_updateSlidePos(slider *);
void slider_updateLevel(slider *, uint16_t);
void slider_updateSlideLevel(slider *, uint16_t);
bool slider_isTouched(slider *, uint16_t, uint16_t);
void slider_draw(slider *);

void MYstrcpy(char *, char*);
void strAppendInt(char *, uint8_t);

#endif /* MYFUNCS_H_ */
