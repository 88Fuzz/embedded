#ifndef COMM_H_
#define COMM_H_

//MIDI message defines
#define NOTEON 0x90
#define NOTEOFF 0x80

//MICRO commands
#define MICRO_NOTEON 0x0
#define MICRO_NOTEOFF 0x01
#define MICRO_NOTEALLOFF 0x02


#define SENDNOTEON_ALL(note,vel) \
	SENDNOTEON_MIDI(note,vel); \
	SENDNOTEON_MICRO(note);

#define SENDNOTEOFF_ALL(note,vel) \
	SENDNOTEOFF_MIDI(note,vel); \
	SENDNOTEOFF_MICRO(note);

#define SENDNOTEON_MIDI(note,vel) \
	UARTCharPutNonBlocking(UART0_BASE, NOTEON); \
	UARTCharPutNonBlocking(UART0_BASE,note); \
	UARTCharPutNonBlocking(UART0_BASE, vel);

#define SENDNOTEOFF_MIDI(note,vel) \
	UARTCharPutNonBlocking(UART0_BASE, NOTEOFF); \
	UARTCharPutNonBlocking(UART0_BASE, note); \
	UARTCharPutNonBlocking(UART0_BASE, vel);

#define SENDNOTEON_MICRO(note) \
		SSIDataPut(SSI0_BASE,(note-LOWESTNOTE));

#define SENDNOTEOFF_MICRO(note) \
		SSIDataPut(SSI0_BASE,((MICRO_NOTEOFF<<12)+(note-LOWESTNOTE)));

#define SENDALLNOTEOFF_MICRO() \
		SSIDataPut(SSI0_BASE,(MICRO_NOTEALLOFF<<12));



#endif /* COMM_H_ */
