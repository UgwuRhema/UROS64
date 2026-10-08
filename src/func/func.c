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

void
outw(u16 port, u16 val)
{
	__asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

u16
inw(u16 port)
{
	u16 val;
	__asm__ volatile("inw %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

/* your typical memset */
void
memset(void *dest, u8 val, u32 len)
{
	u8 *ptr = (u8 *)dest;
	while (len--)
	{
		*ptr++ = (u8)val;
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

int
strncmp(const char *s1, const char *s2, size_t n)
{
	while (n && *s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
		n--;
	}

	if (n == 0)
		return 0;

	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
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

void
shutdown(void)
{
	outw(0x604, 0x2000);
	outw(0xb004, 0x2000);
}

int64_t string_to_int(const char* str)
{
	 int64_t result = 0;
	 int64_t sign = 1; //numbers are positive by default

	//skip whitespace
	while (*str == ' ' || *str == '\t' || *str == '\n')
	{
		str++;
	}

	//handle sign
	if (*str == '-')
	{
		sign = -1;
		str++;
	} else if (*str == '+'){
		str++;
	}

	//main conversion
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}

	return result * sign;
}

void
get_cpu_vendor_name(char *vendor)
{
	u32 eax, ebx, ecx, edx; /* the 32bit registers */
	/* ebx, ecx and edx store the string, while eax call the function 0x0 */
	__asm__ volatile ("cpuid" : "=b"(ebx), "=d"(edx), "=c"(ecx), "=a"(eax) : "a"(0)); /* call CPUID with eax=0x0 */
	*((u32 *)&vendor[0]) = ebx;
	*((u32 *)&vendor[4]) = edx;
	*((u32 *)&vendor[8]) = ecx;
	vendor[12] = '\0'; /* null terminate the string...some of these strings might be more than 12 characters */
}

void
get_cpu_brand_string(char *brand)
{
	u32 eax, ebx, ecx, edx;
	/* first we have to check if extended CPUID is available/supported */
	eax = 0x80000000;
	__asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(eax));
	/* eax now holds the max extended leaf */
	if (eax < 0x80000004)
	{
		brand[0] = '\0'; /* not supported*/
		return;
	}

	/* three leaves, 16 bytes each = 48 bytes */
	for (u32 leaf = 0x80000002; leaf <= 0x80000004; ++leaf)
	{
		u32 a = leaf;
		__asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(a));
	        u32 offset = (leaf - 0x80000002) * 16;
        *((u32 *)&brand[offset +  0]) = eax;
        *((u32 *)&brand[offset +  4]) = ebx;
        *((u32 *)&brand[offset +  8]) = ecx;
        *((u32 *)&brand[offset + 12]) = edx;	
	}
	brand[48] = '\0';
}
