#include "shell.h"

char cmd_buf[CMD_MAX_LEN] = {0};
size_t cmd_idx = 0;

void
execute_command(const char *cmd)
{
    if (strlen(cmd) == 0) return;

    if (strcmp(cmd, "help") == 0)
    {
        kprint("UROS USH Available Commands:\n", WHITE);
        kprint(" help\n clear\n sysinfo\n vreboot\n trigger_*_panic\n", GREEN);
    } else if (strcmp(cmd, "clear") == 0){
        clear_screen();
    } else if (strcmp(cmd, "sysinfo") == 0){
		uint64_t total_seconds = timer_ticks / 1000;
		uint64_t minutes = total_seconds / 60;
		uint64_t seconds = total_seconds % 60;
        kp_log("OS: UROS (Unrestricted Runtime Operating System) ", ONLINE);
        kp_log("Kernel: UROS KERNEL(uroskrnl.ubin/uroskrnl) ", ONLINE);
		kprint(" Uptime: ", WHITE); kprint_num(minutes, GREEN); kprint("m ", WHITE);
		kprint_num(seconds, GREEN); kprint("s\n", WHITE);
        kp_log("Architecture: x86-64 Ring 0 Flat Architecture ", ONLINE);
        kp_log("APIC Status: LAPIC/IOAPIC Online. ", ONLINE);
        kp_log("Identity Map: 0 - 4GB (2MB Pages) ", ONLINE);
    } else if (strcmp(cmd, "vreboot") == 0){
        /* CPU reset through PS/2 controller */
        u8 good = 0x02;
        while (good & 0x02) good = inb(0x64);
        outb(0x64, 0xfe);
    } else if (strcmp(cmd, "trigger_pf_panic") == 0){
        volatile u64 *panic_ptr = (volatile u64 *)0x100000000ULL;
        *panic_ptr = 0x1234;
    } else if (strcmp(cmd, "powerdown") == 0){
		kp_log("Deactivating all services and components", DONE);
		kp_log("Clear interrupts and Halt. You may now turn off the computer", DONE);
		/* if on QEMU this shuts down the VM */
		shutdown();
		/* For now: real hardware, clear interrupts and halt */
		__asm__ volatile ("cli");
		while (1)
			__asm__ volatile ("hlt");
	} else {
        kprint("Unknown Command: ", PURPLE);
        kprint("'", WHITE);
        kprint(cmd, WHITE);
        kprint("'. Type 'help' for available commands. \n", WHITE);
    }
}

void
shell_init(void)
{
    memset(cmd_buf, 0, CMD_MAX_LEN);
    cmd_idx = 0;
    kprint("[Eden:/Uros64]# ", WHITE);
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
        kprint("[Eden:/Uros64]# ", WHITE);
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
