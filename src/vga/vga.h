#include <stdint.h>

typedef uint16_t u16;

enum status
{
    DONE,
    NOT_DONE,
};

#define _VGA_ADDR 0xb8000 /* vga address */
/* VGA screen width and breadth respectively */
#define _VGA_COLS 80
#define _VGA_ROWS 25

/* my attributes UROS is only allowed to use */
static const u16 WHITE = 0x07; /* for some reason i dont wann use #define MACROS */
static const u16 BLUE = 0x09;
static const u16 YELLOW = 0x0e;
static const u16 PURPLE = 0x05;
static const u16 GREEN = 0x0A;

extern uint64_t cursor_pos = 0;
/* the space using the VGA buffer */
#define _VGA_BUF ((volatile u16 *)_VGA_ADDR)

/* important printing functions */
extern void kprint_c(char, u16);
extern void kprint(const char *, u16);
extern void clear_screen(void);
extern void kp_log(const char *, enum status);
extern void kprint_hex(uint64_t, u16);
