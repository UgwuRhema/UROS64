#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096ULL
#define MAX_PHYS_ADDR (4ULL * 1024 * 1024 * 1024)
#define TOTAL_PAGES (MAX_PHYS_ADDR / PAGE_SIZE)
#define BITMAP_SIZE (TOTAL_PAGES / 8)

/*Memory Map Entry struct from BIOS SMAP*/
struct MemoryMapEntry_t {
    uint64_t base_address; /*base address*/
    uint64_t length;
    uint32_t type; /*1 or 0*/
    uint32_t acpi;
} __attribute__((packed));

/*initialize the physcial memory manager*/
void pmm_init(uint32_t, struct MemoryMapEntry_t *);
/*allocate a 4KB Physical Page Frame*/
void *pmm_alloc(void);
/*free an allocated page frame
 * probably just setting it to free */
void pmm_free(void *);
/*pmm test functions*/
void pmm_dump_stats(void);
void pmm_dump_map(void);
void pmm_debug_state(const char *);
void smap_info(void); /*...*/

#endif
