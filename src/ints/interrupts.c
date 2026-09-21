#include "interrupts.h"
#include <stdatomic.h>

void
idt_set_entry(int index, uint64_t hand_addr, uint16_t selector, uint8_t attr)
{
   idt[index].offset_low = hand_addr & 0xffff; /* low 16 bits */
   idt[index].offset_middle = (hand_addr >> 8) & 0xffff; /* blah blah */
   idt[index].offset_high = (hand_addr >> 16) & 0xffff;
   idt[index].selector = selector;
   idt[index].zero = (uint8_t)0; /* i know this is pointless haha */
   idt[index].type_attr = attr; /* 0x8e for active and 0x0e for inactive */
}

void
idt_load(void)
{
    static struct IDTR idtr;
    idtr.size = (sizeof(struct IDT_Entry) * _IDT_ENTRIES) - 1; /* if you look closely this is the same thing we did for the GDT*/
    /* it was "dw gdt_end - gdt_start - 1" to get the size of the gdt */
    idtr.address = (uint64_t)&idt;

    /* the assembly command... */
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}
