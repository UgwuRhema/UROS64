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

#define MIN_SPLIT 4

void *
kmalloc(uint64_t size)
{
	if (size == 0) return NULL;
	size = (size + (8 - 1)) & ~(8 - 1); /* rounding size up to alignment */

	/* let's walk the list now... */
	struct header_t *curr = heap_start;
	while (curr)
	{
		if (curr->is_free && curr->size >= size)
		{
			/* if the block is much bigger than neede, split it... */
			if (curr->size >= size + sizeof(struct header_t) + MIN_SPLIT)
			{
				struct header_t *tail = (struct header_t *)((char *)curr + sizeof(struct header_t) + size);
				tail->next = curr->next;
				tail->size = curr->size - size - sizeof(struct header_t);
				tail->is_free = 1;
				curr->next = tail;
				curr->size = size;
			}

			curr->is_free = 0;
			/* hand back the pointer! */
			return (void *)((char *)curr + sizeof(struct header_t));
		}
		curr = curr->next;
	}

	return NULL; /* when heap is exhausted! */
	/* this is temporary and we will implement a way to grow our heap later */
}

void
kfree(void *ptr)
{
	if (!ptr) return;
	struct header_t *header = (struct header_t *)((char *)ptr - sizeof(struct header_t));
	if (header->magic != 0xdeadbeef) /* cehcking for corruption */
		panic();
	header->is_free = 1;
	struct header_t *curr = heap_start;
	while (curr)
	{
		if (curr->is_free && curr->next && curr->next->is_free)
		{
			/* merge next block into current block */
			/* this supposedly prevents external fragmentation */
			curr->size += sizeof(struct header_t) + curr->next->size;
			curr->next = curr->next->next;
		} else {
			curr = curr->next;
		}
	}
}
