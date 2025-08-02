#include "lib/core/print.h"
#include "lib/cmd.h"  // Include the command module

void kmain(void) {
    cmd_init(); // Initialize command module
    while (1) {
        cmd_handle_input(); // Handle input from the command interface
    }
}
