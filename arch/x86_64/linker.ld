OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHDRS
{
    text   PT_LOAD FLAGS(5); /* R-X */
    rodata PT_LOAD FLAGS(4); /* R-- */
    data   PT_LOAD FLAGS(6); /* RW- */
}

SECTIONS
{
    . = 0x00100000;
    __kernel_start = .;

    .multiboot2 ALIGN(8) : { KEEP(*(.multiboot2)) } :text
    .text ALIGN(0x1000) : { *(.text.boot) *(.text .text.*) } :text
    .rodata ALIGN(0x1000) : { *(.rodata .rodata.*) } :rodata
    .data ALIGN(0x1000) : { *(.data .data.*) } :data
    .bss ALIGN(0x1000) :
    {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    . = ALIGN(0x1000);
    __kernel_end = .;

    /DISCARD/ : { *(.eh_frame .eh_frame_hdr .comment .note .note.*) }
}
