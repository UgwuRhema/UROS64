#include "../vga/vga.h"
#include "interrupts.h"

//extern uint64_t timer_ticks;

extern __attribute__((interrupt)) void divide_by_zero(void *); /* note these interrupt handlers are in order */
/* they are in order of their vector numbers starting from zero */
extern __attribute__((interrupt)) void debug(void *); //1
extern __attribute__((interrupt)) void non_maskable(void *); //2
extern __attribute__((interrupt)) void break_point(void *); //3
extern __attribute__((interrupt)) void overflow(void *); //4
extern __attribute__((interrupt)) void bound_range_exceeded(void *); //5
extern __attribute__((interrupt)) void invalid_opcode(void *);; //6
extern __attribute__((interrupt)) void device_not_available(void *); //7
extern __attribute__((interrupt)) void double_fault(void *); //8
/* vector 9 is unused */
extern __attribute__((interrupt)) void invalid_tss(void *); //10
extern __attribute__((interrupt)) void segement_not_present(void *); //11
extern __attribute__((interrupt)) void stack_segfault(void *); //12
extern __attribute__((interrupt)) void general_protection(void *); //13
extern __attribute__((interrupt)) void page_fault(void *); //14
/* vector 15 currently unused */
extern __attribute__((interrupt)) void x87_fpu_error(void *); /* what is this? */ //16
extern __attribute__((interrupt)) void alignment_check(void *); //17
extern __attribute__((interrupt)) void machine_check(void *); //18
extern __attribute__((interrupt)) void simd_error(void *); //19
/* vectors 20 - 31 currentl unused... */


/* APIC IRQ handlers */
extern void buffer_push(char);
extern char buffer_pop();
extern __attribute__((interrupt)) void keyboard_handler(void *);
extern __attribute__((interrupt)) void timer_handler(void *); 
