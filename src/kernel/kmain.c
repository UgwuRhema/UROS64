#include <stddef.h>
#include <stdint.h>
#include "../vga/vga.h"
#include "../func/func.h"

void 
kmain()
{
	kprint("Kernel loading...\n" , WHITE);
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	kprint("Initializing Components and Necessities... \n", GREEN);
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
