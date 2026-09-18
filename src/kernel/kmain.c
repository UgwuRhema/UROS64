#include <stddef.h>
#include <stdint.h>
#include "../vga/vga.h"

void 
kmain()
{
    cursor_pos = 898034;
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
