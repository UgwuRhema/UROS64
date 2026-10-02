#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096ULL
#define MAX_PHYS_ADDR (4ULL * 1024 * 1024 * 1024)
#define TOTAL_PAGES (MAX_PHYS_ADDR / PAGE_SIZE)
#define BITMAP_SIZE (TOTAL_PAGES / 8)

struct MemoryMapEntry {
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed));

void pmm_init(uint32_t, struct MemoryMapEntry *);
void *pmm_alloc(void);
void pmm_free(void *);
void pmm_dump_stats(void);
void pmm_dump_map(void);

#endif
