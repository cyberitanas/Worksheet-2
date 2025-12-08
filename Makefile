is this makefile good for the task? : # Makefile for Tiny OS

# Compiler and tools
ASM = nasm
CC = gcc
LD = ld
GENISOIMAGE = genisoimage

# Flags
ASMFLAGS = -f elf
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
LDFLAGS = -T source/link.ld -melf_i386

# Directories
BUILD_DIR = build
ISO_DIR = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

# Files
LOADER_OBJ = $(BUILD_DIR)/loader.o
KERNEL_ELF = kernel.elf
OS_ISO = os.iso

# Default target
all: $(OS_ISO)

# Create build directory
$(BUILD_DIR):
    mkdir -p $(BUILD_DIR)

# Assemble loader.asm
$(LOADER_OBJ): source/loader.asm | $(BUILD_DIR)
    $(ASM) $(ASMFLAGS) $< -o $@

# Link kernel
$(KERNEL_ELF): $(LOADER_OBJ)
    $(LD) $(LDFLAGS) $^ -o $@
    cp $(KERNEL_ELF) $(BOOT_DIR)/

# Create ISO image
$(OS_ISO): $(KERNEL_ELF)
    $(GENISOIMAGE) -R \
        -b boot/grub/stage2_eltorito \
        -no-emul-boot \
        -boot-load-size 4 \
        -A os \
        -input-charset utf8 \
        -quiet \
        -boot-info-table \
        -o $@ \
        $(ISO_DIR)

# Run in QEMU
run: $(OS_ISO)
    qemu-system-i386 -nographic -boot d -cdrom $(OS_ISO) -m 32 -d cpu -D logQ.txt

# Clean build artifacts
clean:
    rm -rf $(BUILD_DIR) $(KERNEL_ELF) $(OS_ISO) logQ.txt $(BOOT_DIR)/kernel.elf

.PHONY: all run clean