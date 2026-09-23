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
	kprint("Initializing Components and Necessities... \n", GREEN);
	kp_log("Set up Interrupt Descriptor Table and loaded CPU Exceptions", DONE);
	kp_log("Mask PIC Interrupts and set up PIC IRQs", NOT_DONE);

	volatile u64 *num = (volatile u64 *)0xdeadbeef;
	*num = 0x1234;
	
    /* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
