#include <stdint.h>

/* protos... */
typedef uint8_t u8;
typedef uint16_t u16;

/* writes and reads data from hardware ports respectively */
static void outb(u16 port, u8 val);
static u8 inb(u16 port);
