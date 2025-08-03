#include "io.h"
#include <stdint.h>

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECCOUNT0   0x1F2
#define ATA_LBA0        0x1F3
#define ATA_LBA1        0x1F4
#define ATA_LBA2        0x1F5
#define ATA_DRIVE       0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

#define ATA_READ_CMD    0x20
#define ATA_WRITE_CMD   0x30

#define ATA_SR_BSY      0x80
#define ATA_SR_DRQ      0x08

static void ata_wait() {
    while (inb(ATA_STATUS) & ATA_SR_BSY);
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ));
}

void ata_read_sector5_first8(uint8_t* buffer) {
    ata_wait();

    outb(ATA_SECCOUNT0, 1);       // 1 sector
    outb(ATA_LBA0, 5 & 0xFF);     // LBA low
    outb(ATA_LBA1, (5 >> 8) & 0xFF);
    outb(ATA_LBA2, (5 >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((5 >> 24) & 0x0F)); // Master drive, LBA mode
    outb(ATA_COMMAND, ATA_READ_CMD);

    ata_wait();

    for (int i = 0; i < 4; i++) { // Read 4 words (8 bytes)
        uint16_t data = inw(ATA_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }
}

void ata_write_sector5_first8() {
    uint8_t dummy_data[8] = { 'D', 'U', 'M', 'M', 'Y', 'D', 'A', 'T' };

    ata_wait();

    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, 5 & 0xFF);
    outb(ATA_LBA1, (5 >> 8) & 0xFF);
    outb(ATA_LBA2, (5 >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((5 >> 24) & 0x0F));
    outb(ATA_COMMAND, ATA_WRITE_CMD);

    ata_wait();

    for (int i = 0; i < 4; i++) { // Write 4 words (8 bytes)
        uint16_t word = dummy_data[i * 2] | (dummy_data[i * 2 + 1] << 8);
        outw(ATA_DATA, word);
    }

    // Fill the rest of the sector (512 - 8 bytes = 504 bytes)
    for (int i = 0; i < 252; i++) {
        outw(ATA_DATA, 0x0000);
    }

    // Flush cache (optional, depending on device)
    ata_wait();
}
