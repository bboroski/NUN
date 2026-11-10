#include "nun/framebuffer.h"

#include <limits.h>
#include <stdint.h>

static struct limine_framebuffer *nun_fb;

static uint32_t scale_channel(uint8_t value, uint8_t bits, uint8_t shift)
{
    uint32_t maximum;
    if (bits == 0u || bits > 8u || shift >= 32u || (uint32_t)bits + shift > 32u)
        return 0u;
    maximum = (UINT32_C(1) << bits) - 1u;
    return (((uint32_t)value * maximum + 127u) / 255u) << shift;
}

static uint32_t pixel_value(uint8_t red, uint8_t green, uint8_t blue)
{
    return scale_channel(red, nun_fb->red_mask_size, nun_fb->red_mask_shift) |
           scale_channel(green, nun_fb->green_mask_size, nun_fb->green_mask_shift) |
           scale_channel(blue, nun_fb->blue_mask_size, nun_fb->blue_mask_shift);
}

static void put_pixel(uint64_t x, uint64_t y, uint32_t color)
{
    volatile uint32_t *row;
    if (nun_fb == 0 || x >= nun_fb->width || y >= nun_fb->height) return;
    row = (volatile uint32_t *)((uintptr_t)nun_fb->address + y * nun_fb->pitch);
    row[x] = color;
}

static void fill_rect(uint64_t x, uint64_t y, uint64_t width,
                      uint64_t height, uint32_t color)
{
    uint64_t yy, xx;
    if (x >= nun_fb->width || y >= nun_fb->height) return;
    if (width > nun_fb->width - x) width = nun_fb->width - x;
    if (height > nun_fb->height - y) height = nun_fb->height - y;
    for (yy = 0; yy < height; ++yy)
        for (xx = 0; xx < width; ++xx)
            put_pixel(x + xx, y + yy, color);
}

static void fill_ellipse(uint64_t cx, uint64_t cy, uint64_t rx, uint64_t ry,
                         uint32_t color)
{
    int64_t x, y;
    int64_t sx = (int64_t)rx, sy = (int64_t)ry;
    if (rx == 0u || ry == 0u || rx > 2048u || ry > 2048u) return;
    for (y = -sy; y <= sy; ++y) {
        for (x = -sx; x <= sx; ++x) {
            uint64_t nx = (uint64_t)(x * x) * (uint64_t)(ry * ry);
            uint64_t ny = (uint64_t)(y * y) * (uint64_t)(rx * rx);
            uint64_t limit = (uint64_t)(rx * rx) * (uint64_t)(ry * ry);
            if (nx <= limit && ny <= limit - nx) {
                if (x < 0 && (uint64_t)(-x) > cx) continue;
                if (y < 0 && (uint64_t)(-y) > cy) continue;
                put_pixel((uint64_t)((int64_t)cx + x),
                          (uint64_t)((int64_t)cy + y), color);
            }
        }
    }
}

bool nun_framebuffer_init(const struct limine_framebuffer_response *response)
{
    uint64_t row_bytes, span;
    uintptr_t address;
    if (response == 0 || response->framebuffer_count == 0u ||
        response->framebuffers == 0 || response->framebuffers[0] == 0) {
        return false;
    }
    nun_fb = response->framebuffers[0];
    if (nun_fb->address == 0 || nun_fb->memory_model != LIMINE_FRAMEBUFFER_RGB ||
        nun_fb->bpp != 32u || nun_fb->width == 0u || nun_fb->height == 0u ||
        nun_fb->width > UINT64_MAX / 4u) {
        nun_fb = 0;
        return false;
    }

    row_bytes = nun_fb->width * 4u;
    if (nun_fb->pitch < row_bytes || nun_fb->pitch == 0u ||
        nun_fb->height - 1u > (UINT64_MAX - row_bytes) / nun_fb->pitch ||
        nun_fb->red_mask_size == 0u || nun_fb->green_mask_size == 0u ||
        nun_fb->blue_mask_size == 0u ||
        (uint32_t)nun_fb->red_mask_size + nun_fb->red_mask_shift > 32u ||
        (uint32_t)nun_fb->green_mask_size + nun_fb->green_mask_shift > 32u ||
        (uint32_t)nun_fb->blue_mask_size + nun_fb->blue_mask_shift > 32u ||
        nun_fb->red_mask_size > 8u || nun_fb->green_mask_size > 8u ||
        nun_fb->blue_mask_size > 8u) {
        nun_fb = 0;
        return false;
    }

    span = (nun_fb->height - 1u) * nun_fb->pitch + row_bytes;
    address = (uintptr_t)nun_fb->address;
    if (span == 0u || span - 1u > (uint64_t)UINTPTR_MAX ||
        address > UINTPTR_MAX - (uintptr_t)(span - 1u)) {
        nun_fb = 0;
        return false;
    }
    return true;
}

void nun_framebuffer_boot_marker(void)
{
    uint32_t graphite, panel, yellow, dark;
    uint64_t cx, cy;
    if (nun_fb == 0) return;
    graphite = pixel_value(13u, 13u, 13u);
    panel = pixel_value(26u, 26u, 26u);
    yellow = pixel_value(255u, 214u, 0u);
    dark = pixel_value(13u, 13u, 13u);

    fill_rect(0u, 0u, nun_fb->width, nun_fb->height, graphite);
    fill_rect(0u, 0u, nun_fb->width, 8u, yellow);
    fill_rect(0u, nun_fb->height - (nun_fb->height > 8u ? 8u : nun_fb->height),
              nun_fb->width, 8u, yellow);
    cx = nun_fb->width / 2u;
    cy = nun_fb->height / 2u;
    fill_ellipse(cx, cy + 24u, 120u, 72u, yellow);
    fill_ellipse(cx + 66u, cy - 28u, 56u, 56u, yellow);
    fill_ellipse(cx + 84u, cy - 44u, 7u, 7u, dark);
    fill_ellipse(cx + 116u, cy - 18u, 32u, 14u,
                 pixel_value(255u, 170u, 0u));
    fill_rect(cx - 34u, cy + 78u, 24u, 22u, panel);
    fill_rect(cx + 32u, cy + 78u, 24u, 22u, panel);
}
