#include <stddef.h>
#include <stdint.h>
#include "../vga/vga.h"

void 
kmain()
{
    kprint("Hello World\n", BLUE);
    kprint("Another Hello World\n", WHITE);
    kprint("Final boss hello World\n", YELLOW);
    kprint("True true Ultimate final boss Hello World\n", PURPLE);
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
