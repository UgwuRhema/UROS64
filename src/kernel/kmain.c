#include <stddef.h>
#include <stdint.h>
#include "../vga/vga.h"

static void recurse(void)
{
	void (* volatile rec)(void) = &recurse; /* correct no? */
	rec();
}

void 
kmain()
{
	kprint("Loading Kernel...\n" , WHITE);
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	recurse(); /* this should cause a stack overflow then a page fault which will lead to a triple fault*/	
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
