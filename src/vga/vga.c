#include "vga.h"

void 
kprint_c(char c, u16 attr)
{
    if (c == '\n')
    {
        cursor_pos = (cursor_pos / _VGA_COLS + 1) * _VGA_COLS;         
    } else if (c == '\b'){
        if (cursor_pos > 0)
        {
            cursor_pos--;
            _VGA_BUF[cursor_pos] = (u16)32 | (attr << 8);
        }
    } else {
        _VGA_BUF[cursor_pos] = (u16)c | (attr << 8);
        cursor_pos++;
    }
}

void 
kprint(const char *str, u16 attr)
{
    while (*str)
    {
       kprint_c(*str++, attr);
    }
}
