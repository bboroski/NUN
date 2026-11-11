/* Interrupt entry stubs. All general-purpose registers are preserved. */
.code64
.section .text,"ax"

.macro PUSH_GPRS
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
.endm

.macro POP_GPRS
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
.endm

.macro ISR_NOERR name, vector
.global \name
.type \name, @function
\name:
    pushq $0
    pushq $\vector
    jmp isr_exception_common
.endm

.macro ISR_ERR name, vector
.global \name
.type \name, @function
\name:
    pushq $\vector
    jmp isr_exception_common
.endm

ISR_NOERR isr_divide_error, 0
ISR_NOERR isr_invalid_opcode, 6
ISR_ERR   isr_double_fault, 8
ISR_ERR   isr_general_protection, 13
ISR_ERR   isr_page_fault, 14

.global isr_timer
.type isr_timer, @function
isr_timer:
    PUSH_GPRS
    movq %rsp, %r12
    andq $-16, %rsp
    cld
    call nun_timer_interrupt
    movq %r12, %rsp
    POP_GPRS
    iretq

.type isr_exception_common, @function
isr_exception_common:
    PUSH_GPRS
    movq 120(%rsp), %rdi
    movq %rsp, %r12
    andq $-16, %rsp
    cld
    call nun_exception_handler
    movq %r12, %rsp
    POP_GPRS
    addq $16, %rsp
    iretq

.section .note.GNU-stack,"",@progbits
