#include <unistd.h>

typedef uint64_t u64;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t u8;

#define PTE_PRESENT (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER (1ULL << 2)
#define PTE_HUGE (1ULL << 7) /* for 2MiB pages in PD */

extern void vmm_map(u64, u64, u64);
extern void vmm_unmap(u64);
u64 vmm_translate(u64); /* returns 0 if unmapped */
