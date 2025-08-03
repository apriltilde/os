#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_read_sector5_first8(uint8_t* buffer);
void ata_write_sector5_first8(void);

#endif // ATA_H
