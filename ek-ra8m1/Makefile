# Makefile for images for ek-ra8m1

PREFIX = arm-none-eabi
CC = $(PREFIX)-gcc
OBJCOPY = $(PREFIX)-objcopy
LD := $(CC)
SRC_DIR=./src/RA8M1
BUILD_DIR=./build/$(TARGET)

ENV_INC=./inc/
SYSROOT := $(shell $(CC) --print-sysroot)

CFLAGS += \
	-O3 \
	-Wall -Wextra -Wshadow \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	--sysroot=$(SYSROOT) \
	-DDEVICE=\"ra8m1\" \
	-DARMCM85 \
	-I$(ENV_INC) \
	-I$(SRC_DIR) \
	-I$(SRC_DIR)/platform

ARCH_FLAGS += \
	-mcpu=cortex-m85


CFLAGS += \
	$(ARCH_FLAGS) \
	--specs=nano.specs


CFLAGS += $(CFLAGS_EXTRA)


ifneq (,$(filter %gcc%,$(CC)))
    CFLAGS += -Tfsp.ld --specs=nano.specs
else ifneq (,$(filter %clang%,$(CC)))
    CFLAGS += -Tfsp.lld
endif


# Linker Script Selection (based on compiler)
ifeq ($(findstring gcc,$(CC)),gcc)
	LDSCRIPT = -Tfsp.ld --specs=nano.specs
else ifeq ($(findstring clang,$(CC)),clang)
	CFLAGS += -Tfsp.lld
endif

LDFLAGS += \
	-Wl,--gc-sections \
	-L.

LDFLAGS += \
	--specs=nosys.specs \
	-Wl,--wrap=_open \
	-Wl,--wrap=_read \
	-Wl,--wrap=_write \
	-ffreestanding \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

all: $(TARGET)

HAL_SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OBJECTS_HAL = $(patsubst %.c, $(BUILD_DIR)/%.c.o, $(abspath $(HAL_SOURCES)))
TEST_COMMON_SOURCES = $(wildcard $(TEST_COMMON)/*.c)
OBJECTS_TEST_COMMON = $(patsubst %.c, $(BUILD_DIR)/%.c.o, $(abspath $(TEST_COMMON_SOURCES)))
OBJECTS_SOURCES=$(patsubst %.c, $(BUILD_DIR)/%.c.o, $(abspath $(SOURCES)))
OBJECTS_C = $(OBJECTS_SOURCES) $(OBJECTS_HAL) $(OBJECTS_TEST_COMMON)
OBJECTS_ASM = $(patsubst %.s, $(BUILD_DIR)/%.s.o, $(abspath $(ASMS)))

OBJECTS = $(OBJECTS_C) $(OBJECTS_ASM)

$(OBJECTS_C): $(BUILD_DIR)/%.o: %
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJECTS_ASM): $(BUILD_DIR)/%.o: %
	mkdir -p $(@D)
	$(CC) -x assembler-with-cpp $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	$(OBJCOPY) -S -Obinary $@ $@.bin


.PHONY: build
build: $(TARGET)

run:
	@echo "WARNING: AN555 is not supported by qemu. Skipping"

check:
	@echo "WARNING: AN555 is not supported by qemu. Skipping"

flash:
	@echo "WARNING: Target platform does not support the flash- targets. Skipping"

clean:
	rm -f *.elf *.bin
	rm -rf $(BUILD_DIR)



