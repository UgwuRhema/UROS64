#include "vga.h"
#include "../func/func.h"

#define COM1 0x3f8

uint64_t cursor_pos = 0;

/* page fault */

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
	if (cursor_pos >= _VGA_COLS * _VGA_ROWS)
	{
		vga_scroll();
		cursor_pos = (_VGA_ROWS - 1) * _VGA_COLS;
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

static void
kprint_int(int64_t num, u16 color)
{
	if (num < 0)
	{
		kprint_c('-', color);
		/* -(INT64_MIN) overflows, so negate after adding one */
		kprint_num((uint64_t)(-(num + 1)) + 1, color);
	}
	else
	{
		kprint_num((uint64_t)num, color);
	}
}

static void
kprint_hex_raw(uint64_t num, u16 color, int uppercase)
{
	const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	char buf[16];
	int i = 0;

	if (num == 0)
	{
		kprint_c('0', color);
		return;
	}
	while (num > 0)
	{
		buf[i++] = digits[num & 0xf];
		num >>= 4;
	}
	while (i > 0)
		kprint_c(buf[--i], color);
}

/*print fmt with basic printf-style specifiers*/
static void
kvprint(const char *fmt, va_list args, u16 color)
{
	for (const char *p = fmt; *p; ++p)
	{
		if (*p != '%')
		{
			kprint_c(*p, color);
			continue;
		}
		++p;
		if (*p == '\0')
		{
			kprint_c('%', color);
			break;
		}
		int is_64 = 0;
		while (*p == 'l') /*%l and %ll both mean 64-bit here*/
		{
			is_64 = 1;
			++p;
		}
		if (*p == '\0') /*format ended after the length modifier*/
		{
			kprint_c('%', color);
			break;
		}
		switch (*p)
		{
			case 'd':
			case 'i':
				if (is_64)
					kprint_int(va_arg(args, int64_t), color);
				else
					kprint_int(va_arg(args, int), color);
				break;
			case 'u':
				if (is_64)
					kprint_num(va_arg(args, uint64_t), color);
				else
					kprint_num(va_arg(args, unsigned int), color);
				break;
			case 'x':
			case 'X':
				if (is_64)
					kprint_hex_raw(va_arg(args, uint64_t), color, *p == 'X');
				else
					kprint_hex_raw(va_arg(args, unsigned int), color, *p == 'X');
				break;
			case 'p':
				kprint_hex((uint64_t)(uintptr_t)va_arg(args, void *), color);
				break;
			case 's':
			{
				char *str = va_arg(args, char *);
				kprint(str ? str : "(null)", color);
				break;
			}
			case 'c':
				kprint_c((char)va_arg(args, int), color);
				break;
			case '%':
				kprint_c('%', color);
				break;
			default: /*unknown specifier, print it as-is*/
				kprint_c('%', color);
				kprint_c(*p, color);
				break;
		}
	}
}

void
kp_log_var(const char *fmt, enum status s, ...)
{
	va_list args;
	va_start(args, s);
	kprint(" ", WHITE);
	kvprint(fmt, args, WHITE);
	va_end(args);
	kprint(" [", WHITE);
	if (s == DONE)
		kprint("d", GREEN);
	else if (s == NOT_DONE)
		kprint("fail", PURPLE);
	else
		kprint("o", GREEN);
	kprint("] \n", WHITE);
}

void
kprint_hex(uint64_t num, u16 color)
{
    const char *hex = "0123456789ABCDEF"; /* valid hexadecimal values */
    kprint("0x", color);
    /* let's print all hex digit */
    for (int i = 15; i >= 0; i--)
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
	char buf[21];
	int i = 20;
	buf[i] = '\0';
	while (num > 0 && i > 0)
	{
		buf[--i] = (num % 10) + '0';
		num /= 10;
	}
	kprint(&buf[i], attr);
}


/* not really VGA functions though */
int
is_transmit_empty(void)
{
	return inb(COM1 + 5) & 0x20;
}

void s_c(char c)
{
	while (is_transmit_empty() == 0);
	outb(COM1, c);
}

void
skprint(const char * str)
{
	while (*str)
		s_c(*str++);
}

void
vga_scroll(void)
{
	u16 *vga = (u16*)_VGA_ADDR;

	//physically shift rows 1 to 24 up by one row
	for (int y = 0; y < _VGA_ROWS - 1; y++)
	{
		for (int x = 0; x < _VGA_COLS; x++)
		{
			vga[y * _VGA_COLS + x] = vga[(y + 1) * _VGA_COLS + x];
		}
	}

	//clear out the very bottom row (row 24) to make it empty space
	u16 blank_cell = (u16)' ' | (WHITE << 8);
	for (int x = 0; x < _VGA_COLS; x++)
	{
		vga[(_VGA_ROWS - 1) * _VGA_COLS + x] = blank_cell;
	}

}
