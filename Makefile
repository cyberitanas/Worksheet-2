# Makefile for Tiny OS

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
KMAIN_OBJ  = $(BUILD_DIR)/kmain.o
IO_OBJ     = $(BUILD_DIR)/io.o
FB_OBJ     = $(BUILD_DIR)/framebuffer.o
KBD_OBJ    = $(BUILD_DIR)/keyboard.o
INPUT_OBJ  = $(BUILD_DIR)/input.o
PIC_OBJ    = $(BUILD_DIR)/pic.o
INTERRUPTS_OBJ = $(BUILD_DIR)/interrupts.o
INT_ASM_OBJ    = $(BUILD_DIR)/interrupt_asm.o
INT_STUBS_OBJ  = $(BUILD_DIR)/interrupt_handlers.o

OBJECTS = $(LOADER_OBJ) $(IO_OBJ) $(KMAIN_OBJ) \
          $(FB_OBJ) $(KBD_OBJ) $(INPUT_OBJ) $(PIC_OBJ) \
          $(INTERRUPTS_OBJ) $(INT_ASM_OBJ) $(INT_STUBS_OBJ)

KERNEL_ELF = kernel.elf
OS_ISO = os.iso

# Default target
all: $(OS_ISO)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create ISO directory structure
$(BOOT_DIR):
	mkdir -p $(GRUB_DIR)

# Assemble loader.asm
$(LOADER_OBJ): source/loader.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Assemble io.asm
$(IO_OBJ): source/io.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Assemble interrupt common handler
$(INT_ASM_OBJ): source/interrupt_asm.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Assemble interrupt stubs
$(INT_STUBS_OBJ): source/interrupt_handlers.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile framebuffer.c
$(FB_OBJ): drivers/framebuffer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Compile keyboard.c
$(KBD_OBJ): drivers/keyboard.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Compile input.c
$(INPUT_OBJ): drivers/input.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Compile pic.c
$(PIC_OBJ): source/pic.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Compile interrupts.c
$(INTERRUPTS_OBJ): source/interrupts.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Compile kmain.c
$(KMAIN_OBJ): source/kmain.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Link kernel
$(KERNEL_ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

# Create ISO image
$(OS_ISO): $(KERNEL_ELF) | $(BOOT_DIR)
	cp $(KERNEL_ELF) $(BOOT_DIR)/
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
	qemu-system-i386 \
		-display curses \
		-monitor telnet::45454,server,nowait \
		-serial mon:stdio \
		-boot d \
		-cdrom $(OS_ISO) \
		-m 32 \
		-d cpu \
		-D logQ.txt

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(KERNEL_ELF) $(OS_ISO) logQ.txt $(BOOT_DIR)/kernel.elf

.PHONY: all run clean
