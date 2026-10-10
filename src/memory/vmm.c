#include "vmm.h"
#include "../vga/vga.h"

static u64 *kernel_pml4; /* whatever CR3 points at after boot */

void 
vmm_init(void)
{
	__asm__ volatile ("mov %%cr3, %0" : "=r"(kernel_pml4));
	/* kernel_pml4 is a physica address: identity map means it's also virtual */
}

/* walk to the page table one level above the final PT */
/* allocate intermediate tables on demand */
static u64 *
walk_to_pt(u64 virt, int create)
{
	u64 *pml4 = (u64 *)kernel_pml4;
	u64 *pdpt;
	u64 *pd;
	u64 *pt;


}
