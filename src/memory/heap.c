#include "heap.h"
#include "../vga/vga.h"

static struct header_t *heap_start = NULL;
/*to track the last mapped VA*/
static uint64_t heap_end_virt = HEAP_START_ADDR;

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
		if (vmm_map(virt, (uint64_t)phys, PTE_WRITABLE) != 0)
		{
			pmm_free(phys);
			panic();
		}
	}
	heap_start = (struct header_t *)HEAP_START_ADDR;
	heap_start->size = initial_pages * PAGE_SIZE - sizeof(struct header_t);
	heap_start->is_free = 1;
	heap_start->magic = HEAP_MAGIC;
	heap_start->next = NULL;
	/*set the boundary checker*/
	heap_end_virt = HEAP_START_ADDR + (initial_pages * PAGE_SIZE);
}

#define MIN_SPLIT 32 /* bumped UP! */

#define EXTRA_ADD_PAGES (256 * 1024) /*this is the additional
amount of bytes we want to map to increase the kernel heap
when the already current heap is out of memory*/

/*grows the heap to a particular amount*/
static int
kernel_heap_grow(size_t how_much)
{
	size_t actual_bytes = (how_much > EXTRA_ADD_PAGES) ? how_much : EXTRA_ADD_PAGES;
	size_t pages = (actual_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
	uint64_t base_va = heap_end_virt;/*end of heap*/
	size_t done = 0;/*tracker*/
	for (; done < pages; ++done)
	{
		/*calculate the virtual address*/
		uint64_t virt = base_va + (done * PAGE_SIZE);
		void *pa = pmm_alloc();
		if (!pa) break;
		memset(pa, 0, PAGE_SIZE); /*clean newly allocated page*/
		if (vmm_map(virt, (uint64_t)pa, PTE_WRITABLE) != 0)
		{
			pmm_free(pa);
			break;
		}
	}
	if (done == 0) return -1;/*-1 on errors, failed to allocate any pages*/
	/*now that we have increase pages and the heap size, let's create a header
	 * for the newly created heap space*/
	struct header_t *new_block = (struct header_t *)base_va;
	new_block->size = (done * PAGE_SIZE) - sizeof(struct header_t);
	new_block->is_free = 1;
	new_block->magic = HEAP_MAGIC;/*set magic canary*/
	new_block->next = NULL;
	/*so now let's find the last block in the list and link the new one*/
	struct header_t *curr = heap_start;
	while (curr->next != NULL)
	{
		/*traverses till the it reaches the block whose next points
		 * to NULL*/
		curr = curr->next;
	}
	curr->next = new_block;/*link it*/
	heap_end_virt = base_va + (done * PAGE_SIZE);/*update global tracker*/
	return 0;/*success*/
}

void *
kernel_heap_malloc(uint64_t size)
{
	if (size == 0) return NULL;
	size = (size + (8 - 1)) & ~(8 - 1); /* rounding size up to alignment */
	/* let's walk the list now... */
	struct header_t *curr = heap_start;
	while (curr)
	{
		if (curr->is_free && 
				curr->size >= size)
		{
			/* if the block is much bigger than neede, split it... */
			if (curr->size >= size + sizeof(struct header_t) + MIN_SPLIT)
			{
				struct header_t *tail = (struct header_t *)((char *)curr + sizeof(struct header_t) + size);
				tail->next = curr->next;
				tail->size = curr->size - size - sizeof(struct header_t);
				tail->is_free = 1;
				tail->magic = HEAP_MAGIC;
				curr->next = tail;
				curr->size = size;
			}
			curr->is_free = 0;
			/* hand back the pointer! */
			return (void *)((char *)curr + sizeof(struct header_t));
		}
		curr = curr->next;
	}
	/*look here, when we couldn't we claim we are out memory but we can map 
	 * more pages */
	/*heap full?*/
	return NULL; /*default end point*/
}

void
kernel_heap_free(void *ptr)
{
	if (!ptr) return;
	struct header_t *header = (struct header_t *)((char *)ptr - sizeof(struct header_t));
	if (!header)
	{
		kprint("KFREE failed because the hader is NULL\n", BLUE);
		__asm__ volatile ("cli");
		while (1)
			__asm__ volatile ("hlt");
	}
	if (header->magic != HEAP_MAGIC) /* cehcking for corruption */
		panic();
	header->is_free = 1;
	struct header_t *curr = heap_start;
	/* coalescing, to prevent external fragmentation */
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

void
heap_walk(void)
{
	struct header_t *curr = heap_start;
	while (curr)
	{
		kprint("is_free= ", WHITE); kprint_num((uint64_t)curr->is_free,GREEN);
		if (curr->size < 1024){
			kprint(" size= ", WHITE); kprint_num((uint64_t)curr->size, GREEN); kprint(" B", WHITE);
		} else if (curr->size > 1024000) {
			kprint(" size= ", WHITE); kprint_num((uint64_t)curr->size / 1024000, GREEN); kprint(" MiB", WHITE);
		} else {
			kprint(" size= ", WHITE); kprint_num(((uint64_t)curr->size / 1024), GREEN);
			kprint(" KiB", WHITE);
		}
		kprint(" address= ", WHITE); kprint_hex((uint64_t)curr, GREEN);
		kprint(" m= ", WHITE); kprint_hex((uint64_t)curr->magic, GREEN);
		kprint("\n", WHITE);
		if (!curr->next)
			break;
		curr = curr->next;
	}
}
