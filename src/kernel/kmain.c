#include <stddef.h>
#include <stdint.h>
#include "../vga/vga.h"

void 
kmain()
{
    kprint("Hello World", BLUE);
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
