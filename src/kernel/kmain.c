#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;

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
	//u64 *yes = (void *)0x0;
	//*yes = 50;

    /* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
