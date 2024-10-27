#include "print.h"
#include "keyboard.h"
#include "print.h"
#include "keyboard.h"
#include "pit.h"
#include <stdint.h>
#include <stdlib.h>  // For atoi

#define BUFFER_SIZE 128
static char input_buffer[BUFFER_SIZE];
static int buffer_index = 0;

// Command structure to define a command and its handler
typedef struct {
    const char *name;               // Command name
    void (*handler)(void);          // Handler function for the command
} Command;

// Function prototypes
void cmd_init(void);
void cmd_handle_input(void);
void print_command(void);
void add_command(void);
void unknown_command(void);
void int_to_string(int num, char *str);
int match_command(const char *cmd);
void clear_command(void);
void sleep_command(void);  // Update to take no parameters
void beep_command(void);

// Array of available commands
static Command commands[] = {
    { "print ", print_command },   // "print " command with handler
    { "add ", add_command },       // "add " command with handler
    { "clear", clear_command },    // "clear" command with handler
    { "sleep ", sleep_command }, 
    { "beep", beep_command },
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

// Function to initialize the command module
void cmd_init(void) {
    buffer_index = 0; // Initialize buffer index
    print(WHITE, "Command Line Initialized.\n"); // Display a message on the screen
    print(WHITE, "$ "); // Print the initial prompt
}

void clear_command(void) {
    clear(); // Call the clear function from print.h to clear the screen
}

//Beep command function
void beep_command(void) {
    int freq = 0;
    int ms = 0;
    int is_freq_valid = 0;
    int is_ms_valid = 0;

    // Skip the command name "beep " (which is 5 characters)
    int i = 5; 

    // Extract frequency
    while (i < buffer_index) {
        char ch = input_buffer[i];

        // If character is a digit, build the frequency number
        if (ch >= '0' && ch <= '9') {
            freq = freq * 10 + (ch - '0');
            is_freq_valid = 1; // Mark that we've encountered a valid digit
        } else if (ch == ' ') { // Move to the next part after the space
            i++;
            break; // Stop processing when we reach a space
        } else {
            // Invalid character encountered
            print(RED, "Invalid frequency input.\n");
            return; // Exit the command handler if input is invalid
        }
        i++;
    }

    // Extract duration in milliseconds
    while (i < buffer_index) {
        char ch = input_buffer[i];

        // If character is a digit, build the milliseconds number
        if (ch >= '0' && ch <= '9') {
            ms = ms * 10 + (ch - '0');
            is_ms_valid = 1; // Mark that we've encountered a valid digit
        } else if (ch == ' ' || ch == '\0') {
            break; // Stop processing if we reached the end of the number
        } else {
            // Invalid character encountered
            print(RED, "Invalid duration input.\n");
            return; // Exit the command handler if input is invalid
        }
        i++;
    }

    // Check if both frequency and duration are valid
    if (is_freq_valid && is_ms_valid) {
        beep(freq, ms); // Call the existing beep function from pit.h
        print(WHITE, "\n");
    } else {
        print(RED, "Invalid beep parameters.\n"); // Print error if parameters are not valid
    }
}

// Sleep command function
void sleep_command(void) {
    // Skip the command name "sleep " (which is 6 characters)
    int ms = 0;
    int is_number = 0;

    for (int i = 6; i < buffer_index; i++) {
        char ch = input_buffer[i];

        // If character is a digit, build the number
        if (ch >= '0' && ch <= '9') {
            ms = ms * 10 + (ch - '0');
            is_number = 1; // Mark that we've encountered a digit
        } else if (ch == ' ' || ch == '\0') {
            // If we encounter a space or null and we have a valid number
            break; // Stop processing if we reached the end of the number
        }
    }

    if (is_number) { // Proceed only if we have a valid positive number
        sleep(ms); // Call the existing sleep function from pit.h
        print(WHITE, "\n");
    } else {
        print(RED, "Invalid sleep duration.\n"); // Print error if ms is not valid
    }
}

// Function to handle the "print" command
void print_command(void) {
    char print_buffer[BUFFER_SIZE]; // Temporary buffer for the print command
    int print_index = 0;

    // Copy the string after "print " into print_buffer
    for (int i = 6; i < buffer_index; i++) {
        if (print_index < BUFFER_SIZE - 1) {
            print_buffer[print_index++] = input_buffer[i];
        }
    }
    print_buffer[print_index] = '\0'; // Null-terminate the string
    print(WHITE, "\n");
    print(WHITE, print_buffer); // Print the extracted string
    print(WHITE, "\n"); // Print a newline
}

// Helper function to convert an integer to a string
void int_to_string(int num, char *str) {
    int i = 0;
    int start = 0;
    char temp;

    // Handle zero case directly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Build the string backwards if the number is positive
    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string
    for (start = 0, i--; start < i; start++, i--) {
        temp = str[start];
        str[start] = str[i];
        str[i] = temp;
    }
}

// Function to handle the "add" command
void add_command(void) {
    int sum = 0;
    int num = 0;
    int valid_number = 0;

    // Start after "add " which is 4 characters long
    for (int i = 4; i <= buffer_index; i++) {
        char ch = input_buffer[i];

        // If character is a digit, build the number
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            valid_number = 1;
        } else if ((ch == ' ' || ch == '\0') && valid_number) {
            // End of a number, add to sum
            sum += num;
            num = 0;
            valid_number = 0;
        }
    }

    // Convert sum to string and print it
    char sum_buffer[BUFFER_SIZE];
    int_to_string(sum, sum_buffer); // Convert the integer sum to a string

    print(WHITE, "\n ");
    print(WHITE, sum_buffer); // Print the sum
    print(WHITE, "\n");
}

// Function to handle unknown commands
void unknown_command(void) {
    print(WHITE, "\n");
    print(WHITE, input_buffer); // Print the complete command
    print(WHITE, " command not found"); // Error message
    print(WHITE, "\n"); // Print a newline
}

// Function to check if input matches any known command
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

// Function to handle keyboard input for the command interface
void cmd_handle_input(void) {
    // Call the keyboard handler to get the latest keystroke
    keyboard_handler();

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
            print(WHITE, "$ "); // Print the prompt again
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
