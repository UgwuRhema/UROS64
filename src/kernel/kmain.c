#include <stddef.h>
#include <stdint.h>

#include "../vga/vga.h"
#include "../func/func.h"
#include "../ints/interrupts.h"

void 
kmain()
{
	kprint("Kernel loading...\n" , WHITE);
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	kprint("Initializing Components and Necessities... \n", GREEN);
    idt_init();

	/* Trigger synchronous CPU Exception 0 */
	int a = 10;
	int b = 0;
	int c = a / b;
	(void)c; 

    /* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
