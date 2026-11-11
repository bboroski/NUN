/* NUN x86_64 bootstrap. GRUB enters through the Multiboot2 32-bit ABI. */
.section .multiboot2,"a"
.align 8
mb2_header_start:
    .long 0xe85250d6
    .long 0
    .long mb2_header_end - mb2_header_start
    .long -(0xe85250d6 + (mb2_header_end - mb2_header_start))

    /* Request Multiboot2 memory-map tag (type 6). */
    .short 1
    .short 0
    .long 12
    .long 6
    .align 8

    .short 0
    .short 0
    .long 8
mb2_header_end:

.section .text.boot,"ax"
.code32
.global _start
.type _start, @function
_start:
    cli
    movl $boot_stack_top, %esp
    movl %eax, mb2_magic
    movl %ebx, boot_info_ptr

    /* Identity-map the first GiB with 2 MiB pages. */
    movl $page_pdpt, %eax
    orl  $0x03, %eax
    movl %eax, page_pml4
    movl $0, page_pml4 + 4

    movl $page_directory, %eax
    orl  $0x03, %eax
    movl %eax, page_pdpt
    movl $0, page_pdpt + 4

    movl $page_directory, %edi
    xorl %ecx, %ecx
1:
    movl %ecx, %eax
    shll $21, %eax
    orl  $0x83, %eax
    movl %eax, (%edi)
    movl $0, 4(%edi)
    addl $8, %edi
    incl %ecx
    cmpl $512, %ecx
    jne 1b

    /* Enable PAE, long mode and paging. */
    movl %cr4, %eax
    orl  $0x20, %eax
    movl %eax, %cr4

    movl $page_pml4, %eax
    movl %eax, %cr3

    movl $0xC0000080, %ecx
    rdmsr
    orl  $0x100, %eax
    wrmsr

    lgdt gdt64_pointer
    movl %cr0, %eax
    orl  $0x80010000, %eax
    movl %eax, %cr0
    ljmp $0x08, $long_mode_entry

.code64
long_mode_entry:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs

    lea boot_stack_top(%rip), %rsp
    xorq %rbp, %rbp
    movl boot_info_ptr(%rip), %edi
    movl mb2_magic(%rip), %esi
    call kmain

2:
    cli
    hlt
    jmp 2b
.size _start, . - _start

.align 8
gdt64:
    .quad 0x0000000000000000
    .quad 0x00af9a000000ffff
    .quad 0x00cf92000000ffff
gdt64_end:
gdt64_pointer:
    .word gdt64_end - gdt64 - 1
    .long gdt64

.section .bss,"aw",@nobits
.align 4096
page_pml4:
    .skip 4096
.align 4096
page_pdpt:
    .skip 4096
.align 4096
page_directory:
    .skip 4096
.align 4
mb2_magic:
    .long 0
boot_info_ptr:
    .long 0
.align 16
boot_stack_bottom:
    .skip 16384
boot_stack_top:

.section .note.GNU-stack,"",@progbits
