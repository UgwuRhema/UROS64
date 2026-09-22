#include "handlers.h"

__attribute__((interrupt)) void
divide_by_zero(void *frame)
{
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", PURPLE);
    kprint("Fatal exception in interrupt!\n", PURPLE);
    kprint("Interrupt #0: Divide by zero\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while (1)
        __asm__ volatile ("hlt");    
}

__attribute__((interrupt)) void
debug(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
non_maskable(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
break_point(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
overflow(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
bound_range_exceeded(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
invalid_opcode(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
device_not_available(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
double_fault(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
invalid_tss(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
segement_not_present(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
stack_segfault(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
general_protection(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
page_fault(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
x87_fpu_error(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
alignment_check(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
machine_check(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
simd_error(void *frame)
{
    while(1)
        __asm__ volatile ("hlt");
};
