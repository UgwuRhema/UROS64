#include <stdint.h>

#define _VGA_ADDR 0xb8000 /* vga mem address */
#define _VGA_COLS 80
#define _VGA_ROWS 25

/* my attributes UROS is only allowed to use */
#define WHITE 0x07
#define BLUE
#define YELLOW

static long unsigned int cursor_pos = 0;

extern void kprint_c(char, uint16_t);
extern void kprint(const char *, uint16_t);
