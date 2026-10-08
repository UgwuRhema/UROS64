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
static struct MemoryMapEntry *smap_saved = NULL;
static uint32_t smap_saved_count = 0;
static uint64_t pmm_canary = 0xdeadbeef;

void
pmm_init(uint32_t count, struct MemoryMapEntry *entries)
{
	kprint("pmm_init: count=", WHITE); kprint_num(count, WHITE);
	kprint(" entries=", WHITE); kprint_hex((uint64_t)entries, WHITE);
	kprint("\n", WHITE);
	/* save entries state, not for anything in particular */
	smap_saved = entries;
	smap_saved_count = count;

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

	/* at the very end of pmm_init, after the kernel reservation loop */
    kprint("pmm_init END: saved=", WHITE); kprint_hex((uint64_t)smap_saved, WHITE);
    kprint(" count=", WHITE); kprint_num(smap_saved_count, WHITE);
    kprint(" free=", WHITE); kprint_num(free_pages, WHITE);
    kprint("\n", WHITE);
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

static void
kprint_size(uint64_t bytes)
{
    if (bytes >= (1ULL << 30)) {
        uint64_t gb_whole = bytes >> 30;
        uint64_t gb_frac  = (bytes >> 20) & 0x3FF;   /* 0..1023 */
        uint64_t tenths   = (gb_frac * 10) / 1024;   /* 0..9 */
        kprint_num(gb_whole, WHITE);
        kprint(".", WHITE);
        kprint_num(tenths, WHITE);
        kprint(" GB", WHITE);
    } else if (bytes >= (1ULL << 20)) {
        kprint_num(bytes >> 20, WHITE);
        kprint(" MB", WHITE);
    } else if (bytes >= (1ULL << 10)) {
        kprint_num(bytes >> 10, WHITE);
        kprint(" KB", WHITE);
    } else {
        kprint_num(bytes, WHITE);
        kprint(" B", WHITE);
    }
}

void
pmm_dump_map(void)
{
	kprint("dump_map: ptr=", WHITE); kprint_hex((uint64_t)smap_saved, WHITE);
	kprint(" count=", WHITE); kprint_num(smap_saved_count, WHITE);
	kprint(" free=", WHITE); kprint_num(free_pages, WHITE);
	kprint("\n", WHITE);
    kprint("Memory Map\n", GREEN);

    /* Compute installed usable RAM from SMAP */
    uint64_t usable_bytes = 0;
    uint32_t usable_regions = 0;
    for (uint32_t i = 0; i < smap_saved_count; ++i) {
		if (smap_saved[i].type != 1) continue;

		uint64_t start = smap_saved[i].base_address;
		uint64_t end = smap_saved[i].base_address + smap_saved[i].length;
		if (end > MAX_PHYS_ADDR) end = MAX_PHYS_ADDR;
		if (start >= end) continue;

		usable_bytes += end - start;
		usable_regions++;
    }

    /* Summary */
    kprint("Bitmap covers:  ", WHITE);
    kprint_size(TOTAL_PAGES * PAGE_SIZE);
    kprint(" (", WHITE); kprint_num(TOTAL_PAGES, WHITE); kprint(" pages)\n", WHITE);

    kprint("Installed RAM:  ", WHITE);
    kprint_size(usable_bytes);
    kprint(" (", WHITE); kprint_num(usable_bytes / PAGE_SIZE, WHITE); kprint(" pages)\n", WHITE);

    kprint("Free:           ", WHITE);
    kprint_size(free_pages * PAGE_SIZE);
    kprint(" (", WHITE); kprint_num(free_pages, WHITE); kprint(" pages)\n", WHITE);

    kprint("Used/Reserved:  ", WHITE);
    kprint_size((TOTAL_PAGES - free_pages) * PAGE_SIZE);
    kprint(" (", WHITE); kprint_num(TOTAL_PAGES - free_pages, WHITE); kprint(" pages)\n", WHITE);

    /* Per-region dump */
    kprint("\nSMAP regions (", WHITE);
    kprint_num(smap_saved_count, WHITE);
    kprint(" total, ", WHITE);
    kprint_num(usable_regions, WHITE);
    kprint(" usable):\n", WHITE);

    for (uint32_t i = 0; i < smap_saved_count; ++i) {
        struct MemoryMapEntry *e = &smap_saved[i];

        if (e->type == 1) kprint("  [usable]   ", GREEN);
        else              kprint("  [reserved] ", WHITE);

        kprint_hex(e->base_address, WHITE);
        kprint(" - ", WHITE);
        kprint_hex(e->base_address + e->length, WHITE);
        kprint("  ", WHITE);
        kprint_size(e->length);
        kprint("\n", WHITE);
    }
}

/*  i need this debug function... */
void
pmm_debug_state(const char *tag)
{
    kprint("[", WHITE); kprint(tag, WHITE); kprint("] ", WHITE);
    kprint("saved=", WHITE); kprint_hex((uint64_t)smap_saved, WHITE);
    kprint(" count=", WHITE); kprint_num(smap_saved_count, WHITE);
    kprint(" free=", WHITE); kprint_num(free_pages, WHITE);
    kprint(" canary=", WHITE); kprint_hex(pmm_canary, WHITE);
    kprint("\n", WHITE);
}

void
smap_info(void)
{
	kprint("SMAP count = ", WHITE); kprint_hex(smap_saved_count, PURPLE); kprint("\n", WHITE);
	for (uint32_t i = 0; i < smap_saved_count; i++) {
		kprint("  base_address=", WHITE); kprint_hex(smap_saved[i].base_address, PURPLE);
		kprint(" len=",  WHITE); kprint_hex(smap_saved[i].length, PURPLE);
		kprint(" type=", WHITE); kprint_hex(smap_saved[i].type, PURPLE);
		kprint("\n", WHITE);
	}
}
