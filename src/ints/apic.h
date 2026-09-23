#include <stdint.h>
#include "../func/func.h"

#define IA32_APIC_BASE_MSR 0x1b 
#define IA32_APIC_BASE_ENABLE (1ULL << 11)

#define LAPIC_BASE 0xfee00000ULL
#define LAPIC_REG_TPR 0x0080 /* task priority register */
#define LAPIC_REG_EOI 0x00b0 /* end of interrupt register */
#define LAPIC_REG_SVR 0x00f0 /* spurious interrupt vector register */

#define IOAPIC_BASE 0xfec00000ULL

/* disable PIC */
void pic_disable(void);
/* write and read to and from the LAPIC respectively */
void lapic_write(u32 reg, u32 val);
u32 lapic_read(u32 reg);
/* initialize the Local Advanced Programmable Interrupt Controller(LAPIC) */
//void get_lapic_base();
void lapic_init(void);
/* IOAPIC(I/O Advanced Programmable Interrupt Controller) */
/* ioapic init functions */
extern inline void ioapic_write(u8, u32);
extern inline u32 ioapic_read(u8);
/* map an APIC IRQ just like normal PIC IRQs(eg. keyboard Handler on vector 0x21) */
extern void ioapic_map_irq(u8, u8, u8);
/* send an EOI(End Of Interrupt) to the CPU */
extern void lapic_eoi(void);
