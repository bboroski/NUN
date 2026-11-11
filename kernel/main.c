#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#define COM1 0x3f8

__attribute__((used, section(".limine_requests")))
static volatile uint64_t base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests_start_marker")))
static volatile uint64_t requests_start[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end_marker")))
static volatile uint64_t requests_end[] = LIMINE_REQUESTS_END_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = NULL
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memory_map_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0,
    .response = NULL
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_bootloader_info_request bootloader_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST_ID,
    .revision = 0,
    .response = NULL
};

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xc7);
    outb(COM1 + 4, 0x0b);
}

static void serial_putc(char value) {
    for (uint32_t retry = 0; retry < 100000; ++retry) {
        if ((inb(COM1 + 5) & 0x20u) != 0) {
            outb(COM1, (uint8_t)value);
            return;
        }
    }
}

static void serial_write(const char *text) {
    while (*text != '\0') {
        if (*text == '\n') {
            serial_putc('\r');
        }
        serial_putc(*text++);
    }
}

static void serial_u64(uint64_t value) {
    char digits[21];
    size_t count = 0;

    if (value == 0) {
        serial_putc('0');
        return;
    }

    while (value != 0 && count < sizeof(digits)) {
        digits[count++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (count != 0) {
        serial_putc(digits[--count]);
    }
}

__attribute__((noreturn)) static void halt_forever(void) {
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

static uint32_t channel(uint8_t value, uint8_t size, uint8_t shift) {
    if (size == 0 || size > 8) {
        return 0;
    }

    const uint32_t maximum = (1u << size) - 1u;
    return (((uint32_t)value * maximum) / 255u) << shift;
}

static uint32_t rgb(const struct limine_framebuffer *fb,
                    uint8_t red, uint8_t green, uint8_t blue) {
    return channel(red, fb->red_mask_size, fb->red_mask_shift) |
           channel(green, fb->green_mask_size, fb->green_mask_shift) |
           channel(blue, fb->blue_mask_size, fb->blue_mask_shift);
}

static void draw_rect(const struct limine_framebuffer *fb,
                      uint64_t x, uint64_t y,
                      uint64_t width, uint64_t height,
                      uint32_t colour) {
    if (x >= fb->width || y >= fb->height) {
        return;
    }

    if (width > fb->width - x) {
        width = fb->width - x;
    }
    if (height > fb->height - y) {
        height = fb->height - y;
    }

    for (uint64_t row = 0; row < height; ++row) {
        volatile uint32_t *pixels =
            (volatile uint32_t *)((volatile uint8_t *)fb->address +
                                  (y + row) * fb->pitch);
        for (uint64_t column = 0; column < width; ++column) {
            pixels[x + column] = colour;
        }
    }
}

static void draw_glyph(const struct limine_framebuffer *fb,
                       uint64_t x, uint64_t y, uint64_t scale,
                       const uint8_t rows[7], uint32_t colour) {
    for (uint64_t row = 0; row < 7; ++row) {
        for (uint64_t column = 0; column < 5; ++column) {
            if ((rows[row] & (1u << (4u - column))) != 0) {
                draw_rect(fb, x + column * scale, y + row * scale,
                          scale, scale, colour);
            }
        }
    }
}

static void draw_splash(void) {
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count == 0) {
        return;
    }

    struct limine_framebuffer *fb =
        framebuffer_request.response->framebuffers[0];

    if (fb == NULL || fb->address == NULL ||
        fb->memory_model != LIMINE_FRAMEBUFFER_RGB ||
        fb->bpp != 32 || fb->pitch < fb->width * 4u) {
        return;
    }

    const uint32_t background = rgb(fb, 8, 12, 18);
    const uint32_t yellow = rgb(fb, 255, 207, 45);
    const uint32_t white = rgb(fb, 235, 240, 245);
    const uint32_t accent = rgb(fb, 38, 190, 170);

    draw_rect(fb, 0, 0, fb->width, fb->height, background);
    draw_rect(fb, 0, 0, fb->width, 8, yellow);

    uint64_t scale = fb->height / 56u;
    if (scale < 2u) {
        scale = 2u;
    }
    if (scale > 16u) {
        scale = 16u;
    }

    static const uint8_t letter_n[7] = {
        0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11
    };
    static const uint8_t letter_u[7] = {
        0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e
    };

    const uint64_t glyph_width = 5u * scale;
    const uint64_t gap = scale;
    const uint64_t logo_width = glyph_width * 3u + gap * 2u;
    const uint64_t logo_height = 7u * scale;
    const uint64_t start_x = fb->width > logo_width
                                 ? (fb->width - logo_width) / 2u
                                 : 0u;
    const uint64_t start_y = fb->height / 3u;

    draw_glyph(fb, start_x, start_y, scale, letter_n, yellow);
    draw_glyph(fb, start_x + glyph_width + gap, start_y, scale,
               letter_u, white);
    draw_glyph(fb, start_x + (glyph_width + gap) * 2u, start_y,
               scale, letter_n, yellow);

    const uint64_t bar_width = fb->width / 3u;
    const uint64_t bar_x = (fb->width - bar_width) / 2u;
    const uint64_t bar_y = start_y + logo_height + scale * 3u;
    draw_rect(fb, bar_x, bar_y, bar_width, scale / 2u + 1u, accent);
    draw_rect(fb, bar_x, bar_y, bar_width / 4u, scale / 2u + 1u, yellow);
}

__attribute__((noreturn, used, section(".text.entry")))
void _start(void) {
    serial_init();
    serial_write("\nNUN: x86_64 kernel entry reached\n");

    if (!LIMINE_BASE_REVISION_SUPPORTED(base_revision)) {
        serial_write("NUN: unsupported Limine base revision\n");
        halt_forever();
    }

    if (LIMINE_LOADED_BASE_REVISION_VALID(base_revision)) {
        serial_write("NUN: Limine base revision ");
        serial_u64(LIMINE_LOADED_BASE_REVISION(base_revision));
        serial_write("\n");
    }

    if (bootloader_info_request.response != NULL) {
        serial_write("NUN: bootloader ");
        if (bootloader_info_request.response->name != NULL) {
            serial_write(bootloader_info_request.response->name);
        }
        serial_write(" ");
        if (bootloader_info_request.response->version != NULL) {
            serial_write(bootloader_info_request.response->version);
        }
        serial_write("\n");
    } else {
        serial_write("NUN: bootloader metadata unavailable\n");
    }

    if (memory_map_request.response != NULL) {
        uint64_t usable_regions = 0;
        uint64_t total_regions = memory_map_request.response->entry_count;
        if (memory_map_request.response->entries == NULL) {
            serial_write("NUN: memory map entries unavailable\n");
            draw_splash();
            serial_write("NUN: boot path ready\n");
            halt_forever();
        }
        for (uint64_t index = 0; index < total_regions; ++index) {
            const struct limine_memmap_entry *entry =
                memory_map_request.response->entries[index];
            if (entry != NULL && entry->type == LIMINE_MEMMAP_USABLE) {
                ++usable_regions;
            }
        }
        serial_write("NUN: memory map entries ");
        serial_u64(total_regions);
        serial_write(", usable regions ");
        serial_u64(usable_regions);
        serial_write("\n");
    } else {
        serial_write("NUN: memory map unavailable\n");
    }

    draw_splash();
    serial_write("NUN: boot path ready\n");

    halt_forever();
}
