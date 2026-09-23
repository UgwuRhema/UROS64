#include "interrupts.h"
#include "handlers.h"
#include "apic.h"

struct IDT_Entry idt[_IDT_ENTRIES];

void
idt_set_entry(int index, uint8_t ist,uint64_t hand_addr, uint16_t selector, uint8_t attr)
{
   idt[index].offset_low = hand_addr & 0xffff; /* low 16 bits */
   idt[index].offset_middle = (hand_addr >> 16) & 0xffff; /* blah blah */
   idt[index].offset_high = (hand_addr >> 32);
   idt[index].ist = ist & 0x07;
   idt[index].selector = selector;
   idt[index].zero = (uint8_t)0x0; /* i know this is pointless haha */
   idt[index].type_attr = attr; /* 0x8e for active and 0x0e for inactive */
}

void
idt_load(void)
{
    static struct IDTR idtr;
    idtr.size = (uint16_t)(sizeof(idt) - 1); /* if you look closely this is the same thing we did for the GDT*/
    /* it was "dw gdt_end - gdt_start - 1" to get the size of the gdt */
    idtr.address = (uint64_t)&idt[0];

    /* the assembly command... */
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}

void
idt_init(void)
{
    __asm__ volatile ("cli"); /* clear interrupts..we don't want those here hehe! */
    int i;
    /* yknow we have to clear all first cause we wont be using all 256 entries so some have to be null */
    for (i = 0; i < _IDT_ENTRIES; ++i)
    {
        /* a null gate: all zeros means not present... */
        idt[i].offset_low   = 0;
        idt[i].selector     = 0;
        idt[i].ist          = 0;
        idt[i].type_attr    = 0;   // present bit cleared = invalid
        idt[i].offset_middle= 0;
        idt[i].offset_high  = 0;
        idt[i].zero         = 0;
    }
    
    /* let's check it out... */
    idt_set_entry(0, 0, (uint64_t)divide_by_zero, 0x08, 0x8e);
    idt_set_entry(1, 0, (uint64_t)debug, 0x08, 0x8e);
    idt_set_entry(2, 0, (uint64_t)non_maskable, 0x08, 0x8e);
    idt_set_entry(3, 0, (uint64_t)break_point, 0x08, 0x8e);
    idt_set_entry(4, 0, (uint64_t)overflow, 0x08, 0x8e);
    idt_set_entry(5, 0, (uint64_t)bound_range_exceeded, 0x08, 0x8e);
    idt_set_entry(6, 0, (uint64_t)invalid_opcode, 0x08, 0x8e);
    idt_set_entry(7, 0, (uint64_t)device_not_available, 0x08, 0x8e);
    idt_set_entry(8, 0, (uint64_t)double_fault, 0x08, 0x8e);
    idt_set_entry(10, 0, (uint64_t)invalid_tss, 0x08, 0x8e);
    idt_set_entry(11, 0, (uint64_t)segement_not_present, 0x08, 0x8e);
    idt_set_entry(12, 0, (uint64_t)stack_segfault, 0x08, 0x8e);
    idt_set_entry(13, 0, (uint64_t)general_protection, 0x08, 0x8e);
    idt_set_entry(14, 0, (uint64_t)page_fault, 0x08, 0x8e);
    idt_set_entry(16, 0, (uint64_t)x87_fpu_error, 0x08, 0x8e);
    idt_set_entry(17, 0, (uint64_t)alignment_check, 0x08, 0x8e);
    idt_set_entry(18, 0, (uint64_t)machine_check, 0x08, 0x8e);
    idt_set_entry(19, 0, (uint64_t)simd_error, 0x08, 0x8e);

    /* hardware IRQs */
    idt_set_entry(33, 0, (uint64_t)keyboard_handler, 0x08, 0x8e);

    idt_load();

    /* disable legacy PIC */
    pic_disable();
    //bring up APIC hardware
    lapic_init();
    /* route keyboard (IRQ 1) to  vector 33 or 0x21 */
    ioapic_map_irq(1, 33, 0);
    /* finally unmask all interrupts */
    __asm__ volatile ("sti");
}
