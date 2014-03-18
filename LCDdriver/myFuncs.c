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
#include "myFuncs.h"

LCDscreen ra8875;

void flushSSIFIFO(uint32_t SSIbase)
{
	uint32_t tmp, ass;
	do
	{
		tmp=SSIDataGetNonBlocking(SSIbase,&ass);
	}while(tmp!=0);
}

void mySSIDataPut(uint32_t base, uint32_t data)
{
	SSIDataPut(base, data);
	while(SSIBusy(base));
}
