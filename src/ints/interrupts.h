#include <stddef.h>
#include <stdint.h>

/* this is the interrupt descriptor table entry... */
/* this is the table that tells the CPU where to jump when an interrupt fires */
/* each entry is a like a function pointer split into two halves plus a segment selector(0x10 or 0x08(it's 0x08 we use though))*/
typedef struct
{
    uint32_t offset_low; /* low 16 bits of handler address */
    uint16_t selector; /* like i said 0x08 */
    uint8_t zero; /* default must be zero */
    uint8_t type_attr; /* apparently this is the interrupt gate... */
    uint32_t offset_high; /* high 16 bits of the handler address */
} __attribute__((packed)) IDT_Entry;

/* you know...the register...*/
/* like how we did lgdt [gdt_decriptor], we'll do that same thing for the IDT */
typedef struct
{
    uint16_t size;
    uint64_t address;
} __attribute__((packed)) IDTR;
