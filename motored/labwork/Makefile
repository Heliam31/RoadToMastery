
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
#ASFLAGS += -g3

CFLAGS += \
	-T$(STM32_ROOT)/scripts/link.ld \
	-mthumb \
	-mfloat-abi=hard \
	-mlittle-endian \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16 \
	-I $(STM32_ROOT)/include\
	-g3


LDFLAGS = \
	-static \
	-nostdinc -nostdlib \
	-nostartfiles
ADD_OBJECTS = \
	$(STM32_ROOT)/lib/startup.o \
	$(STM32_ROOT)/lib/tinyprintf.o \
	$(STM32_ROOT)/lib/asm.o
SOURCES = $(wildcard ex*.c)
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


# exercises
ex%.elf: $(ADD_OBJECTS) ex%.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) -c $< -o $@

%.s: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -S

debug_ex%: ex%.elf
	$(GDB) $(GDB_FLAGS) ex$*.elf


# build distribution
DIST_FILES = \
	Makefile \
	STM32RT
DIST_NAME=labwork
DATE = $(shell date +"%y%m%d")

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


# to deliver
assign:
	@tar cvfz assign-$(DATE).tgz ex*.c > /dev/null
	@echo "Assignment in assign-$(DATE).tgz"

ASSIGN1=1 2 3 4 5
assign1:
	@tar cvfz assign1-$(DATE).tgz $(patsubst %,ex%.c,$(ASSIGN1)) > /dev/null
	@echo "Assignment in assign1-$(DATE).tgz"	

ASSIGN2=6 7 8 9 10 11
assign2:
	@tar cvfz assign2-$(DATE).tgz $(patsubst %,ex%.c,$(ASSIGN2)) > /dev/null
	@echo "Assignment in assign2-$(DATE).tgz"	

ASSIGN3=12 13 14
assign3:
	@tar cvfz assign3-$(DATE).tgz $(patsubst %,ex%.c,$(ASSIGN3)) > /dev/null
	@echo "Assignment in assign3-$(DATE).tgz"	

ASSIGN4=15
assign4:
	@tar cvfz assign4-$(DATE).tgz $(patsubst %,ex%.c,$(ASSIGN4)) > /dev/null
	@echo "Assignment in assign4-$(DATE).tgz"	




