#include "lib/print.h"
#include "lib/cmd.h"  // Include the command module

void kmain(void) {
    cmd_init(); // Initialize command module

    // Main loop to read input
    while (1) {
        cmd_handle_input(); // Handle input from the command interface
    }
}
