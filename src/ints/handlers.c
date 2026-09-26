#include "handlers.h"
#include "apic.h"

void
panic_execption(const char *title, uint8_t vector, struct interrupt_frame *frame, uint64_t additional_info)
{
	__asm__ volatile ("cli");
	clear_screen();

    kprint("Kernel Panic\n", PURPLE);
    kprint("Exception=", WHITE); kprint(title, WHITE); kprint("\n", WHITE);
    kprint("Vector=#", WHITE); kprint_num(vector, WHITE); kprint("\n", WHITE);
    kprint("Error=", WHITE); kprint_hex(frame->error_code, PURPLE); kprint("\n", WHITE);

    /* Page Fault specific breakdown */
		if (vector == 14 || vector == 13) {
			kprint("Fault-Address=", WHITE); kprint_hex(additional_info, PURPLE); kprint("\n", WHITE);
        kprint("Fault-Cause=", WHITE);
        kprint((frame->error_code & 0x01) ? "[Page Present Protection Violation]" : "[Page Not Present]", WHITE);
        kprint((frame->error_code & 0x02) ? "[Write Access]" : "[Read Access]", WHITE);
        kprint("[Kernel]", WHITE);
        kprint("\n", WHITE);
    }

    /* CPU Register Dump from ISR Frame */
    kprint("  Frame-Registers=\n", WHITE);
    kprint("  rip=", WHITE); kprint_hex(frame->rip, PURPLE);
    kprint("  cs=", WHITE); kprint_hex(frame->cs, PURPLE); kprint("\n", WHITE);
    kprint("  rsp=", WHITE); kprint_hex(frame->rsp, PURPLE);
    kprint("  ss=", WHITE); kprint_hex(frame->ss, PURPLE); kprint("\n", WHITE);
    kprint("  rflags=", WHITE); kprint_hex(frame->rflags, PURPLE); kprint("\n\n", WHITE);

    kprint("System Halted. Please reboot.\n", WHITE);
    while (1)
        __asm__ volatile ("hlt");
}

__attribute__((interrupt)) void
divide_by_zero(void *frame)
{
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	(void)int_frame;
	clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #0: Divide by zero\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while (1)
        __asm__ volatile ("hlt");    
}

