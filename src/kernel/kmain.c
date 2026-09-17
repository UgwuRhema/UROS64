#include <stddef.h>
#include <stdint.h>

void 
kmain()
{
	/* infinte loop */
	while (1)
	{
		__asm__("hlt");
	}
}
