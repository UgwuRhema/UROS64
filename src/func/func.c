#include "func.h"

//send commands to port...
void 
outb(u16 port, u8 val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

//reads from port
u8
inb(u16 port)
{
	u8 val;
	__asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}
