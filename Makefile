

# Prefix for the arm-eabi-none toolchain.
COMPILER = arm-none-eabi

# Microcontroller properties.
PART=TM4C123GH6PM
CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp

# Tivaware path
TIVAWARE_PATH=~/ti/tivaware
#TIVAWARE_PATH = $(shell test -d /home/gsantha/ti/tivaware && echo /home/gsantha/ti/tivaware || echo /home/sg/ti/tivaware )

OUTDIR = build

# Program name definition for ARM GNU C compiler.
CC      = ${COMPILER}-gcc
# Program name definition for ARM GNU Linker.
LD      = ${COMPILER}-ld
# Program name definition for ARM GNU Object copy.
CP      = ${COMPILER}-objcopy
# Program name definition for ARM GNU Object dump.
OD      = ${COMPILER}-objdump
# Program name definition for ARM GNU Debugger.
DB		= ${COMPILER}-gdb

# Aditional flags to the compiler
CFLAGS=-mthumb ${CPU} ${FPU} -O3 -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -c -g
# Library paths passed as flags.
CFLAGS+= -I ${TIVAWARE_PATH} -DPART_$(PART) -c -DTARGET_IS_BLIZZARD_RB1

# Flags for linker
LFLAGS  =--gc-sections --entry=ResetISR

# Flags for objcopy
CPFLAGS = -Obinary

# Flags for objectdump
ODFLAGS = -S

# Obtain the path to libgcc, libc.a and libm.a for linking from gcc frontend.
LIB_GCC_PATH=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}

# Uploader tool path.
PROGRAMMER=lm4flash

# On chip debugger 
OC_DEBUGGER=openocd
# Config file absolute path
CONFIG_PATH=/usr/share/openocd/scripts/boards/ek-tm4c123gxl.cfg    

#==============================================================================
#                         Project properties
#==============================================================================

# Project name (name of main file)
PROJECT_NAME = main
# Startup file name
STARTUP_FILE = startup_gcc
# Linker file name
LINKER_FILE = $(PART).ld

SRC = $(shell find . -name '[!.]*.c' -not -path "./contrib/*")
#OBJS = $(SRC:.c=.o)
OBJS = $(addprefix $(OUTDIR)/,$(notdir $(SRC:.c=.o)))
RM      = rm -f
MKDIR	= mkdir -p

# Color codes
BLUE = '\033[0;34m'
GREEN = '\033[0;32m'
YELLOW = '\033[0;33m'
NC='\033[0m'
#==============================================================================
#                      Rules to make the target
#==============================================================================

#make all rule
all:$(OBJS) $(OUTDIR)/${PROJECT_NAME}.axf $(OUTDIR)/${PROJECT_NAME}

$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	@echo
	@echo -e $(YELLOW)Compiling $<...$(NC)
	$(CC) -c $(CFLAGS) ${<} -o ${@}

$(OUTDIR)/${PROJECT_NAME}.axf: $(OBJS)
	@echo
	@echo Making driverlib
	$(MAKE) -C ${TIVAWARE_PATH}/driverlib/
	@echo
	@echo -e $(BLUE)Linking...$(NC)
	$(LD) -T $(LINKER_FILE) $(LFLAGS) -o $(OUTDIR)/${PROJECT_NAME}.axf $(OBJS) ${TIVAWARE_PATH}/driverlib/gcc/libdriver.a $(LIBM_PATH) $(LIBC_PATH) $(LIB_GCC_PATH)

$(OUTDIR)/${PROJECT_NAME}: $(OUTDIR)/${PROJECT_NAME}.axf
	@echo
	@echo -e $(GREEN)Copying...$(NC)
	$(CP) $(CPFLAGS) $(OUTDIR)/${PROJECT_NAME}.axf $(OUTDIR)/${PROJECT_NAME}.bin
	@echo
	@echo -e $(GREEN)Creating list file...$(NC)
	$(OD) $(ODFLAGS) $(OUTDIR)/${PROJECT_NAME}.axf > $(OUTDIR)/${PROJECT_NAME}.lst

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

# make clean rule
clean:
	@echo Cleaning $<...
	-$(RM) $(OUTDIR)/*
# Rule to load the project to the board (sudo may be needed if rule is note made).
install:
	@echo Loading the ${PROJECT_NAME}.bin
	${PROGRAMMER} $(OUTDIR)/${PROJECT_NAME}.bin

#   GDB:
#	target extended-remote :3333
#	monitor reset halt
#	load
#	monitor reset init

#debug:
#	@echo -e 'gdb commands:\n   target extended-remote :3333\n   monitor reset halt\n   load\n   monitor reset init\n'
#	${TERMINAL} -x ${DB} ${PROJECT_NAME}.axf
#	${OC_DEBUGGER} -f ${CONFIG_PATH}
	
#test:
#	@echo -e 'gdb commands:\n   target extended-remote :3333\n   monitor reset halt\n   load\n   monitor reset init\n'

