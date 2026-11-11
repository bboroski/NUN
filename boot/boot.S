/* Multiboot2 32-bit entry and minimal x86_64 transition. */
.set MB2_MAGIC, 0xe85250d6
.section .multiboot2,"a"
.align 8
mb2_header_start:
    .long MB2_MAGIC
    .long 0
    .long mb2_header_end - mb2_header_start
    .long -(MB2_MAGIC + (mb2_header_end - mb2_header_start))
    .short 0
    .short 0
    .long 8
mb2_header_end:

.section .text.boot,"ax"
.code32
.global _start
_start:
    cli
    movl $boot_stack_top, %esp
    movl %ebx, %edi

    movl $pdpt, %eax
    orl $3, %eax
    movl %eax, pml4
    movl $page_directory, %eax
    orl $3, %eax
    movl %eax, pdpt
    movl $page_directory, %edi
    movl $0x83, %eax
    movl $512, %ecx
1:
    movl %eax, (%edi)
    movl $0, 4(%edi)
    addl $0x200000, %eax
    addl $8, %edi
    loop 1b

    movl %cr4, %eax
    orl $(1 << 5), %eax
    movl %eax, %cr4
    movl $pml4, %eax
    movl %eax, %cr3
    movl $0xc0000080, %ecx
    rdmsr
    orl $(1 << 8), %eax
    wrmsr
    lgdt gdt64_pointer
    movl %cr0, %eax
    orl $(1 << 31), %eax
    movl %eax, %cr0
    ljmp $0x08, $long_mode_start

.code64
long_mode_start:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs
    movq $boot_stack_top, %rsp
    andq $-16, %rsp
    movl %ebx, %edi
    call kernel_main
2:
    cli
    hlt
    jmp 2b

.align 8
gdt64:
    .quad 0
    .quad 0x00af9a000000ffff
    .quad 0x00cf92000000ffff
gdt64_end:
gdt64_pointer:
    .word gdt64_end - gdt64 - 1
    .long gdt64

.section .bss,"aw",@nobits
.align 4096
pml4: .skip 4096
.align 4096
pdpt: .skip 4096
.align 4096
page_directory: .skip 4096
.align 16
boot_stack_bottom: .skip 65536
boot_stack_top:

.section .text,"ax"
.code64
.macro ISR_NOERR n
.global isr\n
isr\n:
    pushq $0
    pushq $\n
    jmp isr_common
.endm
.macro ISR_ERR n
.global isr\n
isr\n:
    pushq $\n
    jmp isr_common
.endm
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR 29
ISR_ERR 30
ISR_NOERR 31
.global isr32
isr32:
    pushq $0
    pushq $32
    jmp isr_common
.global isr_default
isr_default:
    pushq $0
    pushq $255
isr_common:
    cld
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rsp, %rdi
    movq %rsp, %r12
    andq $-16, %rsp
    call interrupt_dispatch
    movq %r12, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    addq $16, %rsp
    iretq

.section .rodata,"a"
.align 8
.global isr_stub_table
isr_stub_table:
    .quad isr0,isr1,isr2,isr3,isr4,isr5,isr6,isr7
    .quad isr8,isr9,isr10,isr11,isr12,isr13,isr14,isr15
    .quad isr16,isr17,isr18,isr19,isr20,isr21,isr22,isr23
    .quad isr24,isr25,isr26,isr27,isr28,isr29,isr30,isr31,isr32
.section .note.GNU-stack,"",@progbits
