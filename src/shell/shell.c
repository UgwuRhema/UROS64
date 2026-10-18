#include "shell.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../memory/heap.h"
#include "../filesystem/ata.h"

char cmd_buf[CMD_MAX_LEN] = {0};
size_t cmd_idx = 0;

void
execute_command(const char *cmd)
{
    if (strlen(cmd) == 0) return;
    if (strcmp(cmd, "help") == 0)
    {
        kprint("UROS USH Available Commands:\n", WHITE);
        kprint(" help            smap_info\n clear           cpuinfo\n sysinfo         off->powerdown\n vreboot         vmm_test\n trigger_*_panic\n powerdown\n sleep <seconds>\n", GREEN);
		kprint(" puts <text>\n sleep <seconds>\n memmap\n", GREEN);
    } else if (strcmp(cmd, "clear") == 0){
        clear_screen();
    } else if (strcmp(cmd, "sysinfo") == 0){
		uint64_t total_seconds = timer_ticks / 1000;
		uint64_t minutes = total_seconds / 60;
		uint64_t hours = minutes / 60;
		uint64_t seconds = total_seconds % 60;
		char cpu_vendor_name[12] = {0}; /* array for the CPU Vendor name */
		char cpu_brand_name[49] = {0};
		get_cpu_vendor_name(cpu_vendor_name); /* the new function we just made to get the name */
		get_cpu_brand_string(cpu_brand_name);
		if (minutes >= 60) { minutes = 0; hours = hours + 1; }
        kp_log("OS: UROS (Unrestricted Runtime Operating System) ", ONLINE);
        kp_log("Kernel: UROS KERNEL(uroskrnl.ubin/uroskrnl) ", ONLINE);
		kprint(" Uptime: ", WHITE); 
		kprint_num(hours, GREEN); kprint("h ", WHITE);
		kprint_num(minutes, GREEN); kprint("m ", WHITE);
		kprint_num(seconds, GREEN); kprint("s\n", WHITE);
        kp_log("Architecture: x86-64 Ring 0 Flat Architecture ", ONLINE);
		kprint(" CPU Vendor Name: ", WHITE);
		kprint(cpu_vendor_name, GREEN); kprint("\n", WHITE);
		kprint(" CPU Brand Name: ", WHITE);
		kprint(cpu_brand_name, GREEN); kprint("\n", WHITE);
        kp_log("APIC Status: LAPIC/IOAPIC Online. ", ONLINE);
        kp_log("Identity Map: 0 - 4GB (2MB Pages) ", ONLINE);
    } else if (strcmp(cmd, "vreboot") == 0){
        kprint("Rebooting...\n", WHITE);
		sleep(1400);
		/* CPU reset through PS/2 controller */
        u8 good = 0x02;
        while (good & 0x02) good = inb(0x64);
        outb(0x64, 0xfe);
    } else if (strcmp(cmd, "linux") == 0 || strcmp(cmd, "linux?") == 0){
		kprint("Nope buddy...this is not Linux...This is UROS\n", WHITE);
		kprint("The Unrestricted Runtime Operating System. \n", GREEN);
		kprint("\"They might be Open Source but we're Unrestricted\"\n", PURPLE);
	} else if (strcmp(cmd, "trigger_pf_panic") == 0){
        volatile u64 *panic_ptr = (volatile u64 *)0x100000000ULL;
        *panic_ptr = 0x1234;
    } else if (strcmp(cmd, "trigger_dz_panic") == 0){
		volatile uint64_t a = 10;
		volatile uint64_t b = 0;
		volatile uint64_t c = a / b;
		kprint_num(c, WHITE);
	} else if (strcmp(cmd, "trigger_uo_panic") == 0){
		__asm__ volatile ("ud2");
	} else if (strcmp(cmd, "powerdown") == 0 || strcmp(cmd, "off") == 0){
		kp_log("Deactivating all services and components", DONE);
		kp_log("Done...You may now turn off the computer", DONE);
		kprint(" Halting...", WHITE);
		sleep(2000); /* 2 seconds */
		/* if on QEMU this shuts down the VM */
		shutdown();
		/* For now: real hardware, clear interrupts and halt */
		__asm__ volatile ("cli");
		while (1)
			__asm__ volatile ("hlt");
	} else if (strcmp(cmd, "heap_test") == 0){
		kprint("HEAP_TEST\n", PURPLE);
		u64 *A = (u64 *)kmalloc(10);
		if (!A) panic();
		u64 *B = (u64 *)kmalloc(10);
		if (!B){ 
			kfree(A);
			panic();
		}
		kprint("Memory Allocated\n", PURPLE);
		kprint("A := ", WHITE); kprint_hex((u64)A, PURPLE); kprint("\n", WHITE);
		kprint("B := ", WHITE); kprint_hex((u64)B, PURPLE); kprint("\n", WHITE);
		kprint("Current Heap State: \n", WHITE);
		heap_walk();
		kprint("\n", WHITE);
		kfree(A); kfree(B);
		kprint("Memory Freed!\n", GREEN);
	} else if (strncmp(cmd, "sleep ", 6) == 0){
		const char *value = &cmd[6]; /* store the number(in seconds as a string for now) */
		uint64_t seconds = string_to_int(value);
		sleep(seconds * 1000);
	} else if (strncmp(cmd, "puts ", 5) == 0){
		const char *val = &cmd[5];
		kprint(val, WHITE);
		kprint("\n", WHITE);
	} else if (strcmp(cmd, "memmap") == 0){
		pmm_dump_map();
	} else if (strcmp(cmd, "vmm_test") == 0){
		vmm_test();
	} else if (strcmp(cmd, "smap_info") == 0){
		smap_info();
	} else if (strcmp(cmd, "heap") == 0){
		heap_walk();
	} else if (strncmp(cmd, "ata_read ", 9) == 0){
		const char *val = &cmd[9]; /* get the LBA we would like to read */
		uint32_t lba = (uint32_t)string_to_int(val);
		kprint("Reading LBA ", WHITE); kprint_num(lba, GREEN); kprint("\n", WHITE);
		uint8_t *buffer = (uint8_t *)kmalloc(512); /* allocate memory for the buffer */
		int read = ata_read_sector(lba, buffer);
		if (read < 0)
			kp_log("(DRV)ATA PIO Driver LBA sector read fail. ", NOT_DONE);
		else {
			kp_log("(DRV)ATA PIO Driver LBA sector read success. ", DONE);
			kprint("Contents: \n", PURPLE);
			/* let's try showing each character by character */
			for (int i = 0; i < 512; ++i)
				kprint_num((uint64_t)buffer[i], GREEN);
			kprint("\n", WHITE); 
			kfree((void *)buffer);
			kprint("Buffer freed\n", WHITE);
		}
	} else if (strncmp(cmd, "ata_write ", 10) == 0){
        const char *val = &cmd[10];
        uint32_t lba = (uint32_t)string_to_int(val);
        uint8_t *buffer = (uint8_t *)kmalloc(512);/*this is the buffer that will be written at the give lba*/
        int write = ata_write_sector(lba, buffer);
        if (write < 0)
            kp_log("(DRV)ATA PIO Driver LBA Sector write fail. ", NOT_DONE);
        else if (write > 0)
            kp_log("(DRV)ATA PIO Driver timeout during Status polling. ", NOT_DONE);
        else {
            kp_log("(DRV)ATA PIO Driver LBA Sector write success. ", DONE);
            kfree((void *)buffer);
        }
    } else if (strncmp(cmd, "tmp_alloc ", 10) == 0){
		const char *val = &cmd[10];
		if (!val || *val == ' ')
			kprint("Usage: tmp_alloc <bytes>\n", BLUE);
		else {
			uint64_t bytes = string_to_int(val);
			uint64_t *temp_pointer = (uint64_t *)kmalloc(bytes);
			kprint("Allocated temporary memory for ", WHITE); kprint_num(bytes, WHITE);
			kprint(" bytes of unsigned 64 bit integers\n", WHITE);
			kprint("Warning: This Pointer will not be freed so it will cause memory leaks. \n", PURPLE);
			kprint("Address: ", WHITE); kprint_hex((uint64_t)temp_pointer
					, BLUE); kprint("\n", WHITE);
			kprint("Address_Id: ", WHITE); kprint_num((uint64_t)temp_pointer, BLUE);
			kprint("\n", WHITE);
		}
	} else if (strncmp(cmd, "tmp_free ", 9) == 0){
		uint64_t addr = string_to_int(&cmd[9]);
		/* so currently that's a string and we need to convert it to a void *, so that kfree can de-allocate it */
		if (addr == 0)
			kprint("Usage: tmp_free <address>\nNote: tmp_alloc returns the address_id which is just the address in decimal, pass it as the argument to tmp_free.\n", BLUE);
		else {
			kfree((void *)addr);
			kprint("Memory region ", WHITE); kprint_hex(addr, WHITE); kprint(" freed\n", WHITE);
		}
	} else if (strcmp(cmd,"cpuinfo") == 0){
		char vendor[12];
		char brand[49];
		get_cpu_vendor_name(vendor);
		get_cpu_brand_string(brand);
		kprint(" CPU Vendor Name: ", WHITE);
		kprint(vendor, GREEN); kprint("\n", WHITE);
		kprint(" CPU Brand Name: ", WHITE);
		kprint(brand, GREEN); kprint("\n", WHITE);
	} else {
        kprint("Unknown Command: ", PURPLE);
        kprint("'", PURPLE);
        kprint(cmd, PURPLE);
        kprint("'. Type 'help' for available commands. \n", PURPLE);
    }
}

