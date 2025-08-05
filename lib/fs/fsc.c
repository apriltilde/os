#include "fs.h"
#include "../core/string.h" // for str_eq, str_copy
#include "../core/print.h"// for print, print_hex, print_char, hex_to_uint32, etc.
#include "../ata/ata.h"   // for read_sector, write_sector, etc.

#define BUFFER_SIZE 512
#define SECTOR_SIZE 512

extern char input_buffer[BUFFER_SIZE];
extern int extract_arguments(const char *command, char args[][BUFFER_SIZE], int max_args, int max_len);

void readfsc_command(void) {
    char args[1][BUFFER_SIZE];
    int arg_count = extract_arguments("fs", args, 1, BUFFER_SIZE);

    uint8_t dir[512];
    read_sector(dir, 1);

    int i = 0;
    while (i < 512 && dir[i] != 0) {
        char name[64];
        int ni = 0;

        // Parse filename until '<'
        while (dir[i] != '<' && dir[i] != 0 && ni < 63) {
            name[ni++] = dir[i++];
        }
        name[ni] = 0;

        if (dir[i] != '<') break;
        i++; // skip '<'

        // Parse sector number string until '>'
        char snum[12];
        int si = 0;
        while (dir[i] != '>' && dir[i] != 0 && si < 11) {
            snum[si++] = dir[i++];
        }
        snum[si] = 0;

        if (dir[i] == '>') i++; // skip '>'

        if (dir[i] == ';') i++; // skip ';'
		if (arg_count > 0 && str_eq(args[0], ".num")) {
            print(WHITE, "<");
            print(WHITE, snum);
            print(WHITE, ">");
        }
        // Print output based on argument
        print(WHITE, name);
		print(WHITE, " | ");
    }
	newline();
}

void addfsc_command(void) {
    char args[2][BUFFER_SIZE];
    int arg_count = extract_arguments("create", args, 2, BUFFER_SIZE);

    if (arg_count < 2) {
        print(WHITE, "\nUsage: create <filename> <sector>\n");
        return;
    }

    // Convert sector string to uint32
    uint32_t sector = hex_to_uint32(args[1]);

    // Read current directory from sector 1
    uint8_t dir[SECTOR_SIZE];
    read_sector(dir, 1);

    int i = 0;
    while (i < SECTOR_SIZE && dir[i] != 0) {
        // Move to next entry (look for ';')
        while (i < SECTOR_SIZE && dir[i] != ';') {
            i++;
        }
        if (i < SECTOR_SIZE) i++; // Move past ';'
    }

    if (i >= SECTOR_SIZE - 1) {
        print(RED, "\nDirectory full. Cannot add more files.\n");
        return;
    }

    // Format: name<sector>;
    int ni = 0;
    while (args[0][ni] != 0 && i < SECTOR_SIZE - 1) {
        dir[i++] = args[0][ni++];
    }

    if (i >= SECTOR_SIZE - 12) {
        print(RED, "\nFilename too long or not enough space.\n");
        return;
    }

    dir[i++] = '<';

    // Convert sector number back to hex string
    char snum[12];
    uint32_to_hex(sector, snum);

    int si = 0;
    while (snum[si] != 0 && i < SECTOR_SIZE - 1) {
        dir[i++] = snum[si++];
    }

    dir[i++] = '>';
    dir[i++] = ';';
    dir[i] = 0; // Null-terminate directory

    // Write updated directory back to sector 1
    write_sector(dir, 1);

    print(GREEN, "\nFile added successfully.\n");
}

void delfsc_command(void) {
    char args[1][BUFFER_SIZE];
    int arg_count = extract_arguments("delete", args, 1, BUFFER_SIZE);

    if (arg_count < 1) {
		print(WHITE, "\nUsage: delete <filename>\n");
		return;
    }

    delfile(args[0]);
    print(GREEN, "\nFile deleted successfully.\n");
}
