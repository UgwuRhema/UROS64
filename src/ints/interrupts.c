#include "interrupts.h"
#include "handlers.h"

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

    idt_load();
    __asm__ volatile ("sti");
}
