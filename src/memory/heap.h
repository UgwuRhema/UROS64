#include "vmm.h"
#include "pmm.h"

#define HEAP_START_ADDR (VMM_BASE + 0x10000000)

extern void khead_init(uint64_t);
extern void *kmalloc(uint64_t);
extern void kfree(void *);
