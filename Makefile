CC ?= cc
CPPFLAGS ?= -Iinclude
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror

BUILD_DIR := build
TEST_BOOT_INFO := $(BUILD_DIR)/test_boot_info

.PHONY: all test check clean

all: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_BOOT_INFO): src/boot_info.c tests/test_boot_info.c include/nun/boot_info.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) src/boot_info.c tests/test_boot_info.c -o $@

test: $(TEST_BOOT_INFO)
	./$(TEST_BOOT_INFO)

check:
	$(CC) $(CPPFLAGS) $(CFLAGS) -fsyntax-only src/boot_info.c tests/test_boot_info.c

clean:
	rm -rf $(BUILD_DIR)
