#include <stdint.h>

/* protos... */
typedef uint8_t u8;
typedef uint16_t u16;

/* writes and reads data from hardware ports respectively */
extern void outb(u16 port, u8 val);
extern u8 inb(u16 port);
