#ifndef VMM_H
#define VMM_H


#include <stdint.h>
#include "pmm.h"

/*type definitions*/
typedef uint64_t u64;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t u8;

/*page table entry present bit*/
#define PTE_PRESENT (1ULL << 0)
/*page table entry writeable bit*/
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER (1ULL << 2)
#define PTE_HUGE (1ULL << 7) /* for 2MiB pages in PD */

/* useful constants */
#define VMM_BASE 0xffff800000000000ULL
#define PML4_IDX(v) (((v) >> 39) & 0x1ff)/*PML4 index*/
#define PDPT_IDX(v) (((v) >> 30) & 0x1ff)/*...*/
#define PD_IDX(v) (((v) >> 21) & 0x1ff)
#define PT_IDX(v) (((v) >> 12) & 0x1ff)

/*initializes thge virtual memory manager*/
extern void vmm_init(void);
/*map and virtual address to a physical address*/
extern int vmm_map(u64, u64, u64);
/*unmap a virtual page*/
extern void vmm_unmap(u64);
/*translates a virtual address to its mapped physical 
 * address */
extern u64 vmm_translate(u64); /* returns 0 if unmapped */

extern void vmm_test(void);

#endif
