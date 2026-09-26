#include "mouse.h"

void
mouse_wait(uint8_t type)
{
	uint32_t timeout = 100000;
	if (type == 0) /* wait for data to read */
	{
		while (--timeout && !(inb(0x64) & 1));
	} else { /* wait for buffer to be enmpty */
		while (--timeout && (inb(0x64) & 2));
	}
}

void
mouse_write(uint8_t value)
{
	mouse_wait(1);
	outb(0x64, 0xd4); /* send to mouse that we wanna write */
	mouse_wait(1);
	outb(0x60, value); /* send the actual data */
}

void
mouse_init(void)
{
    uint8_t status;

    mouse_wait(1);
    outb(0x64, 0xA8); // Enable mouse port

    mouse_wait(1);
    outb(0x64, 0x20); // Read Command Byte
    mouse_wait(0);
    status = inb(0x60);
    status |= 3;      // SET BOTH BIT 0 (Keyboard) AND BIT 1 (Mouse)

    mouse_wait(1);
    outb(0x64, 0x60); // Write Command Byte
    mouse_wait(1);
    outb(0x60, status);

    mouse_write(0xF6); // Set defaults
    mouse_write(0xF4); // Enable data reporting

}
