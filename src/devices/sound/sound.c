#include "sound.h"

/* play sound using built in speaker */
static void 
play_sound(u32 nFrequency)
{
    u32 Div;
    u8 tmp;

    /* set PIT to desired frequency */
    Div = 1931180 / nFrequency;
    outb(0x43, 0xb6);
    outb(0x42, (u8)(Div));
    outb(0x42, (u8)(Div >> 8));

    /* play the sound ising PC Speaker */
    tmp = inb(0x61);
    if (tmp != (tmp | 3))
    {
        outb(0x61, tmp | 3);
    }
}

/* stop the sound */
static void 
nosound()
{
	u8 tmp = inb(0x61) & 0xfc;
	outb(0x61, tmp);
}

/* make a beep */
void
beep()
{
	play_sound(1000);
	sleep(200);
	nosound();
	enable_pit(1000);
}
