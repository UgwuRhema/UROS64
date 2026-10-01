#include "pmm.h"
#include "../vga/vga.h"

/* 128 KiB in .bss */
static uint8_t pmm_bitmap[BITMAP_SIZE];

/* important bitmap helpers */
static inline void bitmap_set(uint64_t i) { pmm_bitmap[i >> 3] |= (1u << (i & 7)); }
static inline void bitmap_clear(uint64_t i) { pmm_bitmap[i >> 3] &= ~(1u<<(i & 7)); }
static inline int bitmap_test(uint64_t i) { return (pmm_bitmap[i >> 3] >> (i & 7)) & 1; }

/* we will add these to the linker script */
extern char __kernel_start[];
extern char __kernel_end[];

static uint64_t free_pages = 0;

void
pmm_init(uint32_t count, struct MemoryMapEntry *entries)
{
	/* everything is used by default */
	for (uint64_t i = 0; i < BITMAP_SIZE; ++i) *(pmm_bitmap + i) = 0xff;

	for (uint32_t i = 0; i < count; ++i)
	{
		if (entries[i].type != 1) continue; /* type 1 means available RAM per E820/BIOS memory map*/
		uint64_t start = (*(entries + i)).base_address;
		uint64_t end = (*(entries + i)).base_address + (*(entries + i)).length;

		if (start >= MAX_PHYS_ADDR) continue;
		if (end > MAX_PHYS_ADDR) end = MAX_PHYS_ADDR;
		
		/* we now align page...start up and end down */
		start = (start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
		end = end & ~(PAGE_SIZE - 1);

		for (uint64_t a = start; a < end; a += PAGE_SIZE)
		{
			bitmap_clear(a / PAGE_SIZE);
			free_pages++;
		}
	}

	/* here we now re-reserve non-free memory... */
	/* first 1mb, BIOS, VGA, IVT, BDA, and also uboot(out bootloader), page tables... */
	for (uint64_t a = 0; a < 0x100000; a += PAGE_SIZE)
	{
		if (!bitmap_test(a / PAGE_SIZE)) { bitmap_set(a /PAGE_SIZE); free_pages--; }
	}

	/* kernel image itself */
	uint64_t kstart = (uint64_t)__kernel_start;
	uint64_t kend = (uint64_t)__kernel_end;
	kstart &= ~(PAGE_SIZE - 1);
	kend = (kend + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	for (uint64_t a = kstart; a < kend; a += PAGE_SIZE)
	{
		if (!bitmap_test(a / PAGE_SIZE)) { bitmap_set(a / PAGE_SIZE); free_pages--; }
	}
}

void *
pmm_alloc(void)
{
	for (uint64_t i = 0; i < TOTAL_PAGES; ++i)
	{
		if (!bitmap_test(i))
		{
			bitmap_set(i);
			free_pages--;
			return (void*)(i * PAGE_SIZE);
		}
	}
	return NULL;
}

void
pmm_free(void *page)
{
	uint64_t i = (uint64_t)page / PAGE_SIZE;
	if (i >= TOTAL_PAGES) return;
	if (bitmap_test(i))
	{
		bitmap_clear(i);
		free_pages++;
	}
}

/* this method should be self explanatory */
void
pmm_dump_stats(void)
{
	kprint("PMM: total pages = ", WHITE);
	kprint_hex(TOTAL_PAGES, GREEN);
	kprint("\nPMM: free pages = ", WHITE);
	kprint_hex(free_pages, GREEN);
	kprint("\n", WHITE);

	/* let's try some tests here */
	void *a = pmm_alloc();
	void *b = pmm_alloc();
	pmm_free(a);
	void *c = pmm_alloc();

	kprint("PMM: alloc a = ", WHITE); kprint_hex((uint64_t)a, GREEN);
	kprint("  b = ", WHITE); kprint_hex((uint64_t)b, GREEN);
	kprint("  c = ", WHITE); kprint_hex((uint64_t)c, GREEN);
	kprint("\n", WHITE);

	if (a == c && a != b) kprint("PMM: self-test OK\n", GREEN);
	else kprint("PMM: self-test FAILED\n", GREEN);
}
