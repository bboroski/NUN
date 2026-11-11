#ifndef NUN_INTERRUPTS_H
#define NUN_INTERRUPTS_H

void nun_interrupts_init(void);
void nun_interrupts_enable(void);
void nun_timer_interrupt(void);
void nun_exception_handler(unsigned long vector);

#endif
