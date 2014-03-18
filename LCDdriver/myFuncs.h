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

extern LCDscreen ra8875;

void flushSSIFIFO(uint32_t);
void mySSIDataPut(uint32_t base, uint32_t data);





#endif /* MYFUNCS_H_ */