__attribute__((interrupt)) void
debug(void *frame)
{
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	(void)int_frame;clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #1: Debug Interrupt\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
non_maskable(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #2: Non maskable interrupt\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
break_point(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #3: Breakpoint interrupt\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
overflow(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #4: Stack Overflow\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
bound_range_exceeded(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #5: Bound Range Exceeded\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
invalid_opcode(void *frame)
{
	panic_execption("Invalid Opcode", 6, (struct interrupt_frame *)frame, 0);    
};

__attribute__((interrupt)) void
device_not_available(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #7: Device not available\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
double_fault(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Double Fault!\n", PURPLE);
    kprint("Interrupt #8 | Error Code: ", WHITE); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
invalid_tss(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #10: Invalid TSS | Error Code: ", WHITE); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
segement_not_present(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #11: Segment not present\n", WHITE);
    kprint("Error Code: ", WHITE); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
stack_segfault(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Stack Segmentation Fault!\n", PURPLE);
    kprint("Interrupt #12 | Error Code: ", WHITE); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
general_protection(void *frame)
{
	uint64_t cr2;
	__asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
	panic_execption("General Protection Fault", 13, (struct interrupt_frame *)frame, 0);
};

__attribute__((interrupt)) void
page_fault(void *frame)
{
	uint64_t cr2;
	__asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
	panic_execption("Page Fault", 14, (struct interrupt_frame *)frame, 0);
};

__attribute__((interrupt)) void
x87_fpu_error(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("x87 FPU Error\n", GREEN);
    kprint("Interrupt #16\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
alignment_check(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #17: Alignment Check\n", WHITE);
    kprint("Error Code: ", WHITE); kprint_hex(int_frame->error_code, PURPLE);
    kprint("\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
machine_check(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #18: Machine Check\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

__attribute__((interrupt)) void
simd_error(void *frame)
{
    
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
    clear_screen();
    kprint("Kernel Panic\n", WHITE);
    kprint("Fatal exception in interrupt!\n", WHITE);
    kprint("Interrupt #19: SIMD Error\n", WHITE);
    kprint("System Halted. Please reboot\n", WHITE);
    while(1)
        __asm__ volatile ("hlt");
};

/* buffer for keyboard (driver) */
#define _BUFFER_SIZE 128

volatile char key_buffer[_BUFFER_SIZE]; /* actual */
volatile int buffer_head = 0;
volatile int buffer_tail = 0;

void
buffer_push(char c)
{
    int next_head = (buffer_head + 1) % _BUFFER_SIZE;

    /* prevent ring buffer overwrite if queue is full */
    if (next_head == buffer_tail)
        return;

    key_buffer[buffer_head] = c;
    buffer_head = next_head;
}

char
buffer_pop(void)
{
    /* check buffer bounds */
    if (buffer_head == buffer_tail)
        return 0;

    char c = key_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % _BUFFER_SIZE;
    return c;
}

/* special case keys */
int shift_pressed = 0;
int caps_lock = 0;
int ctrl_key = 0;
static int is_extended = 0;
/* the index in the scancde means no character */

/* current characters the CPU can read from the keyboard */
const char kdb_map[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

const char kdb_map_shift[] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

const char kdb_map_numpad[] = {
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

__attribute__((interrupt)) void
keyboard_handler(void *frame)
{
    struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
    (void)int_frame; /* i doubt it'll be used */

    uint8_t scancode = inb(0x60); /* read scancode from PS/2 port since it's a PS/2 Keyboard bus */

    /* handle extended scancode perfix */
    if (scancode == 0xe0)
    {
         is_extended = 1;
         lapic_eoi();
         return;   
    }

    /* handle break code */
    if (scancode & 0x80)
    {
        u8 release_code = scancode & 0x7f;
        if (release_code == 0x2a || release_code == 0x36)
        {
            shift_pressed = 0;
        }
        is_extended = 0;
    }

    /* handle make code (key press) */
    else {
        if (scancode == 0x2a || scancode == 0x36)
            shift_pressed = 1;
        else if (scancode == 0x3a) /* caps */
            caps_lock = !caps_lock;
        else if (scancode == 0x0e) /* \b */
            buffer_push('\b');
        else if (is_extended && scancode == 0x35) /* keypad '/' */
            buffer_push('/');
        else if (is_extended && scancode == 0x1c)
            buffer_push('\n');
        /* numpad digits/operators */
        else if (!is_extended && scancode >= 0x47 && scancode <= 0x53)
            buffer_push(kdb_map_numpad[scancode - 0x47]);
        else if (!is_extended && scancode < sizeof(kdb_map)) /* safety guard */
        {
            char c = kdb_map[scancode];
            if (c != 0)
            {
                if (shift_pressed)
                {
                    c = kdb_map_shift[scancode];
                    if (caps_lock && c >= 'A' && c <= 'Z') c += 32;
                } else if (caps_lock && c >= 'a' && c <= 'z') {
                    c -= 32;
                }
                buffer_push(c);
            }
            
        }
        is_extended = 0;
    }

    lapic_eoi(); /* always send! */
}

volatile uint64_t timer_ticks = 0;

__attribute__((interrupt)) void
timer_handler(void *frame)
{
	struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	timer_ticks++;
	lapic_eoi(); /* always send! */
}

void
sleep(uint64_t milliseconds)
{
	uint64_t target_ticks = timer_ticks + milliseconds;
	while (timer_ticks < target_ticks)
	{
		__asm__ volatile ("hlt");
	}
}

__attribute__((interrupt)) void
com1_handler(void *frame)
{
	struct interrupt_frame *int_frame = (struct interrupt_frame *)frame;
	(void)int_frame;
	
	uint8_t serial_char = inb(COM1);
	(void)serial_char;
	kprint("COM1 activity!\n", WHITE);
	lapic_eoi();
}
