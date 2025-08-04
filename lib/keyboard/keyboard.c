#include "../core/print.h"
#include "../core/io.h"
#include "../vga/vga.h"
#include "keyboard.h"// I/O ports for keyboard

#define KEYBOARD_PORT 0x60
#define STATUS_PORT 0x64

extern const struct bitmap_font font;

static int shift_pressed = 0;      // Track if shift is pressed
static int key_state[256] = {0};   // Track the state of each key
char last_char = '\0';              // Holds the last character pressed

// Function to read a byte from the keyboard
static unsigned char read_keyboard(void) {
    return inb(KEYBOARD_PORT);
}

// Function to check if the keyboard buffer is empty
static int is_keyboard_buffer_empty(void) {
    return (inb(STATUS_PORT) & 0x01) == 0; // Check bit 0
}

// Function to handle keyboard input
void keyboard_handler(void) {
    // Polling the keyboard status
    while (!is_keyboard_buffer_empty()) {
        unsigned char scancode = read_keyboard();
        char c = '\0'; // Initialize character to null

        if (scancode < 0x80) { // Only process key presses
            if (scancode < 0x80) {
                key_state[scancode] = 1; // Mark the key as pressed

                // Handle special keys
                switch (scancode) {
                    case 0x2A: // Left Shift Press
                    case 0x36: // Right Shift Press
                        shift_pressed = 1; // Set shift pressed flag
                        break;

                    // Character keys
                    case 0x1E: c = shift_pressed ? 'A' : 'a'; break;
                    case 0x30: c = shift_pressed ? 'B' : 'b'; break;
                    case 0x2E: c = shift_pressed ? 'C' : 'c'; break;
                    case 0x20: c = shift_pressed ? 'D' : 'd'; break;
                    case 0x12: c = shift_pressed ? 'E' : 'e'; break;
                    case 0x21: c = shift_pressed ? 'F' : 'f'; break;
                    case 0x22: c = shift_pressed ? 'G' : 'g'; break;
                    case 0x23: c = shift_pressed ? 'H' : 'h'; break;
                    case 0x17: c = shift_pressed ? 'I' : 'i'; break;
                    case 0x24: c = shift_pressed ? 'J' : 'j'; break;
                    case 0x25: c = shift_pressed ? 'K' : 'k'; break;
                    case 0x26: c = shift_pressed ? 'L' : 'l'; break;
                    case 0x32: c = shift_pressed ? 'M' : 'm'; break;
                    case 0x31: c = shift_pressed ? 'N' : 'n'; break;
                    case 0x18: c = shift_pressed ? 'O' : 'o'; break;
                    case 0x19: c = shift_pressed ? 'P' : 'p'; break;
                    case 0x10: c = shift_pressed ? 'Q' : 'q'; break;
                    case 0x13: c = shift_pressed ? 'R' : 'r'; break;
                    case 0x1F: c = shift_pressed ? 'S' : 's'; break;
                    case 0x14: c = shift_pressed ? 'T' : 't'; break;
                    case 0x16: c = shift_pressed ? 'U' : 'u'; break;
                    case 0x2F: c = shift_pressed ? 'V' : 'v'; break;
                    case 0x11: c = shift_pressed ? 'W' : 'w'; break;
                    case 0x2D: c = shift_pressed ? 'X' : 'x'; break;
                    case 0x15: c = shift_pressed ? 'Y' : 'y'; break;
                    case 0x2C: c = shift_pressed ? 'Z' : 'z'; break;

                    case 0x02: c = shift_pressed ? '!' : '1'; break; // 1
                    case 0x03: c = shift_pressed ? '"' : '2'; break; // 2
                    case 0x04: c = shift_pressed ? '#' : '3'; break; // 3
                    case 0x05: c = shift_pressed ? '$' : '4'; break; // 4
                    case 0x06: c = shift_pressed ? '%' : '5'; break; // 5
                    case 0x07: c = shift_pressed ? '^' : '6'; break; // 6
                    case 0x08: c = shift_pressed ? '&' : '7'; break; // 7
                    case 0x09: c = shift_pressed ? '*' : '8'; break; // 8
                    case 0x0A: c = shift_pressed ? '(' : '9'; break; // 9
                    case 0x0B: c = shift_pressed ? ')' : '0'; break; // 0
                   
                    case 0x0C: c = shift_pressed ? '_' : '-'; break; // -
		    case 0x0D: c = shift_pressed ? '+' : '='; break; // =
		    case 0x35: c = shift_pressed ? '?' : '/'; break; // /
		    case 0x27: c = shift_pressed ? ':' : ';'; break; // ;
		    
		    case 0x34: c = shift_pressed ? '>' : '.'; break; // .
		    case 0x33: c = shift_pressed ? '<' : ','; break; // ,
		    
			
                    case 0x39: c = ' '; break; // Space bar
                    case 0x1C: c = '\n'; break; // Enter key
                    case 0x0E: c = '\b'; break; // Backspace key
                    // Handle other special keys here (add more cases as needed)
                    default: c = '\0'; break; // Ignore other keys
                }

                // If a character was generated, store it in last_char
                if (c != '\0') {
                    last_char = c; // Update last_char
                }
            }
        } else {
            // Handle key releases (scancodes >= 0x80)
            unsigned char released_scancode = scancode - 0x80; // Convert to original scancode

            if (key_state[released_scancode] == 1) { // Check if the key was pressed
                key_state[released_scancode] = 0; // Mark the key as released

                switch (released_scancode) {
                    case 0x2A: // Left Shift Release
                    case 0x36: // Right Shift Release
                        shift_pressed = 0; // Clear shift pressed flag
                        break;

                    default:
                        break;
                }
            }
        }
    }
}