void
shell_init(void)
{
    //kprint("shell_init: saved=", WHITE); kprint_hex((uint64_t)smap_saved, WHITE);
    //kprint(" count=", WHITE); kprint_num(smap_saved_count, WHITE);
    //kprint(" free=", WHITE); kprint_num(free_pages, WHITE);
    //kprint("\n", WHITE);
	memset(cmd_buf, 0, CMD_MAX_LEN);
    cmd_idx = 0;
    kprint("[Noth:/Uros64]# ", WHITE);
}

void
shell_update(void)
{
    char c= buffer_pop();
    if (!c) return; /* buffer empty */
    if (c == '\n')
    {
        kprint("\n", WHITE);
        cmd_buf[cmd_idx] = '\0';
        execute_command(cmd_buf);
        /* reset command line */
        cmd_idx = 0;
        memset(cmd_buf, 0, CMD_MAX_LEN);
        kprint("[Noth:/Uros64]# ", WHITE);
    } else if (c == '\b'){
        if (cmd_idx > 0)
        {
            cmd_idx--;
            cmd_buf[cmd_idx] = '\0';
            kprint("\b", WHITE); /* backspace */
        }
    } else {
        if (cmd_idx < CMD_MAX_LEN - 1)
        {
            cmd_buf[cmd_idx++] = c;
            /* print key to terminal */
            char str[2] = {c, '\0'};
            kprint(str, WHITE);
        }
    }
}
