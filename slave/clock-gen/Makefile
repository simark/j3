# sources
C_SRC = clock-gen.c
H_SRC =

# target MCU
MCU = attiny24a
PROGRAM_MCU = t24
F_CPU = 8000000UL

# targets
TARGET = clock-gen
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
O_FILES = $(subst .c,.o,$(C_SRC))

# C flags
CFLAGS = -mmcu=$(MCU) -O$(OPTLEVEL) -fshort-enums \
	-funsigned-bitfields -funsigned-char -Wall -DF_CPU=$(F_CPU)

# linker flags
LDFLAGS = -Wl,-Map,$(TARGET_MAP) -mmcu=$(MCU)

# phony targets
.PHONY: clean all program

# main target
all: $(TARGET_OUT)

# make output
$(TARGET_OUT): $(O_FILES) $(H_SRC)
	$(CC) $(LDFLAGS) -o $@ $(O_FILES) && \
	$(SIZE) $(TARGET_OUT)

# make binary image
$(TARGET_HEX): $(TARGET_OUT)
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@

# program target
program: $(TARGET_HEX)
	$(PROG) $(PROGFLAGS) -U flash:w:$(TARGET_HEX)

# objects from C files
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

# clean target
clean:
	$(RM) $(TARGET_OUT) $(TARGET_HEX) $(TARGET_MAP) $(O_FILES)
