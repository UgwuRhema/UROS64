#include "shell.h"

static void execute_command(const char *cmd)
{
    if (strlen(cmd) == 0) return;

    if (strcmp(cmd, "help") == 0)
    {
        kprint("USH(Unrestrcited SHell) Commands:\n", WHITE);
        kprint("  help, clear, sysinfo, vreboot\n", GREEN);
    } else if (strcmp(cmd, "clear") == 0){
        clear_screen();
    } else if (strcmp(cmd, "sysinfo") == 0){
        kp_log("OS: UROS (Unrestricted Runtime Operating System) ", ONLINE);
        kp_log("Kernel: UROS KERNEL(uroskrnl.ubin/uroskrnl) ", ONLINE);
        kp_log("Architecture: x86-64 Ring 0 Flat Architecture ", ONLINE);
        kp_log("APIC Status: LAPIC/IOAPIC Online. ", ONLINE);
        kp_log("Identity Map: 0 - 4GB (2MB Pages) ", ONLINE);
    } else if (strcmp(cmd, "vreboot") == 0){
        /* CPU reset through PS/2 controller */
        u8 good = 0x02;
        while (good & 0x02) good = inb(0x64);
        outb(0x64, 0xfe);
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
    kprint("\nUSH (Unreastricted Shell) v0.1\n", GREEN);
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
