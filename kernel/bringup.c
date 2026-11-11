#include <stdint.h>
#include <stddef.h>

#define COM1 0x3f8u
#define PAGE_SIZE 4096u
#define MAX_TRACKED_PAGES (1024u * 1024u * 1024u / PAGE_SIZE)
#define BITMAP_BYTES (MAX_TRACKED_PAGES / 8u)
#define MB2_TAG_END 0u
#define MB2_TAG_MMAP 6u

extern uint8_t __kernel_start, __kernel_end;
extern void *isr_stub_table[];
extern void isr_default(void);

static uint8_t page_bitmap[BITMAP_BYTES];
static volatile uint64_t pit_ticks;

struct idt_entry {
    uint16_t offset_low, selector;
    uint8_t ist, attributes;
    uint16_t offset_mid;
    uint32_t offset_high, reserved;
} __attribute__((packed));
struct idt_pointer { uint16_t limit; uint64_t base; } __attribute__((packed));
static struct idt_entry idt[256];

struct interrupt_frame {
    uint64_t r15,r14,r13,r12,r11,r10,r9,r8;
    uint64_t rdi,rsi,rbp,rdx,rcx,rbx,rax;
    uint64_t vector,error,rip,cs,rflags;
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
    outb(COM1+1,0); outb(COM1+3,0x80); outb(COM1,3); outb(COM1+1,0);
    outb(COM1+3,3); outb(COM1+2,0xc7); outb(COM1+4,0x0b);
}
static void serial_char(char c) {
    for (uint32_t spin=0; spin<1000000u && !(inb(COM1+5)&0x20u); ++spin) { }
    outb(COM1,(uint8_t)c);
}
static void serial_write(const char *s) {
    while (*s) { if (*s=='\n') serial_char('\r'); serial_char(*s++); }
}
static void serial_hex(uint64_t value) {
    static const char digits[]="0123456789abcdef";
    serial_write("0x");
    for (int shift=60; shift>=0; shift-=4) serial_char(digits[(value>>(unsigned)shift)&15u]);
}
static void halt_forever(void) {
    __asm__ volatile ("cli");
    for (;;) __asm__ volatile ("hlt");
}
static void idt_set(uint8_t vector, void (*handler)(void)) {
    uint64_t address=(uint64_t)(uintptr_t)handler;
    idt[vector].offset_low=(uint16_t)address;
    idt[vector].selector=0x08;
    idt[vector].ist=0;
    idt[vector].attributes=0x8e;
    idt[vector].offset_mid=(uint16_t)(address>>16);
    idt[vector].offset_high=(uint32_t)(address>>32);
    idt[vector].reserved=0;
}
static void idt_init(void) {
    for (uint16_t i=0;i<256;i++) idt_set((uint8_t)i,isr_default);
    for (uint8_t i=0;i<33;i++) idt_set(i,(void (*)(void))isr_stub_table[i]);
    struct idt_pointer p={(uint16_t)(sizeof(idt)-1u),(uint64_t)(uintptr_t)idt};
    __asm__ volatile ("lidt %0" : : "m"(p));
}
static void pic_init(void) {
    outb(0x20,0x11); outb(0xa0,0x11);
    outb(0x21,0x20); outb(0xa1,0x28);
    outb(0x21,0x04); outb(0xa1,0x02);
    outb(0x21,0x01); outb(0xa1,0x01);
    outb(0x21,0xfe); outb(0xa1,0xff);
}
static void pit_init(void) {
    uint16_t divisor=(uint16_t)(1193182u/100u);
    outb(0x43,0x36); outb(0x40,(uint8_t)divisor); outb(0x40,(uint8_t)(divisor>>8));
}
void interrupt_dispatch(struct interrupt_frame *frame) {
    if (frame->vector==32u) {
        ++pit_ticks;
        if ((pit_ticks%100u)==0u) serial_write("NUN: PIT interrupt received (100 Hz)\n");
        outb(0x20,0x20);
        return;
    }
    serial_write("NUN: exception vector="); serial_hex(frame->vector);
    serial_write(" error="); serial_hex(frame->error);
    serial_write(" rip="); serial_hex(frame->rip); serial_write("\n");
    halt_forever();
}

