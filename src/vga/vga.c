#include "vga.h"

void kprint_c(char c, uint16_t attr)
{
    
}

void kprint(const char *str, uint16_t attr)
{
    while (*str != '\0')
    {
       kprint_c(*str, attr);
    }
}
