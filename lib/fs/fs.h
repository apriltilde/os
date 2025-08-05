#ifndef FS_H
#define FS_H

#include <stdint.h>

// Initialize the basic filesystem directory (sector 1)
void writebasicfs(void);

// Add a filename-to-sector mapping to the directory ledger (sector 1)
// Does NOT write file data
void addfile(const char* filename, uint32_t data_sector);

// Read the file content for the given filename into out_buffer (512 bytes)
void readfs(const char* filename, uint8_t* out_buffer);

void delfile(const char* filename);

#endif // FS_H

