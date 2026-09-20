#include <stddef.h>
#include <stdint.h>
#include "../vga/vga.h"

void 
kmain()
{
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL v0.01\n", BLUE);
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
