CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
CPPFLAGS ?= -Iinclude
BUILD_DIR ?= build

.PHONY: all check test clean

all: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_boot_info: src/boot_info.c tests/test_boot_info.c include/nun/boot_info.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) src/boot_info.c tests/test_boot_info.c -o $@

check:
	$(CC) $(CPPFLAGS) $(CFLAGS) -fsyntax-only src/boot_info.c tests/test_boot_info.c

test: $(BUILD_DIR)/test_boot_info
	./$(BUILD_DIR)/test_boot_info

clean:
	rm -rf $(BUILD_DIR)
