#include "../core/print.h"
#include "../core/io.h"
#include "../core/string.h"
#include "../core/datatypes.h"

#include "../keyboard/keyboard.h"
#include "edit.h"
#include "../fs/fs.h"
#include "../ata/ata.h"

#include "../vga/vga.h"

#define BUFFER_SIZE 512
#define SECTOR_SIZE 512

extern struct bitmap_font font;

extern char input_buffer[BUFFER_SIZE];
extern int buffer_index;
extern void disable_cursor(void);

extern int extract_arguments(char* command, char args[][BUFFER_SIZE], int max_args, int max_len);
extern char last_char;


void edit(void) {
    char args[1][BUFFER_SIZE];
    int arg_count = extract_arguments("edit", args, 1, BUFFER_SIZE);
    uint8_t buffer[SECTOR_SIZE];


    void draw_buffer(void) {
        int line_num = 1;
        int pos = 0;
        int screen_y = 20;

int total_lines = 1;
for (int k = 0; k < SECTOR_SIZE && buffer[k] != 0; k++) {
    if (buffer[k] == '\n') total_lines++;
}


int paper_x = 400;
int paper_y = 6;
int paper_width = 300;    // same as before
int paper_height = total_lines * 12;   // same as before

// Fill background white
for (int i = 0; i < paper_width; i++) {
    for (int j = 0; j < paper_height; j++) {
        putpixel(paper_x + i, paper_y + j, white);
    }
}

// Draw horizontal black lines for each text row
// Assuming each row spacing = font.height * 2 (one blank line between)
int line_spacing =  12;
for (int y = paper_y + line_spacing; y < paper_y + paper_height; y += line_spacing) {
    for (int x = paper_x; x < paper_x + paper_width; x++) {
        putpixel(x, y, black);
    }
}

// Draw vertical red margin line after line numbers
// Example: place it at +40px from left edge of paper
int margin_x = paper_x + 12;
for (int y = paper_y; y < paper_y + paper_height; y++) {
    putpixel(margin_x, y, red);
}
        while (pos < SECTOR_SIZE && buffer[pos] != 0) {
            // Prepare a temporary string for line text
            char line_str[BUFFER_SIZE];
            int line_len = 0;

            while (pos < SECTOR_SIZE && buffer[pos] != 0 && buffer[pos] != '\n') {
                line_str[line_len++] = (char)buffer[pos];
                pos++;
            }
            line_str[line_len] = '\0';

            // Draw line number
            putint(400, screen_y, line_num, &font, black);

            // Draw line text starting a bit to the right
            putstring(420, screen_y, line_str, &font, black);

            // Move down two "lines" in screen coordinates
            screen_y += 12;


            // Skip the newline char if present
            if (pos < SECTOR_SIZE && buffer[pos] == '\n') {
                pos++;
            }

            line_num++;
        }
    }
    if (arg_count == 0) {
        print(WHITE, "\nUsage: edit <sector|/filename>\n");
        return;
    }

    uint32_t sector = str_to_int(args[0]);

    read_sector(buffer, sector);
	draw_buffer();

    print(WHITE, "\n[Editing sector ");
    print(WHITE, args[0]);
    print(WHITE, "]\nType 'exit' to save and quit.\n");

    last_char = '\0';

    // --- helper to draw entire buffer ---

    // --- end helper ---

    // Helper: find line start pointer by line number (1-based)
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
        return p;
    }

    // Helper: find line end pointer
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

        // Parse line number
        int line_num = 0;
        int i = 0;
        while (line[i] >= '0' && line[i] <= '9') {
            line_num = line_num * 10 + (line[i] - '0');
            i++;
        }
        while (line[i] == ' ') i++;

        if (line_num <= 0) {
            print(WHITE, "Invalid line number.\n");
            continue;
        }

        // Count existing lines
        int existing_lines = 1;
        for (int k = 0; k < SECTOR_SIZE && buffer[k] != 0; k++) {
            if (buffer[k] == '\n') existing_lines++;
        }

        // Fill missing lines if needed
        if (line_num > existing_lines) {
            int empty_lines_to_add = line_num - existing_lines;
            int used_size = 0;
            while (used_size < SECTOR_SIZE && buffer[used_size] != 0) used_size++;

            if (used_size + empty_lines_to_add >= SECTOR_SIZE) {
                print(WHITE, "Not enough space to add empty lines.\n");
                continue;
            }

            for (int e = 0; e < empty_lines_to_add; e++) {
                buffer[used_size++] = '\n';
            }
            if (used_size < SECTOR_SIZE) buffer[used_size] = 0;
        }

        // Edit the selected line
        uint8_t* line_start = get_line_start(buffer, line_num);
        uint8_t* line_end = get_line_end(line_start);

        int old_line_len = (int)(line_end - line_start);
        if (line_end < buffer + SECTOR_SIZE && *line_end == '\n') {
            old_line_len++;
            line_end++;
        }

        int new_line_len = 0;
        while (line[i + new_line_len] != '\0' && new_line_len < SECTOR_SIZE - 2) {
            new_line_len++;
        }
        new_line_len += 1;

        int used_size = 0;
        while (used_size < SECTOR_SIZE && buffer[used_size] != 0) used_size++;

        int new_used_size = used_size - old_line_len + new_line_len;
        if (new_used_size > SECTOR_SIZE) {
            print(WHITE, "Line too long to fit in buffer.\n");
            continue;
        }

        if (new_line_len != old_line_len) {
            if (new_line_len > old_line_len) {
                for (int k = used_size - 1; k >= (line_end - buffer); k--) {
                    buffer[k + (new_line_len - old_line_len)] = buffer[k];
                }
            } else {
                for (int k = (line_end - buffer); k < used_size; k++) {
                    buffer[k - (old_line_len - new_line_len)] = buffer[k];
                }
                for (int k = new_used_size; k < used_size; k++) {
                    buffer[k] = 0;
                }
            }
        }

        int pos = 0;
        for (; pos < new_line_len - 1; pos++) {
            buffer[(line_start - buffer) + pos] = (uint8_t)line[i + pos];
        }
        buffer[(line_start - buffer) + pos] = '\n';

        if (new_used_size < SECTOR_SIZE) {
            buffer[new_used_size] = 0;
        }

        // ✅ Refresh the on-screen file view
        draw_buffer();
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

