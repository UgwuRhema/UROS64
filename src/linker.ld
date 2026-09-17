SECTIONS
{
    . = 0x10000;
    
    .text : ALIGN(4) {
        *(.text.boot)
        *(.text)
    }
    
    .data : ALIGN(4) {
        *(.data)
    }
    
    .bss : ALIGN(4) {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }
}

