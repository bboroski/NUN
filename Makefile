SHELL := /bin/bash
.DEFAULT_GOAL := help

.PHONY: help iso build run usb clean check-tools test e2k-host e2k-host-test e2k-ecosystem-test

help:
	@printf '%s\\n' \\
	  'NUN boot targets:' \\
	  '  make iso                 Build the hybrid BIOS/UEFI ISO' \\
	  '  make run                 Boot the ISO in QEMU' \\
	  '  make usb DEVICE=/dev/sdX Write the ISO to a selected USB disk' \\
	  '  make clean               Remove generated build files' \\
	  '  make e2k-host            Build the MCST LCC E2K Linux userspace probe' \\
	  '  make e2k-host-test       Run the E2K userspace self-test' \\
	  '  make e2k-ecosystem-test  Build and test integrated C11 modules on native E2K'

iso build:
	bash scripts/build.sh

run: iso
	qemu-system-x86_64 -machine q35 -m 512M -cdrom build/NUN.iso \\
	  -serial stdio -display none -monitor none -no-reboot

usb: iso
	@test -n "$(DEVICE)" -a -n "$(DEVICE)" || { echo 'Set DEVICE=/dev/sdX (whole disk)' >&2; exit 2; }
	bash scripts/install-usb.sh build/NUN.iso "$(DEVICE)"

clean:
	rm -rf -- build

check-tools:
	@command -v gcc >/dev/null
	@command -v make >/dev/null
	@command -v qemu-system-x86_64 >/dev/null
	@command -v python3 >/dev/null

test:
	python3 -m unittest discover -s tests -v

# E2K Linux userspace build; requires the MCST LCC SDK.
e2k-host:
	bash scripts/build-e2k-host.sh

# Run only on native E2K Linux (or explicitly under qemu-e2k-user).
e2k-host-test: e2k-host
	bash scripts/test-e2k-host.sh

# Build and run the full integrated C11 component suite on native E2K Linux.
e2k-ecosystem-test:
	bash scripts/test-e2k-ecosystem.sh
