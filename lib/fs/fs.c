// fs.c

#include "../ata/ata.h"
#include "../core/string.h"

// Initialize the filesystem with an empty directory sector (sector 1)

void writebasicfs() {
    uint8_t buffer[512];
    for (int i = 0; i < 512; i++) buffer[i] = 0;

    // Write initial directory entries
    const char* init_str = "vars<0>;fs<1>;";
    int i = 0;
    while (init_str[i] && i < 511) {
        buffer[i] = (uint8_t)init_str[i];
        i++;
    }
    buffer[i] = 0; // null terminate

    write_sector(buffer, 1);
}


// Add a filename-sector mapping to the directory ledger (sector 1)
// Does NOT write any file data to the data sector
void addfile(const char* filename, uint32_t data_sector) {
    uint8_t dir[512];
    read_sector(dir, 1);

    // Find end of current directory string
    int pos = 0;
    while (pos < 512 && dir[pos] != 0) pos++;

    if (pos + 20 >= 512) return; // not enough space to add entry

    // Copy filename chars into dir buffer
    int i = 0;
    while (filename[i] && pos < 512 - 1) {
        dir[pos++] = filename[i++];
    }

    // Add '<'
    dir[pos++] = '<';

    // Convert data_sector to ASCII string
    char snum[12];
    int si = 0;
    uint32_t n = data_sector;

    if (n == 0) {
        snum[si++] = '0';
    } else {
        while (n > 0) {
            snum[si++] = '0' + (n % 10);
            n /= 10;
        }
        // Reverse snum
        for (int j = 0; j < si / 2; j++) {
            char tmp = snum[j];
            snum[j] = snum[si - j - 1];
            snum[si - j - 1] = tmp;
        }
    }

    // Append sector number string to dir buffer
    for (int j = 0; j < si && pos < 512; j++) {
        dir[pos++] = snum[j];
    }

    // Add closing '>' and separator ';'
    dir[pos++] = '>';
    dir[pos++] = ';';

    // Null-terminate if possible
    if (pos < 512) dir[pos] = 0;

    // Write back updated directory sector
    write_sector(dir, 1);
}

// Read the file data for given filename into out_buffer (512 bytes)
void readfs(const char* filename, uint8_t* out_buffer) {
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

        // Compare filename
        if (str_eq(name, filename)) {
            // Convert sector string to uint32_t
            uint32_t sector = 0;
            for (int j = 0; snum[j]; j++) {
                sector = sector * 10 + (snum[j] - '0');
            }
            // Read file data sector into out_buffer
            read_sector(out_buffer, sector);
            return;
        }
    }

    // File not found - zero out buffer
    for (int k = 0; k < 512; k++) {
        out_buffer[k] = 0;
    }
}

