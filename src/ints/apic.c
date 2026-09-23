#include "apic.h"

inline void
lapic_write(u32 reg, u32 val)
{
    *(volatile u32 *)(LAPIC_BASE + reg) = val;   
}

inline u32
lapic_read(u32 reg)
{
    return *(volatile u32 *)(LAPIC_BASE + reg);
}

void
lapic_init()
{
    /* hardware enable LAPIC in MSR */
    u64 msr = rdmsr(IA32_APIC_BASE_MSR);
}
