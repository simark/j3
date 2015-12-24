# sources
C_SRC = $(wildcard *.c) $(wildcard ../../common/*.c)

# target MCU
MCU = atmega32
PROGRAM_MCU = m32
F_CPU = 8000000UL1

# targets
TARGET = mastercube
TARGET_OUT = $(TARGET).out
TARGET_HEX = $(TARGET).hex
TARGET_MAP = $(TARGET).map

# configuration
HEXFORMAT = ihex
OPTLEVEL = 2

# tools
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
PROG = avrdude
PROGFLAGS = -c usbasp -p $(PROGRAM_MCU) -e
RM = rm -f
AR = avr-ar

# shorts
O_FILES = $(notdir $(C_SRC:.c=.o))
D_FILES = $(O_FILES:.o=.d)

INC = -I../../common

# C flags
CFLAGS = $(INC) -mmcu=$(MCU) -O$(OPTLEVEL) -fshort-enums \
	-funsigned-bitfields -funsigned-char -Wall -Wextra -Werror -DF_CPU=$(F_CPU) $(EXTRA_CFLAGS)

# linker flags
LDFLAGS = -Wl,-Map,$(TARGET_MAP) -mmcu=$(MCU)

# phony targets
.PHONY: clean all program

# main target
all: $(TARGET_OUT)

# make output
$(TARGET_OUT): $(O_FILES)
	$(CC) $(LDFLAGS) -o $@ $(O_FILES) && \
	$(SIZE) $(TARGET_OUT)

# make binary image
$(TARGET_HEX): $(TARGET_OUT)
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@

# program target
program: $(TARGET_HEX)
	if [ -z "$(CUBE_ID)" ]; then echo "CUBE_ID not set."; exit 1; fi
	echo -n -e "\00$(CUBE_ID)" > cubeid.raw
	$(PROG) $(PROGFLAGS) -U flash:w:$(TARGET_HEX) -U eeprom:w:cubeid.raw:r

program-%:
	make program CUBE_ID=$*

# objects from C files
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: ../../common/%.c
	$(CC) $(CFLAGS) -o $(notdir $@) -c $<
	$(CC) -MM $(CFLAGS) $< > $(notdir $*).d

# clean target
clean:
	$(RM) $(TARGET_OUT) $(TARGET_HEX) $(TARGET_MAP) $(O_FILES) $(D_FILES)

-include $(O_FILES:.o=.d)
