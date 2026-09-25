#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;

//#include "../vga/vga.h"
#include "../func/func.h"
#include "../ints/interrupts.h"
#include "../shell/shell.h"

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
kmain()
{
	/* enable_pit(1000); */
	idt_init();
	kprint("Kernel loading...\n" , WHITE);
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	kprint("x86 Monolithic 64-bit Kernel written in URSL\n", WHITE);
	/*kprint("\n", WHITE); */
	kprint("Setting up... \n", WHITE);
	kp_log("Enable 64-bit Paging. ", DONE);
	sleep(100);
	kp_log("Setup up VGA and VGA functions. ", DONE);
	sleep(100);
	kp_log("Set up Interrupt Descriptor Table and load CPU Exceptions. ", DONE);
	sleep(100);
	kp_log("Enable LAPIC and IOAPIC Hardware Interrupts. ", DONE);
	sleep(100);
	kp_log("Set up PS/2 Keyboard Driver and Model. ", DONE);
	sleep(100);
	kp_log("Initialized the PIT(Programmable Interval Timer). ", DONE);
	sleep(100);
	kp_log("Implement the Memory Management Unit. ", NOT_DONE);
	sleep(100);
	kp_log("Same as the last one; Implement PMM and VMM. ", NOT_DONE);
	sleep(100);
	kp_log("Starting shell...", DONE);
	sleep(900);
	shell_init();
		
    /* infinte loop */
	while (1)
	{
		shell_update(); /* process queued characters from key_buffer */
		__asm__ volatile ("hlt");
	}
}
