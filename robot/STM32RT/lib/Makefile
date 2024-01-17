
# ST-Link configuration
STLINK_ROOT=stlink-1.5.1/build/Release

# sources
SOURCES = $(wildcard ex*.c)
PROGRAMS = $(SOURCES:.c=.elf)
OBJECTS = $(SOURCES:.c=.o)

# compiler configuration
ARCH=arm-none-eabi-
CC=$(ARCH)gcc
OBJCOPY=$(ARCH)objcopy
CFLAGS += -g3 -Wall -O2

CFLAGS += \
	-T../scripts/link.ld \
	-mfloat-abi=hard \
	-mlittle-endian \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16 \
	-mthumb \
	-I ../include
LDFLAGS = \
	-static \
	-nostdinc -nostdlib \
	-nostartfiles


# rules
all: $(PROGRAMS)

$(APP).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(PROGRAMS) $(OBJECTS)

ex%.elf: startup.o ex%.o tinyprintf.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

