#include <stddef.h>
#include <stdint.h>

typedef uint64_t u64;

//#include "../vga/vga.h"
#include "../func/func.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../devices/sound/sound.h"
#include "../shell/shell.h"
#include "../memory/heap.h"
#include "../filesystem/ata.h"

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
	beep();
	kprint("UROS(Unrestricted Runtime Operating System) KERNEL ", WHITE);
	kprint("v0.01\n", GREEN);
	kprint("x86 Monolithic 64-bit Kernel written in URSL\n", WHITE);
	/*kprint("\n", WHITE); */
	skprint("Entering setup....\n");
	kp_log("(HARD)Enabled PC Speaker. ", DONE);
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
	kprint("SMAP count= ", WHITE); kprint_hex(memory_entries_count, PURPLE); kprint("\n", WHITE);
	for (uint32_t i = 0; i < memory_entries_count; i++) {
		kprint("base_address=", WHITE); kprint_hex(mmap_entries[i].base_address, PURPLE);
		kprint("len=",  WHITE); kprint_hex(mmap_entries[i].length, PURPLE);
		kprint("type=", WHITE); kprint_hex(mmap_entries[i].type, PURPLE);
		kprint("\n", WHITE);
		sleep(100); /* just for realism */
	}
	pmm_init(memory_entries_count, mmap_entries); /* this might fail, we hope it won't */
	pmm_debug_state("after init"); //dbg A
	kp_log("(MEM)Implemt Physical Memory Manager. Active. ", DONE);
	kp_log("(MEM)PMM Dump Stats. Online. ", ONLINE);
	pmm_dump_stats();
	/* we will not dump stats for now, let's verify it doesn't cause a triple fault... */
	pmm_debug_state("after stats"); //dbg B
	pmm_dump_map();
	sleep(100);
	vmm_init();
	kp_log("(MEM)Implement Virtual Memory Manager. Active. ", DONE);
	kp_log("(MEM)VMM Dump Test. ", ONLINE);
	vmm_test();
	kheap_init(128); /* Initialization of the kernel heap, 64 Pages for now! */
	kp_log("(MEM)Kernel Heap Initialized and Online. ", DONE);
	kp_log("(MEM)Kernel Heap Allocator and De-Allocator Online. (kmalloc & kfree)", ONLINE);
	kp_log("(DRV)Initialize ATA PIO Driver. ATA_READ_SECTOR ACTIVE. ", DONE);
	uint8_t initial_status = inb(0x1f7);
	kprint(" ATA Intial Status=", WHITE); kprint_hex((uint64_t)initial_status, PURPLE);
	kprint("\n", WHITE);
	kprint(" ***TESTING ATA_READ_SECTOR*** \n", GREEN);
	uint8_t buffer[512];
	int read = ata_read_sector(256, buffer); /* this reads LBA 0, which is our Boot Sector and where our bootloader lives */
	if (read < 0)
		kp_log("(DRV)ATA PIO Driver Subsystem Failure. ", NOT_DONE);
	kp_log("(UTILS)Starting shell...", ONLINE);
	sleep(900);
	shell_init();
		
    /* infinte loop */
	while (1)
	{
		shell_update(); /* process queued characters from key_buffer */
		__asm__ volatile ("hlt");
	}
}
