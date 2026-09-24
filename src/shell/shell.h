#include "../vga/vga.h"
#include "../ints/handlers.h"
#include "../func/func.h"

#define CMD_MAX_LEN 256

static char cmd_buf[CMD_MAX_LEN]; /* command buffer */
static size_t cmd_idx = 0;

static void execute_command(const char *);
extern void shell_init(void);
extern void shell_update(void);
