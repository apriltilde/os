#include "print.h"
#include "../cmd.h"
#include "../vga/vga.h"

extern const struct bitmap_font font;

#define VGA_ADDRESS 0xB8000       // Base address for VGA text mode
#define VGA_WIDTH 80              // Number of columns
#define VGA_HEIGHT 25             // Number of rows

unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;
int cursor_pos = 0;

// Function to clear the screen
void clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = ' ' | (WHITE << 8); // Fill with spaces and set color
    }
    cursor_pos = 0; // Reset cursor position to the top-left
}

// Function to scroll the screen up by one line
void scroll() {
    for (int i = 0; i < VGA_HEIGHT - 1; i++) {
        for (int j = 0; j < VGA_WIDTH; j++) {
            vga_buffer[i * VGA_WIDTH + j] = vga_buffer[(i + 1) * VGA_WIDTH + j];
        }
    }
    // Clear the last line after scrolling
    for (int j = 0; j < VGA_WIDTH; j++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + j] = ' ' | (WHITE << 8);
    }
}

// Function to move the cursor to the next line
void newline() {
    if (in_graphics_mode == 1) {
        // In graphics mode, move the cursor to the next line
        gfx_cursor_x = cmd_x_start; // Reset to start of command area
        gfx_cursor_y += font_char_height; // Move down one line
        return;
    }

    cursor_pos += VGA_WIDTH; // Move cursor to the start of the next line

    // Check if we have exceeded the screen height
    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        scroll();  // Scroll the screen up
        cursor_pos -= VGA_WIDTH; // Move the cursor back to the last line
    }

    // Ensure the cursor starts at the leftmost column of the new line
    cursor_pos -= (cursor_pos % VGA_WIDTH); // Reset to the start of the line
}

// Function to move the cursor back by one position
void backspace() {
    if (cursor_pos > 0) { // Ensure we don't go out of bounds
        cursor_pos--; // Move cursor back
    }
    vga_buffer[cursor_pos] = ' ' | (WHITE << 8);
}

// Function to print a single character
void print_char(int color, char c) {
    if (in_graphics_mode == 1) {
        // In graphics mode, use the putchar function
        putchar(gfx_cursor_x, gfx_cursor_y, c, &font, white);
        gfx_cursor_x += font_char_width; // Move cursor position
        return;
    }

    if (c == '\0') return; // Ignore null characters
    if (c == '\n') {
        newline(); // Call the newline function
    } else if (c == '\b') {
        backspace(); // Handle backspace
    } else {
        vga_buffer[cursor_pos] = (c | (color << 8));
        cursor_pos++;

        // Move to the next line if we reach the end of a row
        if (cursor_pos % VGA_WIDTH == 0) {
            newline(); // Move to the next line if at the end of the row
        }
    }
}

// Function to print a string with a specified color
void print(int color, const char *str) {
    if (in_graphics_mode == 1) {
        // In graphics mode, use the putstring function
        putstring(gfx_cursor_x, gfx_cursor_y, str, &font, white);
        gfx_cursor_x += strlen(str) * font_char_width; // Update cursor position
        return;
    }
    while (*str) {
        print_char(color, *str++);
    }
}

void print_hex(int color, uint16_t value) {
    const char* hex_chars = "0123456789ABCDEF";
    for (int i = 12; i >= 0; i -= 4) {
        print_char(color, hex_chars[(value >> i) & 0xF]);
    }
}

