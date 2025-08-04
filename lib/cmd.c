#include "core/print.h"
#include "core/io.h"
#include "core/string.h"

#include "keyboard/keyboard.h"
#include "mem/emem.h"
#include "clock/clock.h"
#include "math/math.h"
#include "ata/ata.h"
#include "ata/rw.h"
#include "fs/fs.h"
#include "fs/fsc.h"
#include "vga/vga.h"

#include <stdint.h>

#define BUFFER_SIZE 512
#define SECTOR_SIZE 512


static char input_buffer[BUFFER_SIZE];
static int buffer_index = 0;

// Command structure to define a command and its handler
typedef struct {
    const char *name;               // Command name
    void (*handler)(void);          // Handler function for the command
	const char *category;			// Command category
} Command;

void cmd_init(void);
void cmd_handle_input(void);
int extract_arguments(const char *command, char args[][BUFFER_SIZE], int max_args, int max_len);


void help_command(void);
void clear_command(void);
void print_command(void);
void graphicsmode_command(void);

void peek_command(void);
void poke_command(void);

void clock_command(void);
void date_command(void);

void add_command(void);
void sub_command(void);
void mul_command(void);
void div_command(void);

void write_command(void);
void read_command(void);
void wipe_command(void);

void readfs_command(void);
void addfs_command(void);

static Command commands[] = {
    { "help", help_command, "system" },
    { "clear", clear_command, "system" },
    { "print", print_command, "system" },
    { "peek", peek_command, "memory" },
    { "poke", poke_command, "memory" },
    { "time", clock_command, "system" },
    { "date", date_command, "system" },
	{ "add", add_command, "math" },
	{ "sub", sub_command, "math" },
	{ "mul", mul_command, "math" },
	{ "div", div_command, "math" },
    { "write",  write_command, "disk" },
    { "read", read_command, "disk" },
    { "wipe", wipe_command, "disk" },
	{ "fs", readfs_command, "disk" },
    { "create", addfs_command, "disk" },
	{ "graphicsmode", graphicsmode_command, "graphics" }
};


#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))


void cmd_init(void) {
    buffer_index = 0; // Initialize buffer index
	//writebasicfs(); // DO THIS IF YOU FUCK IT ALL UP
    print(WHITE, "APRILos\n"); // Display a message on the screen
    print(LIGHT_RED, "$ "); // Print the initial prompt
}

void graphicsmode_command(void) {
	initvideo();
}

void addfs_command(void) {
    addfsc_command();
}

void readfs_command(void) {
	readfsc_command();
}

void read_command(void) {
	readsec();
}

void write_command(void) {
	writesec();
}

void wipe_command(void) {
	wipesec();
}

//Basic commands
void help_command(void) {
    print(WHITE, "\nAvailable commands:");

    const char* last_category = 0;

    for (int i = 0; i < NUM_COMMANDS; i++) {
        const char* category = commands[i].category;

        if (last_category == 0 || !str_eq(category, last_category)) {
            print(LIGHT_CYAN, "\n\n== ");
            print(LIGHT_CYAN, category);
            print(LIGHT_CYAN, " ==\n");
            last_category = category;
        }

        print(WHITE, " ");
        print(WHITE, commands[i].name);
    }

    print(WHITE, "\n");
}



void clear_command(void) {
    clear(); // Call the clear function from print.h to clear the screen
}

void unknown_command(void) {
    print(WHITE, "\n");
    print(WHITE, input_buffer); // Print the complete command
    print(WHITE, " command not found"); // Error message
    print(WHITE, "\n"); // Print a newline
}

void print_command(void) {
    char args[1][BUFFER_SIZE]; // Only expect 1 argument: the string to print
    int arg_count = extract_arguments("print", args, 1, BUFFER_SIZE);

    if (arg_count == 0) {
        print(WHITE, "\nUsage: print <string>\n");
        return;
    }

    print(WHITE, "\n");
    print(WHITE, args[0]);
	print(WHITE, "\n");
}



//Math

void add_command(void);
void sub_command(void);
void mul_command(void);
void div_command(void);

//Memory commands

void peek_command(void);
void poke_command(void);


void clock_command(void) {
    print_time();
}

void date_command(void) {
    print_date();
}


void disable_cursor(void) {
    // Disable the cursor by writing to VGA registers
    // Set cursor start and end position to the same value
    outb(0x3D4, 0x0A); // Cursor start
    outb(0x3D5, 0x20); // Set start to 32 (invisible)

    outb(0x3D4, 0x0B); // Cursor end
    outb(0x3D5, 0x20); // Set end to 32 (invisible)
}

