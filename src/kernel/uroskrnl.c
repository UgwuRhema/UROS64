#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;

//#include "../vga/vga.h"
#include "../func/func.h"
#include "../ints/interrupts.h"
#include "../shell/shell.h"
#include "../memory/pmm.h"

/*
uint64_t get_uptime_in_minutes(uint64_t timer_ticks)
{
	uint64_t total_seconds =  timer_ticks / 1000;
	uint64_t minutes = total_seconds / 60;
	uint64_t seconds = total_seconds % 60;
	return minutes;
}
*/

void
uroskrnl_main_entry_point(uint32_t memory_entries_count, struct MemoryMapEntry *mmap_entries)
{
	/* enable_pit(1000); */
	idt_init();
	kprint("Kernel loading...\n" , WHITE);
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	kprint("x86 Monolithic 64-bit Kernel written in URSL\n", WHITE);
	/*kprint("\n", WHITE); */
	skprint("Entering setup....\n");
	kprint("Setting up... \n", WHITE);
	kp_log("(MEM)Enable 64-bit Paging. ", DONE);
	sleep(100);
	kp_log("(VGA)Setup up VGA and VGA functions. ", DONE);
	sleep(100);
	kp_log("(INT)Set up Interrupt Descriptor Table and load CPU Exceptions. ", DONE);
	sleep(100);
	kp_log("(INT)Enable LAPIC and IOAPIC Hardware Interrupts. ", DONE);
	sleep(100);
	kp_log("(DRVS)Set up PS/2 Keyboard Driver and Model. ", DONE);
	sleep(100);
	kp_log("(INT)Initialized IRQ 2 and the PIT(Programmable Interval Timer). ", DONE);
	sleep(100);
	kp_log("(DRVS)Activated the COM1 and initialize the Serial Driver. ", DONE);
	sleep(100);
	kp_log("(DRVS)Initailized the PS/2 Mouse Interrupt Handler and Driver. ", DONE);
	sleep(100);
	kp_log("(MEM)Memory Map Entries Online. ", ONLINE);
	kprint("SMAP count = ", WHITE); kprint_hex(memory_entries_count, PURPLE); kprint("\n", WHITE);
	for (uint32_t i = 0; i < memory_entries_count; i++) {
		kprint("  base_address=", WHITE); kprint_hex(mmap_entries[i].base_address, PURPLE);
		kprint(" len=",  WHITE); kprint_hex(mmap_entries[i].length, PURPLE);
		kprint(" type=", WHITE); kprint_hex(mmap_entries[i].type, PURPLE);
		kprint("\n", WHITE);
		sleep(100); /* just for realism */
	}
	kp_log("(MEM)Implement the Memory Management Unit. ", NOT_DONE);
	sleep(100);
	kp_log("(MEM)Same as the last one; Implement PMM and VMM. ", NOT_DONE);
	sleep(100);
	kp_log("Starting shell...", ONLINE);
	sleep(900);
	shell_init();
		
    /* infinte loop */
	while (1)
	{
		shell_update(); /* process queued characters from key_buffer */
		__asm__ volatile ("hlt");
	}
}
