#include "vmm.h"
#include "pmm.h"
#include "../func/func.h"

#define HEAP_START_ADDR (VMM_BASE + 0x10000000ULL) /* 256MB above base */

/* the header struct */
struct header_t
{
	uint64_t size;
	int is_free;
	struct header_t *next;
	uint32_t magic = 0xdeadbeef; /* this would be used for memory corruption detection */
};

extern void kheap_init(uint64_t);
extern void *kmalloc(uint64_t);
extern void kfree(void *);
