#include <stdint.h>
#include <stddef.h>

/* protos... */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* writes and reads data from hardware ports respectively */
extern void outb(u16, u8);
extern u8 inb(u16);
extern void outw(u16, u16);
extern u16 inw(u16);
extern void memset(void *, u16, u32);
extern void wrmsr(u32, u64);
extern u64 rdmsr(u32);
extern int strcmp(const char *, const char *);
extern size_t strlen(const char *);
extern void shutdown(void);
