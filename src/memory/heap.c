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
			panic();
		}
		vmm_map((uint64_t)phys, virt, PTE_WRITABLE);
	}

	heap_start = (header_t *)HEAP_START_ADDR;
	heap_start->size = inittial_pages * PAGE_SIZE - sizeof(header_t);
	heap_start->is_free = 1;
	head_start->next = NULL;
}