int extract_arguments(const char *command, char args[][BUFFER_SIZE], int max_args, int max_len) {
    int cmd_len = 0;
    while (command[cmd_len] != '\0') cmd_len++;

    int i = cmd_len;
    while (input_buffer[i] == ' ') i++; // Skip space after command

    int arg_count = 0;
    while (arg_count < max_args && input_buffer[i] != '\0') {
        int j = 0;

        // Copy one argument
        while (input_buffer[i] != ' ' && input_buffer[i] != '\0' && j < max_len - 1) {
            args[arg_count][j++] = input_buffer[i++];
        }
        args[arg_count][j] = '\0';

        // Handle :var syntax
        if (args[arg_count][0] == ':') {
            char resolved_value[BUFFER_SIZE];
            var_extract(args[arg_count] + 1, resolved_value, max_len);
            str_copy(args[arg_count], resolved_value, max_len);
            print(WHITE, "\b");
        }

        // Handle /filename -> replace with sector number
        else if (args[arg_count][0] == '/') {
            char filename[BUFFER_SIZE];
            str_copy(filename, args[arg_count] + 1, BUFFER_SIZE); // Skip '/'

            uint8_t dir[512];
            read_sector(dir, 1);

            int di = 0;
            while (di < 512 && dir[di] != 0) {
                char name[64];
                int ni = 0;

                while (dir[di] != '<' && dir[di] != 0 && ni < 63) {
                    name[ni++] = dir[di++];
                }
                name[ni] = '\0';

                if (dir[di] != '<') break;
                di++;

                char snum[12];
                int si = 0;
                while (dir[di] != '>' && dir[di] != 0 && si < 11) {
                    snum[si++] = dir[di++];
                }
                snum[si] = '\0';

                if (dir[di] == '>') di++;
                if (dir[di] == ';') di++;

                if (str_eq(name, filename)) {
                    // Use decimal sector string directly (no conversion needed)
                    str_copy(args[arg_count], snum, max_len);
					print(WHITE, "\b");
                    break;
                }
            }
        }

        arg_count++;

        // Skip spaces before next argument
        while (input_buffer[i] == ' ') i++;
    }

    return arg_count;
}


int match_command(const char *cmd) {
    for (int i = 0; i < NUM_COMMANDS; i++) {
        const char *name = commands[i].name;
        int j = 0;

        // Check if the command prefix matches input
        while (name[j] != '\0' && input_buffer[j] == name[j]) {
            j++;
        }

        // If the entire command prefix matches
        if (name[j] == '\0') {
            return i; // Return the command index
        }
    }
    return -1; // No matching command found
}


void cmd_handle_input(void) {
    // Call the keyboard handler to get the latest keystroke
    keyboard_handler();
    disable_cursor();

    // Using the globally defined last_char variable from keyboard.c
    extern char last_char;

    // If last_char is not null, process it
    if (last_char != '\0') {
        if (last_char == '\n') { // If Enter is pressed
            input_buffer[buffer_index] = '\0'; // Null-terminate the input buffer

            // Attempt to find and execute a matching command
            int cmd_index = match_command(input_buffer);
            if (cmd_index != -1) {
                commands[cmd_index].handler(); // Call the command handler
            } else {
                unknown_command(); // Handle unknown command
            }

            buffer_index = 0; // Reset buffer index for next command
            print(LIGHT_RED, "$ "); // Print the prompt again
        } else if (last_char == '\b') { // If Backspace is pressed
            if (buffer_index > 0) { // Ensure there's something to delete
                buffer_index--; // Move buffer index back
                input_buffer[buffer_index] = '\0'; // Null-terminate the buffer

                // Overwrite the last character on the screen
                print_char(WHITE, '\b'); // Move cursor back visually
                print_char(WHITE, ' '); // Clear the last character visually
                print_char(WHITE, '\b'); // Move cursor back again
            }
        } else {
            if (buffer_index < BUFFER_SIZE - 1) { // Avoid overflow
                input_buffer[buffer_index++] = last_char; // Add character to buffer
                input_buffer[buffer_index] = '\0'; // Null-terminate the buffer
                print_char(WHITE, last_char); // Print character on the screen
            }
        }

        // Clear last_char after processing
        last_char = '\0';
    }
}
