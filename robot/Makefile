
# configuration
LOG = stm32f4.log
GDB = gdb-multiarch
STM32_ROOT = STM32RT
GDB = gdb
#GDB = gdb-multiarch
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
	-nostartfiles
ADD_OBJECTS = \
	$(STM32_ROOT)/lib/startup.o \
	$(STM32_ROOT)/lib/tinyprintf.o \
	$(STM32_ROOT)/lib/asm.o

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
ELVES = $(SOURCES:.c=.elf)

# rules
all: $(ELVES)

clean:
	rm -f $(LOG)
	rm $(ELVES)

openocd:
	-rm $(LOG)
	openocd -f $(STM32_ROOT)/scripts/openocd.cfg

debug:
	gdb-multiarch src/$(APP).elf

log:
	tail -f $(LOG)

%.elf: $(ADD_OBJECTS) %.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) -c $< -o $@

%.s: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -S

debug_%: %.elf
	$(GDB) $(GDB_FLAGS) $*.elf
