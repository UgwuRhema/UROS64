#include <stdint.h>

/* protos... */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

/* writes and reads data from hardware ports respectively */
extern void outb(u16, u8);
extern u8 inb(u16);
extern void memset(void *, u8, u32);
