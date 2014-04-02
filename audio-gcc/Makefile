# Tiva C Makefile
#
# Modification of Makefile for Stellaris written by Mauro Scomparin.
# Original Copyright: (c) 2012, Mauro Scomparin
#
#==============================================================================
#           Cross compiling toolchain / tools specifications
#==============================================================================

# Prefix for the arm-eabi-none toolchain.
PREFIX_ARM = arm-none-eabi

# Microcontroller properties.
PART=TM4C123GH6PM
CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp

# Tivaware path
TIVAWARE_PATH=/home/mreut/Development/tivac/tivaware/

# Program name definition for ARM GNU C compiler.
CC      = ${PREFIX_ARM}-gcc
# Program name definition for ARM GNU Linker.
LD      = ${PREFIX_ARM}-ld
# Program name definition for ARM GNU Object copy.
CP      = ${PREFIX_ARM}-objcopy
# Program name definition for ARM GNU Object dump.
OD      = ${PREFIX_ARM}-objdump
# Program name definition for ARM GNU Debugger.
DB		= ${PREFIX_ARM}-gdb

# Optional arguments for C compiler.
CFLAGS=-mthumb ${CPU} ${FPU} -O3 -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -c -g
# Library paths passed as flags.
CFLAGS+= -I ${TIVAWARE_PATH} -DPART_$(PART) -c -DTARGET_IS_BLIZZARD_RA1

# Flags for LD
LFLAGS  = --gc-sections

# Flags for objcopy
CPFLAGS = -Obinary

# Flags for objectdump
ODFLAGS = -S

# Obtain the path to libgcc, libc.a and libm.a for linking from gcc frontend.
LIB_GCC_PATH=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}

# Uploader tool path.
FLASHER=lm4flash
# Flags for the uploader program.
FLASHER_FLAGS=

# Terminal
TERMINAL=xfce4-terminal
# On chip debugger 
OC_DEBUGGER=openocd
# Config file absolute path
CONFIG_PATH=/usr/share/openocd/scripts/tiva.cfg 


#==============================================================================
#                         Project properties
#==============================================================================

# Project name (W/O .c extension eg. "main")
PROJECT_NAME = main
# Startup file name (W/O .c extension eg. "LM4F_startup")
STARTUP_FILE = startup_gcc
# Linker file name
LINKER_FILE = ldStart.ld

SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)

#==============================================================================
#                      Rules to make the target
#==============================================================================

#make all rule
all: $(OBJS) ${PROJECT_NAME}.axf ${PROJECT_NAME}

%.o: %.c
	@echo
	@echo Compiling $<...
	$(CC) -c $(CFLAGS) ${<} -o ${@}

${PROJECT_NAME}.axf: $(OBJS)
	@echo
	@echo Making driverlib
	$(MAKE) -C ${TIVAWARE_PATH}driverlib/
	@echo
	@echo Linking...
	$(LD) -T $(LINKER_FILE) $(LFLAGS) -o ${PROJECT_NAME}.axf $(OBJS) ${TIVAWARE_PATH}driverlib/gcc/libdriver.a $(LIBM_PATH) $(LIBC_PATH) $(LIB_GCC_PATH)

${PROJECT_NAME}: ${PROJECT_NAME}.axf
	@echo
	@echo Copying...
	$(CP) $(CPFLAGS) ${PROJECT_NAME}.axf ${PROJECT_NAME}.bin
	@echo
	@echo Creating list file...
	$(OD) $(ODFLAGS) ${PROJECT_NAME}.axf > ${PROJECT_NAME}.lst

# make clean rule
clean:
	rm *.bin *.o *.d *.axf *.lst

# Rule to load the project to the board (sudo may be needed if rule is note made).
load:
	${FLASHER} ${PROJECT_NAME}.bin ${FLASHER_FLAGS}

# Make sure to run the following commands in GDB after connecting on chip debugger.
#	target extended-remote :3333
#	monitor reset halt
#	load
#	monitor reset init
#
# reference: http://kernelhacks.blogspot.com/2012/11/the-complete-tutorial-for-stellaris_23.html
debug:
	${TERMINAL} -x ${DB} ${PROJECT_NAME}.axf
	${OC_DEBUGGER} -f ${CONFIG_PATH}
