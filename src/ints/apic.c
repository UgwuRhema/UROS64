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
    wrmsr(IA32_APIC_BASE_MSR, msr | IA32_APIC_BASE_ENABLE);
    /* set task priority register to 0 ( accept all interrupts )*/
    lapic_write(LAPIC_REG_TPR, 0);
    /* set spurious vector to 0xff and turn of software enable bit */
    lapic_write(LAPIC_REG_SVR, 0x1ff);
}

