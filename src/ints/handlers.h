#include "../vga/vga.h"
#include "interrupts.h"

extern __attribute__((interrupt)) void divide_by_zero(void *);
