#include "handlers.h"

__attribute__((interrupt)) void divide_by_zero([[maybe_unused]]void *frame)
{
    kprint("KERNEL PANIC!\n", PURPLE);
    kprint("Fatal exception in interrupt!\n", YELLOW);
    kprint("Interrupt divide by zero\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while (1)
        __asm__ volatile ("hlt");    
}
