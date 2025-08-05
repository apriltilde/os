#include "../core/print.h"
#include "../core/io.h"
#include "../core/string.h"
#include "../core/datatypes.h"

#include "../keyboard/keyboard.h"
#include "edit.h"
#include "../fs/fs.h"
#include "../ata/ata.h"

#define BUFFER_SIZE 512
#define SECTOR_SIZE 512

extern char input_buffer[BUFFER_SIZE];
extern int buffer_index;
extern void disable_cursor(void);

extern int extract_arguments(char* command, char args[][BUFFER_SIZE], int max_args, int max_len);
extern char last_char;

void edit(void) {
    char args[1][BUFFER_SIZE];
    int arg_count = extract_arguments("edit", args, 1, BUFFER_SIZE);

    if (arg_count == 0) {
        print(WHITE, "\nUsage: edit <sector|/filename>\n");
        return;
    }

    uint32_t sector = str_to_int(args[0]);

    uint8_t buffer[SECTOR_SIZE];
    read_sector(buffer, sector);

    print(WHITE, "\n[Editing sector ");
    print(WHITE, args[0]);
    print(WHITE, "]\nType 'exit' to save and quit.\n");

    last_char = '\0';

    // Helper: find line start in buffer by line number (1-based).
    // If line not found, returns pointer to end of buffer.
    uint8_t* get_line_start(uint8_t* buf, int line_num) {
        int current_line = 1;
        uint8_t* p = buf;

        if (line_num <= 1) return p;

        while (*p != 0 && p < buf + SECTOR_SIZE) {
            if (*p == '\n') {
                current_line++;
                p++;
                if (current_line == line_num) {
                    return p;
                }
                continue;
            }
            p++;
        }

        // If requested line is after the last line, return end of buffer
        return p;
    }

    // Helper: find end of line starting at p (or end of buffer)
    uint8_t* get_line_end(uint8_t* p) {
        while (*p != 0 && *p != '\n' && p < buffer + SECTOR_SIZE) {
            p++;
        }
        return p;
    }

    while (1) {
        print(LIGHT_GREEN, "> ");

        int input_len = 0;
        char line[BUFFER_SIZE];
        while (1) {
            keyboard_handler();
            disable_cursor();
            if (last_char != '\0') {
                if (last_char == '\n') {
                    line[input_len] = '\0';
                    newline();
                    last_char = '\0';
                    break;
                } else if (last_char == '\b') {
                    if (input_len > 0) {
                        input_len--;
                        print_char(WHITE, '\b');
                        print_char(WHITE, ' ');
                        print_char(WHITE, '\b');
                    }
                    last_char = '\0';
                } else {
                    if (input_len < BUFFER_SIZE - 1) {
                        line[input_len++] = last_char;
                        print_char(WHITE, last_char);
                    }
                    last_char = '\0';
                }
            }
        }

        if (str_eq(line, "exit")) {
            write_sector(buffer, sector);
            print(WHITE, "Saved and exited editor.\n");
            return;
        }

        // Parse line number at start of input line
        int line_num = 0;
        int i = 0;
        while (line[i] >= '0' && line[i] <= '9') {
            line_num = line_num * 10 + (line[i] - '0');
            i++;
        }

        // Skip spaces after number
        while (line[i] == ' ') i++;

        // Ignore if no valid line number
        if (line_num <= 0) {
            print(WHITE, "Invalid line number.\n");
            continue;
        }

        // Find where the new line should be written in buffer
        uint8_t* line_start = get_line_start(buffer, line_num);
        uint8_t* line_end = get_line_end(line_start);

        // Calculate length of new line content + newline
        int new_line_len = 0;
        while (line[i + new_line_len] != '\0' && new_line_len < SECTOR_SIZE - 2) {
            new_line_len++;
        }
        // +1 for newline character
        new_line_len += 1;

        // Calculate old line length (including possible '\n')
        int old_line_len = (int)(line_end - line_start);
        if (line_end < buffer + SECTOR_SIZE && *line_end == '\n') {
            old_line_len++; // include '\n'
            line_end++;
        }

        // Calculate total used size of buffer
        int used_size = 0;
        while (used_size < SECTOR_SIZE && buffer[used_size] != 0) used_size++;

        // Calculate new size after replacement
        int new_used_size = used_size - old_line_len + new_line_len;
        if (new_used_size > SECTOR_SIZE) {
            print(WHITE, "Line too long to fit in buffer.\n");
            continue;
        }

        // Shift buffer content after old line to make room or close gap
        if (new_line_len != old_line_len) {
            if (new_line_len > old_line_len) {
                // Need to move data forward (to right)
                for (int k = used_size - 1; k >= (line_end - buffer); k--) {
                    buffer[k + (new_line_len - old_line_len)] = buffer[k];
                }
            } else {
                // Move data backward (to left)
                for (int k = (line_end - buffer); k < used_size; k++) {
                    buffer[k - (old_line_len - new_line_len)] = buffer[k];
                }
                // Clear leftover bytes at the end
                for (int k = new_used_size; k < used_size; k++) {
                    buffer[k] = 0;
                }
            }
        }

        // Write new line content to buffer at line_start
        int pos = 0;
        for (; pos < new_line_len - 1; pos++) {
            buffer[(line_start - buffer) + pos] = (uint8_t)line[i + pos];
        }
        buffer[(line_start - buffer) + pos] = '\n'; // add newline terminator

        // Null terminate buffer if space available
        if (new_used_size < SECTOR_SIZE) {
            buffer[new_used_size] = 0;
        }
    }
}


void view(void) {
    char args[1][BUFFER_SIZE];
    int arg_count = extract_arguments("view", args, 1, BUFFER_SIZE);

    if (arg_count == 0) {
        print(WHITE, "\nUsage: view <sector|/filename>\n");
        return;
    }

    uint32_t sector = str_to_int(args[0]);

    uint8_t buffer[SECTOR_SIZE];
    read_sector(buffer, sector);

    print(WHITE, "\n[Viewing sector ");
    print(WHITE, args[0]);
    print(WHITE, "]\n");

    int line_num = 1;
    int pos = 0;

    while (pos < SECTOR_SIZE && buffer[pos] != 0) {
        // Print line number
        // Convert line_num to string manually (max 10 digits enough)
        char num_str[12];
        int num_len = 0;
        int temp = line_num;
        if (temp == 0) {
            num_str[num_len++] = '0';
        } else {
            // store digits reversed
            while (temp > 0) {
                num_str[num_len++] = '0' + (temp % 10);
                temp /= 10;
            }
            // reverse digits
            for (int i = 0; i < num_len / 2; i++) {
                char tmp = num_str[i];
                num_str[i] = num_str[num_len - i - 1];
                num_str[num_len - i - 1] = tmp;
            }
        }
        num_str[num_len] = '\0';

        print(LIGHT_GREEN, num_str);
        print(LIGHT_GREEN, " ");

        // Print line content until newline or null or end of buffer
        while (pos < SECTOR_SIZE && buffer[pos] != 0 && buffer[pos] != '\n') {
            char c = (char)buffer[pos];
            print_char(WHITE, c);
            pos++;
        }

        // Print newline after line content
        newline();

        // Skip newline if present
        if (pos < SECTOR_SIZE && buffer[pos] == '\n') {
            pos++;
        }

        line_num++;
    }
}