static void bit_set(uint32_t page) { page_bitmap[page>>3] |= (uint8_t)(1u<<(page&7u)); }
static void bit_clear(uint32_t page) { page_bitmap[page>>3] &= (uint8_t)~(1u<<(page&7u)); }
static int bit_test(uint32_t page) { return (page_bitmap[page>>3]>>(page&7u))&1u; }
static uint64_t pmm_init(uint32_t mbi) {
    for (size_t i=0;i<sizeof(page_bitmap);i++) page_bitmap[i]=0xffu;
    if (!mbi) return 0;
    uint32_t total=*(volatile uint32_t *)(uintptr_t)mbi;
    uint32_t offset=8u;
    uint64_t kernel_first=(uint64_t)(uintptr_t)&__kernel_start/PAGE_SIZE;
    uint64_t kernel_last=((uint64_t)(uintptr_t)&__kernel_end+PAGE_SIZE-1u)/PAGE_SIZE;
    while (offset+8u<=total) {
        uint32_t *tag=(uint32_t *)(uintptr_t)(mbi+offset);
        uint32_t type=tag[0], size=tag[1];
        if (size<8u || offset+size>total) break;
        if (type==MB2_TAG_MMAP && size>=16u) {
            uint32_t entry_size=*(uint32_t *)(uintptr_t)(mbi+offset+8u);
            if (entry_size<24u) break;
            for (uint32_t pos=offset+16u; pos+entry_size<=offset+size; pos+=entry_size) {
                uint64_t base=*(uint64_t *)(uintptr_t)(mbi+pos);
                uint64_t length=*(uint64_t *)(uintptr_t)(mbi+pos+8u);
                uint32_t region_type=*(uint32_t *)(uintptr_t)(mbi+pos+16u);
                if (region_type!=1u || length==0u || base+length<base) continue;
                uint64_t first=(base+PAGE_SIZE-1u)/PAGE_SIZE;
                uint64_t last=(base+length)/PAGE_SIZE;
                if (first<256u) first=256u;
                if (last>MAX_TRACKED_PAGES) last=MAX_TRACKED_PAGES;
                for (uint64_t page=first;page<last;page++) {
                    if (page<kernel_first || page>=kernel_last) bit_clear((uint32_t)page);
                }
            }
        }
        if (type==MB2_TAG_END) break;
        offset=(offset+size+7u)&~7u;
    }
    uint64_t free_pages=0;
    for (uint32_t p=256u;p<MAX_TRACKED_PAGES;p++) if (!bit_test(p)) ++free_pages;
    return free_pages;
}
static uintptr_t pmm_alloc_page(void) {
    for (uint32_t p=256u;p<MAX_TRACKED_PAGES;p++) {
        if (!bit_test(p)) { bit_set(p); return (uintptr_t)p*PAGE_SIZE; }
    }
    return 0;
}

void kernel_main(uint32_t mbi) {
    serial_init();
    serial_write("\nNUN kernel: x86_64 entry reached\n");
    serial_write("NUN: Multiboot2 info at "); serial_hex(mbi); serial_write("\n");
    uint64_t free_pages=pmm_init(mbi);
    serial_write("NUN: usable physical pages below 1 GiB: "); serial_hex(free_pages); serial_write("\n");
    uintptr_t page=pmm_alloc_page();
    serial_write("NUN: physical page allocator probe: "); serial_hex(page); serial_write("\n");
    idt_init();
    pic_init();
    pit_init();
    serial_write("NUN: IDT installed; PIC remapped; PIT configured\n");
    serial_write("NUN: enabling interrupts\n");
    __asm__ volatile ("sti");
    halt_forever();
}
