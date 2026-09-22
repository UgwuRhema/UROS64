#include "../vga/vga.h"
#include "interrupts.h"

extern __attribute__((interrupt)) void divide_by_zero(void *); /* note these interrupt handlers are in order */
/* they are in order of their vector numbers starting from zero */
extern __attribute__((interrupt)) void debug(void *);
extern __attribute__((interrupt)) void non_maskable(void *);
extern __attribute__((interrupt)) void break_point(void *);
extern __attribute__((interrupt)) void overflow(void *);
extern __attribute__((interrupt)) void bound_range_exceeded(void *);
extern __attribute__((interrupt)) void invalid_opcode(void *);;
extern __attribute__((interrupt)) void device_not_available(void *);
extern __attribute__((interrupt)) void double_fault(void *);
/* vector 9 is unused */
extern __attribute__((interrupt)) void invalid_tss(void *);
extern __attribute__((interrupt)) void segement_not_present(void *);
extern __attribute__((interrupt)) void stack_segfault(void *);
extern __attribute__((interrupt)) void general_protection(void *);
extern __attribute__((interrupt)) void page_fault(void *);
/* vector 15 currently unused */
extern __attribute__((interrupt)) void x87_fpu_error(void *); /* what is this? */
extern __attribute__((interrupt)) void alignment_check(void *);
extern __attribute__((interrupt)) void machine_check(void *);
extern __attribute__((interrupt)) void simd_error(void *);
/* vectors 20 - 31 currentl unused... */
