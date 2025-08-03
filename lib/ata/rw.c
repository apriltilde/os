#include <stdint.h>
#include "../core/string.h" // for str_eq, str_copy
#include "rw.h"
#include "../core/print.h"// for print, print_hex, print_char, hex_to_uint32, etc.
#include "ata.h"   // for read_sector, write_sector, etc.

#define BUFFER_SIZE 512
#define SECTOR_SIZE 512

extern char input_buffer[BUFFER_SIZE];
extern int extract_arguments(const char *command, char args[][BUFFER_SIZE], int max_args, int max_len);

void readsec(void) {
    char args[1][BUFFER_SIZE];
    int count = extract_arguments("read", args, 1, BUFFER_SIZE);

    if (count != 1) {
        print(WHITE, "\nUsage: read <hex-sector>\n");
        return;
    }
    clear();
    uint32_t sector = hex_to_uint32(args[0]);
    uint8_t* buffer = (uint8_t*)0x7E00;

    print(BLUE, "\nReading sector ");
    print_hex(WHITE, sector);
    print(WHITE, "...\n");

    read_sector(buffer, sector);

    print(WHITE, "\n\nText (ASCII):\n");
    for (int i = 0; i < 512; i++) {
        char c = (char)buffer[i];
        if (c >= 32 && c <= 126) {
            print_char(WHITE, c);
        } else {
            print_char(WHITE, '.');
        }
    }

    print(GREEN, "\n\nRead complete.\n");
}

void writesec(void) {
    char args[11][BUFFER_SIZE];  // 1: sector, 2: start offset, 3+: data
    int count = extract_arguments("write", args, 50, BUFFER_SIZE);

    if (count < 3) {
        print(WHITE, "\nUsage: write <hex-sector> <start-byte> <data-string> [<data-string> ...]\n");
        return;
    }

    uint32_t sector = hex_to_uint32(args[0]);
    uint32_t start_byte = hex_to_uint32(args[1]);

    if (start_byte >= 512) {
        print(RED, "\nError: Start byte must be less than 512.\n");
        return;
    }

    uint8_t* buffer = (uint8_t*)0x7E00;

    // Load existing data in the sector before writing
    read_sector(buffer, sector);

    int buf_index = start_byte;

    for (int arg_index = 2; arg_index < count; arg_index++) {
        int j = 0;
        while (args[arg_index][j] != '\0' && buf_index < 512) {
            buffer[buf_index++] = (uint8_t)args[arg_index][j++];
        }
        if (arg_index < count - 1 && buf_index < 512) {
            buffer[buf_index++] = ' ';
        }
    }

    print(LIGHT_MAGENTA, "\nWriting to sector ");
    print_hex(WHITE, sector);
    print(WHITE, " starting at byte ");
    print_hex(WHITE, start_byte);
    print(WHITE, " with data: ");

    char temp_str[2] = {0, 0};
    for (int i = start_byte; i < buf_index; i++) {
        temp_str[0] = buffer[i];
        print(WHITE, temp_str);
    }

    print(WHITE, "\n");

    write_sector(buffer, sector);
}

void wipesec(void) {
    char args[1][BUFFER_SIZE];
    int count = extract_arguments("wipe", args, 1, BUFFER_SIZE);

    if (count != 1) {
        print(WHITE, "\nUsage: wipe <hex-sector>\n");
        return;
    }

    uint32_t sector = hex_to_uint32(args[0]);
    uint8_t* buffer = (uint8_t*)0x7E00;

    // Fill buffer with 0s
    for (int i = 0; i < 512; i++) {
        buffer[i] = 0;
    }

    print(RED, "\nWiping sector ");
    print_hex(WHITE, sector);
    print(WHITE, "...\n");

    write_sector(buffer, sector);

    print(GREEN, "Sector wiped successfully.\n");
}


// Extract variable value from sector 0
void var_extract(const char* var_name, char* output, int max_len) {
    uint8_t sector_data[SECTOR_SIZE];
    read_sector(sector_data, 0);

    int i = 0;
    while (i < SECTOR_SIZE) {
        // Skip any spaces, nulls, or garbage between entries
        while (i < SECTOR_SIZE && (sector_data[i] == ' ' || sector_data[i] == '\0')) {
            i++;
        }

        if (i >= SECTOR_SIZE) break;

        // Parse key
        char key[32];
        int k = 0;
        while (i < SECTOR_SIZE && sector_data[i] != '=' && sector_data[i] != ';' && k < 31) {
            key[k++] = sector_data[i++];
        }
        key[k] = '\0';

        if (i >= SECTOR_SIZE || sector_data[i] != '=') {
            // Skip malformed or partial entry
            while (i < SECTOR_SIZE && sector_data[i] != ';') i++;
            if (i < SECTOR_SIZE && sector_data[i] == ';') i++;
            continue;
        }

        i++; // Skip '='

        // Parse value
        char value[32];
        int v = 0;
        while (i < SECTOR_SIZE && sector_data[i] != ';' && v < 31) {
            value[v++] = sector_data[i++];
        }
        value[v] = '\0';

        if (i < SECTOR_SIZE && sector_data[i] == ';') i++;

        // Check match
        if (str_eq(key, var_name)) {
            str_copy(output, value, max_len);
            return;
        }
    }

    // Fallback if not found
    str_copy(output, var_name, max_len);
}


