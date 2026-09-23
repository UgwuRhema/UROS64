#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;

#include "../vga/vga.h"
#include "../func/func.h"
#include "../ints/interrupts.h"

void 
kmain()
{
	idt_init();
	kprint("Kernel loading...\n" , WHITE);
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	kprint("x86 Monolithic 64-bit Kernel written in URSL\n", GREEN);
	kprint("\n", WHITE);
	kprint("Setting up... \n", GREEN);
	kp_log("Enable 64-bit Paging. ", DONE);
	kp_log("Setup up VGA and VGA functions. ", DONE);
	kp_log("Set up Interrupt Descriptor Table and load CPU Exceptions. ", DONE);
	kp_log("Enable LAPIC and IOAPIC Hardware Interrupts. ", DONE);
	kp_log("Set up PS/2 Keyboard Driver and Model", DONE);
	kp_log("Implement the Memory Management Unit. ", NOT_DONE);
	kp_log("Same as the last one; Implement PMM and VMM. ", NOT_DONE);
	
    /* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
