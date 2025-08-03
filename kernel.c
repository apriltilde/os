#include "lib/core/print.h"
#include "lib/cmd.h"  // Include the command module

void kmain(void) {
	extern void init_paging(void);
	init_paging(); // before framebuffer access
    cmd_init(); // Initialize command module
    while (1) {
        cmd_handle_input(); // Handle input from the command interface
    }
}
