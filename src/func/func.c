#include "func.h"

//send commands to port...
void 
outb(u16 port, u8 val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

//reads from port
u8
inb(u16 port)
{
	u8 val;
	__asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

/* your typical memset */
void
memset(void *dest, u16 val, u32 len)
{
	u16 *ptr = (u16 *)dest;
	while (len--)
	{
		*ptr++ = val;
	}
}

/* write to a 64 bit Model Specific Register */
void
wrmsr(u32 msr, u64 val)
{
	u32 low = (u32)val;
	u32 high = (u32)(val >> 32);
	__asm__ volatile ("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

/* read from a Model Specific Register */
u64
rdmsr(u32 msr)
{
	u32 low, high;
	__asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
	return ((u64)high << 32) | low;
}

int
strcmp(const char *str1, const char *str2)
{
	while(*str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
	}

	return *(const unsigned char *)str1 - *(const unsigned char *)str2;
}

size_t
strlen(const char *str)
{
	size_t count = 0;
	while (*str)
	{
		count++;
		str++;
	}

	return count;
}
