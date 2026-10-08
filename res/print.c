#include <unistd.h>


int
main(void)
{
	const char str[] = "Hello World\n";
	(void)write(1, str, sizeof(str) - 1); /* write syscall */
	return 0;
}
