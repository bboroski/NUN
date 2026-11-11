CC ?= gcc
LD ?= ld
QEMU ?= qemu-system-x86_64
GRUB_FILE ?= grub-file
GRUB_MKRESCUE ?= grub-mkrescue
TIMEOUT ?= timeout
BUILD := build
ISO_ROOT := $(BUILD)/iso
KERNEL := $(BUILD)/nun.elf
ISO := $(BUILD)/nun.iso

CFLAGS := -std=gnu11 -O2 -g -Wall -Wextra -Werror -Wpedantic \
          -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
          -fno-asynchronous-unwind-tables -fno-unwind-tables \
          -m64 -mno-red-zone -mcmodel=small -mgeneral-regs-only \
          -Iinclude
ASFLAGS := -g -ffreestanding -fno-pic -fno-pie -m64 -Iinclude
LDFLAGS := -m elf_x86_64 -nostdlib --build-id=none -z max-page-size=0x1000 \
           -T arch/x86_64/linker.ld

C_SOURCES := kernel/main.c kernel/serial.c kernel/memory.c kernel/interrupts.c
ASM_SOURCES := arch/x86_64/boot.S arch/x86_64/interrupts.S
OBJECTS := $(patsubst %.c,$(BUILD)/%.o,$(C_SOURCES)) \
           $(patsubst %.S,$(BUILD)/%.o,$(ASM_SOURCES))

.PHONY: all kernel iso qemu-test clean
all: iso
kernel: $(KERNEL)
iso: $(ISO)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS) arch/x86_64/linker.ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	$(GRUB_FILE) --is-x86-multiboot2 $@

$(ISO): $(KERNEL) boot/grub/grub.cfg
	mkdir -p $(ISO_ROOT)/boot/grub
	cp $(KERNEL) $(ISO_ROOT)/boot/nun.elf
	cp boot/grub/grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $@ $(ISO_ROOT) >/dev/null 2>&1

qemu-test: $(ISO)
	@set -eu; \
	  mkdir -p $(BUILD); \
	  status=0; \
	  $(TIMEOUT) 8s $(QEMU) -machine q35 -cpu qemu64 -m 512M \
	    -cdrom $(ISO) -boot d -display none -monitor none \
	    -serial file:$(BUILD)/qemu-serial.log -no-reboot -no-shutdown || status=$$?; \
	  if [ "$$status" -ne 0 ] && [ "$$status" -ne 124 ]; then \
	    cat $(BUILD)/qemu-serial.log; exit "$$status"; \
	  fi; \
	  cat $(BUILD)/qemu-serial.log; \
	  grep -Fq "NUN Kernel Ready" $(BUILD)/qemu-serial.log; \
	  grep -Fq "[MM] allocated and zeroed frame" $(BUILD)/qemu-serial.log; \
	  grep -Fq "[IRQ] PIT timer interrupt observed" $(BUILD)/qemu-serial.log

clean:
	rm -rf $(BUILD)
