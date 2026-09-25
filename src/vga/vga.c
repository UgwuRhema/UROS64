#include "vga.h"
#include "../func/func.h"

uint64_t cursor_pos = 0;

void 
kprint_c(char c, u16 attr)
{
    /* handle newline */
    if (c == '\n')
    {
        cursor_pos = (cursor_pos / _VGA_COLS + 1) * _VGA_COLS;         
    } else if (c == '\b'){ /* handle backspace */
        if (cursor_pos > 0)
        {
            cursor_pos--;
            _VGA_BUF[cursor_pos] = (u16)32 | (attr << 8);
        }
    } else {
        /* else it's just a normal character... */
        _VGA_BUF[cursor_pos] = (u16)c | (attr << 8);
        cursor_pos++;
    }
    
    /* track the blinking cursor */
    outb(0x3d4, 0x0f);
    outb(0x3d5, (u8)(cursor_pos & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (u8)(cursor_pos >> 8) & 0xff);
}

void 
kprint(const char *str, u16 attr)
{
    /* while the current character is not the '\0' null terminator */
    while (*str)
    {
       kprint_c(*str++, attr);
    }
}

/* better or nah? */
void
clear_screen(void)
{
    u16 blank = (WHITE << 8) | ' ';

    for (int i = 0; i < _VGA_COLS * _VGA_ROWS; ++i)
    {
        _VGA_BUF[i] = blank;
    }

    cursor_pos = 0;
}

void
kp_log(const char *str, enum status s)
{
	kprint(" ", WHITE);
    kprint(str, WHITE);
    kprint(" [", WHITE);
    if (s == 0)
    {
        kprint("d", GREEN);
    } else if (s == 1){
        kprint("f", PURPLE);
    } else {
        kprint("o", GREEN);
    }
    kprint("] \n", WHITE);
}

void
kprint_hex(uint64_t num, u16 color)
{
    const char *hex = "0123456789ABCDEF"; /* valid hexadecimal values */
    kprint("0x", color);
    /* let's print all hex digit */
    for (int i = 7; i >= 0; i--)
    {
        u8 digit = (num >> (i * 4)) & 0xf;
        kprint_c(hex[digit], color);
    }
}

void
kprint_num(uint64_t num, u16 attr)
{
	if (num == 0)
	{
		kprint_c('0', attr);
		return;
	}
	char buf[12];
	int i = 10;
	buf[11] = '\0';
	while (num > 0 && i >= 0)
	{
		buf[i--] = (num % 10) + '0';
		num /= 10;
	}
	kprint(&buf[i + 1], attr);
}
