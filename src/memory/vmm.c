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

	u64 i4 = PML4_IDX(virt);
	u64 i3 = PDPT_IDX(virt);
	u64 i2 = PD_IDX(virt);
	
	/* Page Map Level 4 Entry */
	if (!(pml4[i4] & PTE_PRESENT))
	{
		if (!create) return NULL;
		u64 *new = pmm_alloc();
		for (int j = 0; j < 512; ++j) new[j] = 0;
		pml4[i4] = (u64)new | PTE_PRESENT | PTE_WRITABLE;
	}

	/* Page Directory Pointer Table */
	pdpt = (u64 *)(pml4[i4] & ~0xfffULL);


	if (!(pdpt[i3] & PTE_PRESENT))
	{
		if (!create) return NULL;
		u64 *new = pmm_alloc();
		for (int j = 0; j < 512; ++j) new[j] = 0;
		pdpt[i3] = (u64)new | PTE_PRESENT | PTE_WRITABLE;
	}

	/* Page Directory */
	pd = (u64 *)(pdpt[i3] & ~0xfffULL);

	if (!pd[i2] & PTE_PRESENT)
	{
		if (!create) return NULL;
		u64 *new = pmm_alloc();
		for (int j = 0; j < 512; ++j) new[j] = 0;
		pd[i2] = (u64)new | PTE_PRESENT | PTE_WRITABLE;
	}

	/* Page Table */
	pt = (u64 *)(pd[i2] & ~0xfffULL);

	return pt;
}

void
vmm_map(u64 virt, u64 phys, u64 flags)
{
	/* TODO */
}

void
vmm_unmap(u64 virt)
{
	/* TODO: I will do stuff here, i gotta go read! */
}

u64 
vmm_translate(u64 virt)
{
	/* stuff */
}
