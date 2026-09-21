#include <stddef.h>
#include <stdint.h>

#define _IDT_ENTRIES 256

/* this is the interrupt descriptor table entry... */
/* this is the table that tells the CPU where to jump when an interrupt fires */
/* each entry is a like a function pointer split into two halves plus a segment selector(0x10 or 0x08(it's 0x08 we use though))*/
struct __attribute__((packed)) IDT_Entry
{
    uint16_t offset_low; /* low 16 bits of handler address */
    uint16_t selector; /* like i said 0x08 */
    uint8_t ist; /* ist index (priviledged array of registers to special exceptions/interrupts) */
    uint8_t type_attr; /* apparently this is the interrupt gate... */
    uint16_t offset_middle; /* since we are doing 64 bit, this has to be here*/ 
    uint32_t offset_high; /* high 16 bits of the handler address */
    uint32_t zero; /* reserved must be zero */
} static idt[_IDT_ENTRIES]; /* always 256 IDT Entries both on 32 and 64 bit OSes(not that it matters the architecture) */

/* you know...the register...*/
/* like how we did lgdt [gdt_decriptor], we'll do that same thing for the IDT */
struct __attribute__((packed)) IDTR
{
    uint16_t size;
    uint64_t address;
};

extern void idt_set_entry(int, uint8_t, uint64_t, uint16_t, uint8_t); /* function to set 1 IDT Entry... */
extern void idt_load(void);
extern void idt_init(void);
