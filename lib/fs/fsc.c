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
            print(WHITE, "> | ");
        }
        // Print output based on argument
        print(WHITE, name);
        newline();
    }
}


