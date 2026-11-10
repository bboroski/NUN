SHELL := /usr/bin/env bash
BUILD_DIR := build
CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc

CPPFLAGS := -Iinclude -I$(BUILD_DIR)/include \
  -Iecosystem/sources/Kloz/include \
  -Iecosystem/sources/CO/include \
  -Iecosystem/sources/SaintSI/include \
  -Iecosystem/sources/ReaperCripple/include
KERNEL_CFLAGS := -std=gnu11 -O2 -g -Wall -Wextra -Werror \
  -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie \
  -fno-asynchronous-unwind-tables -fno-unwind-tables \
  -m64 -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2
KERNEL_LDFLAGS := -nostdlib -static -no-pie \
  -Wl,-T,linker/x86_64.ld -Wl,--build-id=none \
  -Wl,-z,max-page-size=0x1000 -Wl,-Map,$(BUILD_DIR)/NUN.map

KERNEL_SOURCES := \
  src/boot/requests.c \
  src/kernel/main.c \
  src/arch/x86_64/serial.c \
  src/arch/x86_64/framebuffer.c \
  src/mm/pmm.c \
  src/services/integration.c \
  ecosystem/sources/Kloz/src/arena.c \
  ecosystem/sources/Kloz/src/platform.c \
  ecosystem/sources/Kloz/src/budget.c \
  ecosystem/sources/CO/src/runtime.c \
  ecosystem/sources/SaintSI/src/health.c \
  ecosystem/sources/SaintSI/src/crc32.c \
  ecosystem/sources/ReaperCripple/src/core/reaper_runtime.c
KERNEL_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES))

.PHONY: all deps ecosystem-sources kernel check iso run clean
all: kernel

deps: $(BUILD_DIR)/include/limine.h

$(BUILD_DIR)/include/limine.h: scripts/fetch-limine-protocol.sh
	./scripts/fetch-limine-protocol.sh

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/include/limine.h
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/NUN.elf: $(KERNEL_OBJECTS) linker/x86_64.ld
	$(CC) $(KERNEL_CFLAGS) $(KERNEL_LDFLAGS) $(KERNEL_OBJECTS) -o $@

ecosystem-sources:
	git submodule update --init --recursive

kernel: deps ecosystem-sources
	$(MAKE) $(BUILD_DIR)/NUN.elf

$(BUILD_DIR)/test_pmm: tests/test_pmm.c src/mm/pmm.c $(BUILD_DIR)/include/limine.h
	mkdir -p $(BUILD_DIR)
	$(CC) -std=c11 -O2 -Wall -Wextra -Werror $(CPPFLAGS) \
	  tests/test_pmm.c src/mm/pmm.c -o $@

check: $(BUILD_DIR)/test_pmm
	$(BUILD_DIR)/test_pmm

iso: kernel
	./scripts/build-iso.sh

run: iso
	qemu-system-x86_64 -M q35 -m 512M -cdrom $(BUILD_DIR)/NUN.iso \
	  -serial stdio -no-reboot -no-shutdown

clean:
	rm -rf $(BUILD_DIR)
