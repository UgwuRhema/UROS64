#ifndef VMM_H
#define VMM_H


#include <stdint.h>
#include "pmm.h"

typedef uint64_t u64;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t u8;

#define PTE_PRESENT (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER (1ULL << 2)
#define PTE_HUGE (1ULL << 7) /* for 2MiB pages in PD */

/* useful constants */
#define VMM_BASE 0xffff800000000000ULL
#define PML4_IDX(v) (((v) >> 39) & 0x1ff)
#define PDPT_IDX(v) (((v) >> 30) & 0x1ff)
#define PD_IDX(v) (((v) >> 21) & 0x1ff)
#define PT_IDX(v) (((v) >> 12) & 0x1ff)

extern void vmm_init(void);
extern void vmm_map(u64, u64, u64);
extern void vmm_unmap(u64);
extern u64 vmm_translate(u64); /* returns 0 if unmapped */

#endif
