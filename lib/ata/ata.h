#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void read(void);
void write(void);

void read_sector(uint8_t* buffer, uint32_t lba);
void write_sector(const uint8_t* buffer, uint32_t lba);

uint32_t hex_to_uint32(const char *hex);
void uint32_to_hex(uint32_t value, char *out);
#endif // ATA_H
