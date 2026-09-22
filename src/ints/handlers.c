#include "handlers.h"

__attribute__((interrupt)) void
divide_by_zero(void *frame)
{
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #0: Divide by zero\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while (1)
        __asm__ volatile ("hlt");    
}

__attribute__((interrupt)) void
debug(void *frame)
{
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #1: Debug Interrupt\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
non_maskable(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #2: Non maskable interrupt\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
break_point(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #3: Breakpoint interrupt\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
overflow(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #4: Stack Overflow\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
bound_range_exceeded(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #5: Bound Range Exceeded\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
invalid_opcode(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #6: Invalid Opcode\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
device_not_available(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #7: Device not available\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
double_fault(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Double Fault!\n", PURPLE);
    kprint("Interrupt #8 | Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
invalid_tss(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #10: Invalid TSS | Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
segement_not_present(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #11: Segment not present\n", YELLOW);
    kprint("Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
stack_segfault(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Stack Segmentation Fault!\n", PURPLE);
    kprint("Interrupt #12 | Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
general_protection(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("General Protection Fault!\n", PURPLE);
    kprint("Interrupt #13 | Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
page_fault(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Page Fault!\n", PURPLE);
    kprint("Interrupt #14 | Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
x87_fpu_error(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("x87 FPU Error\n", GREEN);
    kprint("Interrupt #16\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
alignment_check(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #17: Alignment Check\n", YELLOW);
    kprint("Error Code: ", YELLOW); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
machine_check(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #18: Machine Check\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
simd_error(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("KERNEL PANIC!\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #19: SIMD Error\n", YELLOW);
    kprint("System Halted. Please reboot\n", BLUE);
    while(1)
        __asm__ volatile ("hlt");
};
