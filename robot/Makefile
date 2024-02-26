
# configuration
LOG = stm32f4.log
# GDB = gdb-multiarch
STM32_ROOT = STM32RT
GDB = gdb
# GDB = gdb-multiarch
GDB_FLAGS=-command $(STM32_ROOT)/scripts/gdbinit

# compiler configuration
ARCH=arm-none-eabi-
#ARCH=arm-linux-gnueabihf-
CC=$(ARCH)gcc
AS=$(ARCH)as
OBJCOPY=$(ARCH)objcopy
CFLAGS += -g3 -Wall -O2
# ASFLAGS += -g3

CFLAGS += \
	-T$(STM32_ROOT)/scripts/link.ld \
	-mthumb \
	-mfloat-abi=hard \
	-mlittle-endian \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16 \
	-I $(STM32_ROOT)/include \
	-g3 

LDFLAGS = \
	-static \
	-nostdinc -nostdlib \
	-nostartfiles \
	-lgcc

ADD_OBJECTS = \
	$(STM32_ROOT)/lib/startup.o \
	$(STM32_ROOT)/lib/tinyprintf.o \
	$(STM32_ROOT)/lib/asm.o

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:.c=.o)
ELF = $(SRC:.c=.elf)
EXEC=robot


# rules
all : debug_robot

clean :
	rm -rf *.o $(EXEC) $(ELF) $(LOG)

openocd:
	-rm $(LOG)
	openocd -f $(STM32_ROOT)/scripts/openocd.cfg

debug:
	gdb-multiarch src/$(APP).elf

log:
	tail -f $(LOG)

# $(EXEC): $(OBJ)
#	$(CC) -o $@ $(OBJ) $(CFLAGS) $(GTKLIBS) $(LDFLAGS)

debug_%: %.elf
	$(GDB) $(GDB_FLAGS) $*.elf

robot.elf: $(ADD_OBJECTS) $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c $(HDR)
	$(CC) -o $@ -c $< $(CFLAGS)


# build distribution
DIST_FILES= \
	Makefile \
	STM32RT
DIST_NAME=robot
DATE=$(shell date +"%y%m%d")

dist:
	if [ -e $(DIST_NAME) ]; then \
		rm -rf $(DIST_NAME); \
	fi
	mkdir $(DIST_NAME)
	cp -RL $(DIST_FILES) $(DIST_NAME)
	for f in ex*.c; do \
		autofilter.py < $$f > $(DIST_NAME)/$$f; \
	done
	cd $(DIST_NAME); make; make clean
	tar cvfz $(DIST_NAME)-$(DATE).tgz $(DIST_NAME)
