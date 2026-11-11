#include <stdint.h>
#include <stddef.h>

#define COM1 0x3f8u
#define PAGE_SIZE 4096u
#define MAX_TRACKED_PAGES (1024u * 1024u * 1024u / PAGE_SIZE)
#define BITMAP_BYTES (MAX_TRACKED_PAGES / 8u)
#define PIT_HZ 1000u
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
    /* Only IRQ0 is enabled until the other devices have handlers. */
    outb(0x21,0xfe); outb(0xa1,0xff);
}
static void pit_init(void) {
    uint16_t divisor=(uint16_t)(1193182u/PIT_HZ);
    outb(0x43,0x36); outb(0x40,(uint8_t)divisor); outb(0x40,(uint8_t)(divisor>>8));
}
void interrupt_dispatch(struct interrupt_frame *frame) {
    if (frame->vector==32u) {
        ++pit_ticks;
        if ((pit_ticks%PIT_HZ)==0u) serial_write("NUN: PIT interrupt received (1000 Hz)\n");
        outb(0x20,0x20);
        return;
    }
    serial_write("NUN: kernel exception vector="); serial_hex(frame->vector);
    serial_write(" error="); serial_hex(frame->error);
    serial_write(" rip="); serial_hex(frame->rip); serial_write("\n");
    halt_forever();
}

static void bit_set(uint32_t page) { page_bitmap[page>>3] |= (uint8_t)(1u<<(page&7u)); }
static void bit_clear(uint32_t page) { page_bitmap[page>>3] &= (uint8_t)~(1u<<(page&7u)); }
static int bit_test(uint32_t page) { return (page_bitmap[page>>3]>>(page&7u))&1u; }
static uint64_t pmm_init(uint32_t mbi) {
    for (size_t i=0;i<sizeof(page_bitmap);i++) page_bitmap[i]=0xffu;
    if (!mbi || (uint64_t)mbi>=((uint64_t)MAX_TRACKED_PAGES*PAGE_SIZE)) return 0;
    uint32_t total=*(volatile uint32_t *)(uintptr_t)mbi;
    if (total<16u || (uint64_t)mbi+(uint64_t)total>((uint64_t)MAX_TRACKED_PAGES*PAGE_SIZE)) return 0;
    uint32_t offset=8u;
    uint64_t kernel_first=(uint64_t)(uintptr_t)&__kernel_start/PAGE_SIZE;
    uint64_t kernel_last=((uint64_t)(uintptr_t)&__kernel_end+PAGE_SIZE-1u)/PAGE_SIZE;
    uint64_t mbi_first=(uint64_t)mbi/PAGE_SIZE;
    uint64_t mbi_last=((uint64_t)mbi+total+PAGE_SIZE-1u)/PAGE_SIZE;
    int mmap_seen=0;
    while ((uint64_t)offset+8u<=total) {
        uint32_t *tag=(uint32_t *)(uintptr_t)(mbi+offset);
        uint32_t type=tag[0], size=tag[1];
        if (size<8u || (uint64_t)offset+size>total) return 0;
        if (type==MB2_TAG_MMAP && size>=16u) {
            uint32_t entry_size=*(uint32_t *)(uintptr_t)(mbi+offset+8u);
            if (entry_size<24u || entry_size>size-16u) return 0;
            mmap_seen=1;
            for (uint64_t pos=(uint64_t)offset+16u; pos+entry_size<=(uint64_t)offset+size; pos+=entry_size) {
                uint64_t base=*(uint64_t *)(uintptr_t)(mbi+(uint32_t)pos);
                uint64_t length=*(uint64_t *)(uintptr_t)(mbi+(uint32_t)pos+8u);
                uint32_t region_type=*(uint32_t *)(uintptr_t)(mbi+(uint32_t)pos+16u);
                if (region_type!=1u || !length || base+length<base) continue;
                uint64_t first=base/PAGE_SIZE+((base%PAGE_SIZE)!=0u);
                uint64_t last=(base+length)/PAGE_SIZE;
                if (first<256u) first=256u;
                if (last>MAX_TRACKED_PAGES) last=MAX_TRACKED_PAGES;
                for (uint64_t page=first;page<last;page++) {
                    if ((page<kernel_first || page>=kernel_last) &&
                        (page<mbi_first || page>=mbi_last)) bit_clear((uint32_t)page);
                }
            }
        }
        if (type==MB2_TAG_END) break;
        offset=(uint32_t)(((uint64_t)offset+size+7u)&~7ull);
    }
    if (!mmap_seen) return 0;
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
static int pmm_self_test(void) {
    volatile uint64_t *page=(volatile uint64_t *)pmm_alloc_page();
    if (!page) return 0;
    for (size_t i=0;i<PAGE_SIZE/sizeof(uint64_t);i++)
        page[i]=0x4e554e5041474500ull^(uint64_t)i;
    for (size_t i=0;i<PAGE_SIZE/sizeof(uint64_t);i++)
        if (page[i]!=(0x4e554e5041474500ull^(uint64_t)i)) return 0;
    for (size_t i=0;i<PAGE_SIZE/sizeof(uint64_t);i++) page[i]=0;
    return 1;
}
static int command_is(const char *line,size_t length,const char *word) {
    size_t i=0;
    while (word[i]) { if (i>=length || line[i]!=word[i]) return 0; ++i; }
    return i==length;
}
/* Serial RX is polled after each timer wake until a UART IRQ handler exists. */
static void serial_poll(void) {
    static char line[16];
    static size_t length;
    while (inb(COM1+5)&1u) {
        char c=(char)inb(COM1);
        if (c=='\r' || c=='\n') {
            if (command_is(line,length,"ping")) serial_write("NUN: pong\n");
            else if (command_is(line,length,"help")) serial_write("NUN serial commands: ping, help\n");
            else if (length) serial_write("NUN: unknown command; send help\n");
            length=0;
        } else if ((c=='\b' || (uint8_t)c==0x7fu) && length) {
            --length;
        } else if ((uint8_t)c>=0x20u && (uint8_t)c<=0x7eu) {
            if (length<sizeof(line)) line[length++]=c;
            else length=0;
        }
    }
}

void kernel_main(uint32_t mbi) {
    serial_init();
    serial_write("\nNUN kernel: x86_64 entry reached\n");
    serial_write("NUN: Multiboot2 info at "); serial_hex(mbi); serial_write("\n");
    uint64_t free_pages=pmm_init(mbi);
    serial_write("NUN: usable physical pages below 1 GiB: "); serial_hex(free_pages); serial_write("\n");
    if (!free_pages || !pmm_self_test()) {
        serial_write("NUN: physical memory self-test FAILED\n");
        halt_forever();
    }
    serial_write("NUN: physical memory self-test passed\n");
    idt_init();
    pic_init();
    pit_init();
    serial_write("NUN: IDT installed; PIC remapped; PIT configured at 1000 Hz\n");
    serial_write("NUN Kernel Ready\n");
    serial_write("NUN serial console: send ping or help\n");
    __asm__ volatile ("sti");
    for (;;) {
        __asm__ volatile ("hlt");
        serial_poll();
    }
}
