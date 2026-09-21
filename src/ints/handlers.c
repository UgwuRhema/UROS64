#include "handlers.h"

__attribute__((interrupt)) void divide_by_zero(void *frame)
{
    clear_screen();
    kprint("KERNEL PANIC!\n", PURPLE);
    kprint("Fatal exception in interrupt!\n", PURPLE);
    kprint("Interrupt #0: Divide by zero\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while (1)
        __asm__ volatile ("hlt");    
}
