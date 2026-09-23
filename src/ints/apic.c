#include "apic.h"

/* disable the 8259 IBM PIC */
void
pic_disable()
{
    /* mask all interrupts */
    outb(0x21, 0xff);
    outb(0xa1, 0xff);
}

void
lapic_write(u32 reg, u32 val)
{
    *(volatile u32 *)(LAPIC_BASE + reg) = val;   
}
u32
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

inline void
ioapic_write(u8 reg, u32 val)
{
    *(volatile u32 *)(IOAPIC_BASE + 0x00) = reg; /* set index */
    *(volatile u32 *)(IOAPIC_BASE + 0x10) = val; /* write data */
}

inline u32
ioapic_read(u8 reg)
{
    *(volatile u32 *)(IOAPIC_BASE + 0x00) = reg; /* set index */
    return *(volatile u32 *)(IOAPIC_BASE + 0x10);
}

void
ioapic_map_irq(u8 irq, u8 vector, u8 target_lapic_id)
{
    u8 redirection_reg = 0x10 + (irq * 2); /* each entry is 64 bits (2 32 bit registers)*/
    /* lower 32 bits: vector number, unmask bit (0 = unmasked) */
    u32 low_bits = vector;
    u32 high_bits = ((u32)target_lapic_id) << 24; /* dest LAPIC ID shifted to highes byte */
    ioapic_write(redirection_reg, low_bits);
    ioapic_write(redirection_reg + 1, high_bits);
}

void
lapic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}
