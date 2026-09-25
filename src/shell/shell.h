//#include "../vga/vga.h"
#include "../ints/handlers.h"
#include "../func/func.h"

#define CMD_MAX_LEN 256

static char cmd_buf[CMD_MAX_LEN]; /* command buffer */
static size_t cmd_idx = 0;

static void execute_command(const char *);
extern void shell_init(void);
extern void shell_update(void);

extern uint64_t get_timer_ticks_in_minutes(uint64_t);
