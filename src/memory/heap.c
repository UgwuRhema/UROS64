#include "heap.h"

static struct header_t *heap_start = NULL;

void 
kheap_init(uint64_t initial_pages)
{
	for (uint64_t i = 0; i < initial_pages; ++i)
	{
		uint64_t virt = HEAP_START_ADDR + i * PAGE_SIZE;
		void *phys = pmm_alloc();
		if (!phys)
		{
			/* memory allocation failure */
			panic(); /* this functions just clears interrupts and halts the CPU */
		}
		vmm_map(virt, (uint64_t)phys, PTE_WRITABLE);
	}

	heap_start = (struct header_t *)HEAP_START_ADDR;
	heap_start->size = initial_pages * PAGE_SIZE - sizeof(struct header_t);
	heap_start->is_free = 1;
	heap_start->next = NULL;
}
